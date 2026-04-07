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

    auto const baseWidth = width(cluster[0]);
    for (auto const cp: cluster.substr(1))
    {
        if (cp == 0xFE0F) // VS16: emoji presentation
            return 2;
        if (cp == 0xFE0E) // VS15: text presentation
            return 1;
    }
    return baseWidth;
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
