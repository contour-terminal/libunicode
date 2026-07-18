/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2020 Christian Parpart <christian@parpart.family>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <libunicode/codepoint_properties.h>
#include <libunicode/convert.h>
#include <libunicode/grapheme_segmenter.h>
#include <libunicode/ucd.h>
#include <libunicode/width.h>

#include <string>
#include <string_view>

namespace unicode
{

unsigned width(char32_t codepoint) noexcept
{
    return codepoint_properties::get(codepoint).char_width;
}

unsigned grapheme_cluster_width(std::u32string_view cluster) noexcept
{
    if (cluster.empty())
        return 0;

    if (cluster.size() == 1)
        return width(cluster[0]);

    auto total = 0u;   // width of the segments already closed off
    auto current = 0u; // width of the segment being accumulated
    auto previousWasVirama = false;
    auto regionalIndicators = 0u;
    auto previousIsEmojiModifierBase = false;

    for (size_t i = 0; i < cluster.size(); ++i)
    {
        auto const cp = cluster[i];

        // A flag is a PAIR of regional indicators rendered as one glyph, so the second of each pair
        // adds nothing.
        if (auto const props = codepoint_properties::get(cp);
            props.grapheme_cluster_break == Grapheme_Cluster_Break::Regional_Indicator)
        {
            ++regionalIndicators;
            if (regionalIndicators % 2 == 0)
                continue;
        }
        else
            regionalIndicators = 0;

        // A skin tone modifier is absorbed by the emoji it modifies rather than placed beside it.
        if (cp >= 0x1F3FB && cp <= 0x1F3FF && previousIsEmojiModifierBase)
            continue;

        if (cp == 0x200D) // ZWJ
        {
            // A ZWJ consumes the codepoint that follows it, which is why an emoji ZWJ sequence is
            // measured by its first segment rather than by its widest member. After a virama the ZWJ
            // is merely a joining hint, so the consonant behind it must still be seen.
            if (!previousWasVirama && i + 1 < cluster.size())
                ++i;
            continue;
        }

        if (cp == 0xFE0F && current != 0) // VS16: emoji presentation
        {
            current = 2;
            previousWasVirama = false;
            continue;
        }

        if (cp == 0xFE0E && current == 2) // VS15: text presentation
        {
            current = 1;
            previousWasVirama = false;
            continue;
        }

        auto const properties = codepoint_properties::get(cp);
        if (auto const w = static_cast<unsigned>(properties.char_width); w != 0)
        {
            if (previousWasVirama)
                // A consonant joined to the previous one through a virama: the conjunct they form is
                // wider than one cell, but never wider than two.
                current = 2;
            else if (current != 0)
            {
                total += current;
                current = w;
            }
            else
                current = w;
            previousWasVirama = false;
            previousIsEmojiModifierBase = properties.is_emoji_modifier_base();
        }
        else if (properties.is_virama())
            previousWasVirama = true;
        else if (properties.general_category == General_Category::Spacing_Mark && current != 0)
        {
            // A spacing mark takes room of its own next to its base, unlike a non-spacing one.
            current = 2;
            previousWasVirama = false;
        }
        else
            previousWasVirama = false;
    }

    return total + current;
}

unsigned grapheme_cluster_width(std::string_view utf8Text) noexcept
{
    auto const u32 = convert_to<char32_t>(utf8Text);
    if (u32.empty())
        return 0;

    auto totalWidth = 0u;
    auto segmenter = grapheme_segmenter(std::u32string_view(u32));
    totalWidth += grapheme_cluster_width(*segmenter);
    while (segmenter.codepointsAvailable())
    {
        ++segmenter;
        totalWidth += grapheme_cluster_width(*segmenter);
    }
    return totalWidth;
}

} // namespace unicode
