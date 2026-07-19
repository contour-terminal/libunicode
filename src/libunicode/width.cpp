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

namespace
{
    /// wcwidth's _EMOJI_ZWJ_SET: every Extended_Pictographic plus the 26 regional indicators.
    bool joinsEmojiSequence(char32_t codepoint) noexcept
    {
        auto const props = codepoint_properties::get(codepoint);
        return props.is_extended_pictographic() || props.grapheme_cluster_break == Grapheme_Cluster_Break::Regional_Indicator;
    }
} // namespace

void grapheme_cluster_width_accumulator::push(char32_t codepoint) noexcept
{
    auto const properties = codepoint_properties::get(codepoint);

    // A flag is a PAIR of regional indicators rendered as one glyph, and which half this codepoint is
    // depends on how many regional indicators sit *immediately* before it. That run length is
    // therefore maintained for every codepoint, including the ones the branches below return early
    // on: a variation selector or a ZWJ ends the run, so the indicator after it opens a new pair.
    // wcwidth reads the run off the raw input rather than off what it processed, which is why even a
    // codepoint that the ZWJ below swallows still counts toward it.
    auto const regionalIndicatorsBefore = _regionalIndicators;
    _regionalIndicators =
        properties.grapheme_cluster_break == Grapheme_Cluster_Break::Regional_Indicator ? _regionalIndicators + 1 : 0;

    // A ZWJ consumes the codepoint that follows it, which is why an emoji ZWJ sequence is measured
    // by its first segment rather than by its widest member.
    if (_skipNext)
    {
        _skipNext = false;
        return;
    }

    if (codepoint == 0x200D) // ZWJ
    {
        // After a virama the ZWJ is merely a joining hint, so the consonant behind it must still be
        // seen. A ZWJ that ends the cluster consumes nothing, which is what _skipNext expiring
        // unused amounts to.
        if (!_previousWasVirama)
        {
            _skipNext = true;
            _lastMeasuredWidth = 0;
        }
        return;
    }

    // VS16 requests the emoji presentation, which is two columns -- but only for a base an emoji
    // variation sequence is actually defined for. A variation selector after ordinary text (`x`, a
    // bullet, an em dash) is not a presentation request and must not widen anything.
    if (codepoint == 0xFE0F && _lastMeasuredIsOpen)
    {
        // The base's OWN width decides this, not the width recorded when it was measured: a ZWJ
        // zeroes the latter, and wcwidth resolves VS16 purely by looking the base up in its
        // narrow-to-wide table without consulting it. That table is encoded here as the variation
        // base flag plus a base width of one.
        if (auto const base = codepoint_properties::get(_lastMeasured); base.is_emoji_variation_base() && base.char_width == 1)
            _current = 2;
        _lastMeasuredIsOpen = false; // prevent a second application
        return;
    }

    // VS15 requests the text presentation, which is one column. Again this is meaningful only for a
    // defined variation base: it must not narrow a CJK ideograph, a Hangul jamo, or an Indic cluster
    // that a spacing mark or a conjunct widened.
    //
    // Deliberately NOT symmetric with VS16 above, in two ways that both mirror wcwidth: this branch
    // does read the recorded width (so a ZWJ that zeroed it suppresses the narrowing), and it does
    // not mark the base consumed (so a repeated VS15 decrements again, underflowing the cluster to
    // zero). Both are pinned by tests; see repeated_vs15_underflows_like_wcwidth.
    if (codepoint == 0xFE0E && _lastMeasuredIsOpen)
    {
        if (codepoint_properties::get(_lastMeasured).is_emoji_variation_base() && _lastMeasuredWidth == 2)
            --_total;
        return;
    }

    // The second indicator of a pair is drawn into the flag the first one opened, so it adds nothing.
    if (properties.grapheme_cluster_break == Grapheme_Cluster_Break::Regional_Indicator && regionalIndicatorsBefore % 2 == 1)
    {
        _lastMeasured = codepoint;
        return;
    }

    // A skin tone modifier is absorbed by the emoji it modifies rather than placed beside it. The
    // base need not be Emoji_Modifier_Base: an ill-formed but perfectly real sequence such as a
    // heart or a copyright sign followed by a modifier still renders as one glyph.
    if (codepoint >= 0x1F3FB && codepoint <= 0x1F3FF && joinsEmojiSequence(_lastMeasured))
        return;

    if (auto const w = static_cast<int>(properties.char_width); w != 0)
    {
        if (_previousWasVirama)
            // A consonant joined to the previous one through a virama: the conjunct they form is
            // wider than one cell, but never wider than two.
            _current = 2;
        else if (_current != 0)
        {
            _total += _current;
            _current = w;
        }
        else
            _current = w;

        _lastMeasured = codepoint;
        _lastMeasuredWidth = w;
        _lastMeasuredIsOpen = true;
        _previousWasVirama = false;
    }
    else if (properties.is_virama())
        _previousWasVirama = true;
    else if (properties.general_category == General_Category::Spacing_Mark && _lastMeasuredIsOpen)
    {
        // A spacing mark takes room of its own next to its base, unlike a non-spacing one.
        _current = 2;
        _lastMeasuredIsOpen = false;
        _previousWasVirama = false;
    }
    else
        _previousWasVirama = false;
}

unsigned grapheme_cluster_width_accumulator::width() const noexcept
{
    auto const total = _total + _current;
    return total > 0 ? static_cast<unsigned>(total) : 0u;
}

unsigned grapheme_cluster_width(std::u32string_view cluster) noexcept
{
    if (cluster.empty())
        return 0;

    if (cluster.size() == 1)
        return width(cluster[0]);

    auto accumulator = grapheme_cluster_width_accumulator {};
    for (auto const codepoint: cluster)
        accumulator.push(codepoint);
    return accumulator.width();
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
