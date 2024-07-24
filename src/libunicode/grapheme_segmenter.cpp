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
#include <libunicode/utf8_grapheme_segmenter.h>

namespace unicode
{

void grapheme_process_init(char32_t nextCodepoint, grapheme_segmenter_state& state) noexcept
{
    auto const Pb = codepoint_properties::get(nextCodepoint);
    auto const B = Pb.grapheme_cluster_break;

    state.previousCodepoint = nextCodepoint;
    state.previousProperties = codepoint_properties::get(nextCodepoint);
    state.ri_counter = (B == Grapheme_Cluster_Break::Regional_Indicator) ? 1 : 0;
}

bool grapheme_process_breakable(char32_t nextCodepoint, grapheme_segmenter_state& state) noexcept
{
    auto const a = state.previousCodepoint;
    auto const Pa = state.previousProperties;
    auto const A = Pa.grapheme_cluster_break;

    auto const b = nextCodepoint;
    auto const Pb = codepoint_properties::get(b);
    auto const B = Pb.grapheme_cluster_break;

    state.previousCodepoint = b;
    state.previousProperties = Pb;

    static constexpr char32_t CR = 0x000D; // NOLINT
    static constexpr char32_t LF = 0x000A; // NOLINT

    {
        // Set state.ri_counter to zero if the next codepoint is not of category Regional_Indicator.
        //
        // We move the state.ri_counter out to help GCC optimize
        // this code to be branchless.
        // Sadly only GCC succeeds in doing this and Clang fails.
        auto const ri_counter = state.ri_counter;
        state.ri_counter = (B == Grapheme_Cluster_Break::Regional_Indicator) ? ri_counter : 0;
    }

    // GB3: Do not break between a CR and LF. Otherwise, break before and after controls.
    if (a == CR && b == LF)
        return false;

    // GB4 (a) + GB5 (b) part 1 (C0 characers) + US-ASCII shortcut
    // The US-ASCII part is a pure optimization improving performance
    // in standard Latin text.
    if (a < 128 && b < 128)
        return true;

    // GB4: (part 2)
    if (A == Grapheme_Cluster_Break::Control)
        return true;

    // GB5: (part 2)
    if (B == Grapheme_Cluster_Break::Control)
        return true;

    // Do not break Hangul syllable sequences.
    // GB6:
    if (A == Grapheme_Cluster_Break::L
        && (B == Grapheme_Cluster_Break::L || B == Grapheme_Cluster_Break::V || B == Grapheme_Cluster_Break::LV
            || B == Grapheme_Cluster_Break::LVT))
        return false;

    // GB7:
    if ((A == Grapheme_Cluster_Break::LV || A == Grapheme_Cluster_Break::V)
        && (B == Grapheme_Cluster_Break::V || B == Grapheme_Cluster_Break::T))
        return false;

    // GB8:
    if ((A == Grapheme_Cluster_Break::LV || A == Grapheme_Cluster_Break::T) && B == Grapheme_Cluster_Break::T)
        return false;

    // GB9: Do not break before extending characters.
    if (B == Grapheme_Cluster_Break::Extend || B == Grapheme_Cluster_Break::ZWJ)
        return false;

    // GB9a: Do not break before SpacingMarks
    if (B == Grapheme_Cluster_Break::SpacingMark)
        return false;

    // GB9b: or after Prepend characters.
    if (A == Grapheme_Cluster_Break::Prepend)
        return false;

    // GB11: Do not break within emoji modifier sequences or emoji zwj sequences.
    if (A == Grapheme_Cluster_Break::ZWJ && Pb.extended_pictographic())
        return false;

    // GB12/GB13: Do not break within emoji flag sequences.
    // That is, do not break between regional indicator (RI) symbols
    // if there is an odd number of RI characters before the break point.
    if (A == Grapheme_Cluster_Break::Regional_Indicator && A == B && state.ri_counter == 1)
    {
        state.ri_counter = static_cast<uint8_t>((state.ri_counter + 1) % 2);
        return false;
    }

    // GB999: Otherwise, break everywhere.
    return true; // GB10
}

} // namespace unicode
