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
#pragma once

#include <unicode/codepoint_properties_data.h>
#include <unicode/ucd.h>

#include <string_view>

namespace unicode
{

/// Implements http://www.unicode.org/reports/tr29/tr29-27.html#Grapheme_Cluster_Boundary_Rules
class grapheme_segmenter
{
  private:
    static constexpr char32_t CR = 0x000D;   // NOLINT
    static constexpr char32_t LF = 0x000A;   // NOLINT
    static constexpr char32_t ZWJ = 0x200D;  // NOLINT
    static constexpr char32_t ZWNJ = 0x200C; // NOLINT

  public:
    constexpr grapheme_segmenter(char32_t const* _begin, char32_t const* _end) noexcept:
        left_ { _begin }, right_ { _begin }, end_ { _end }
    {
        ++*this;
    }

    constexpr grapheme_segmenter(std::u32string_view const& _sv) noexcept:
        grapheme_segmenter(_sv.data(), _sv.data() + _sv.size())
    {
    }

    constexpr grapheme_segmenter() noexcept: grapheme_segmenter({}, {}) {}

    constexpr grapheme_segmenter& operator++()
    {
        left_ = right_;

        while (right_ != end_ && nonbreakable(*right_, *(right_ + 1)))
            ++right_;

        if (right_ != end_)
            ++right_; // points to the codepoint after the last nonbreakable codepoint.

        return *this;
    }

    constexpr std::u32string_view operator*() const noexcept
    {
        return std::u32string_view(left_, static_cast<size_t>(right_ - left_));
    }

    constexpr bool codepointsAvailable() const noexcept { return right_ != end_; }

    constexpr operator bool() const noexcept { return codepointsAvailable(); }

    constexpr bool operator==(grapheme_segmenter const& _rhs) const noexcept
    {
        return (!codepointsAvailable() && !_rhs.codepointsAvailable())
               || (left_ == _rhs.left_ && right_ == _rhs.right_);
    }

    /// Tests if codepoint @p a and @p b are breakable, and thus, two different grapheme clusters.
    ///
    /// @retval true both codepoints to not belong to the same grapheme cluster
    /// @retval false both codepoints belong to the same grapheme cluster
    static bool breakable(char32_t a, char32_t b) noexcept
    {
        // GB3: Do not break between a CR and LF. Otherwise, break before and after controls.
        if (a == CR && b == LF)
            return false;

        // GB4 (a) + GB5 (b) part 1 (C0 characers) + US-ASCII shortcut
        // The US-ASCII part is a pure optimization improving performance
        // in standard Latin text.
        if (a < 128 && b < 128)
            return true;

        auto const Pa = codepoint_properties::get(a);
        auto const Pb = codepoint_properties::get(b);

        // GB4: (part 2)
        if (control(a, Pa))
            return true;

        // GB5: (part 2)
        if (control(b, Pb))
            return true;

        auto const A = Pa.grapheme_cluster_break;
        auto const B = Pb.grapheme_cluster_break;

        // Do not break Hangul syllable sequences.
        // GB6:
        if (A == Grapheme_Cluster_Break::L
            && (B == Grapheme_Cluster_Break::L || B == Grapheme_Cluster_Break::V
                || B == Grapheme_Cluster_Break::LV || B == Grapheme_Cluster_Break::LVT))
            return false;

        // GB7:
        if ((A == Grapheme_Cluster_Break::LV || A == Grapheme_Cluster_Break::V)
            && (B == Grapheme_Cluster_Break::V || B == Grapheme_Cluster_Break::T))
            return false;

        // GB8:
        if ((A == Grapheme_Cluster_Break::LV || A == Grapheme_Cluster_Break::T)
            && B == Grapheme_Cluster_Break::T)
            return false;

        // GB9: Do not break before extending characters.
        if (extend(b, Pb) || b == ZWJ) // GB9
            return false;

        // GB9a: Do not break before SpacingMarks
        if (spacing_mark(b, Pb))
            return false;

        // GB9b: or after Prepend characters.
        if (prepend(a))
            return false;

        // GB11: Do not break within emoji modifier sequences or emoji zwj sequences.
        if (a == ZWJ && extended_pictographic(b))
            return false;

        // GB12/GB13: Do not break within emoji flag sequences.
        // That is, do not break between regional indicator (RI) symbols
        // if there is an odd number of RI characters before the break point.
        if (A == Grapheme_Cluster_Break::Regional_Indicator
            || B == Grapheme_Cluster_Break::Regional_Indicator)
            return false;

        // GB999: Otherwise, break everywhere.
        return true; // GB10
    }

    static bool nonbreakable(char32_t a, char32_t b) noexcept { return !breakable(a, b); }

  private:
    static bool extend(char32_t codepoint, codepoint_properties const& properties) noexcept
    {
        return properties.core_grapheme_extend()
               || properties.general_category == General_Category::Spacing_Mark
               || (properties.emoji_modifier() && codepoint != ZWJ);
    }

    static bool control(char32_t ch, codepoint_properties const& properties) noexcept
    {
        // clang-format off
        return properties.general_category == General_Category::Line_Separator
            || properties.general_category == General_Category::Paragraph_Separator
            || properties.general_category == General_Category::Control
            || properties.general_category == General_Category::Surrogate
            || (properties.general_category == General_Category::Unassigned && contains(Core_Property::Default_Ignorable_Code_Point, ch))
            || (properties.general_category == General_Category::Format && ch != CR && ch != LF && ch != ZWNJ && ch != ZWJ);
        // clang-format on
    }

    static bool spacing_mark(char32_t codepoint, codepoint_properties const& properties) noexcept
    {
        return properties.general_category == General_Category::Spacing_Mark || codepoint == 0x0E33
               || codepoint == 0x0EB3;
    }

    static constexpr bool prepend([[maybe_unused]] char32_t codepoint) noexcept
    {
        // (NB: wrt "Prepend": Currently there are no characters with this value)
        // return contains(General_Category::Pepend, _codepoint)
        return false;
    }

  private:
    char32_t const* left_;
    char32_t const* right_;
    char32_t const* end_;
};

} // namespace unicode
