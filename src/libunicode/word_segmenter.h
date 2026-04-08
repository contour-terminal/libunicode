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

#include <libunicode/codepoint_properties.h>

#include <string_view>

namespace unicode
{

/// State for incremental word boundary detection per UAX #29.
struct word_segmenter_state
{
    /// Raw WB property of the immediately previous codepoint (before WB4 filtering).
    /// Used for WB3 (CR x LF), WB3a, WB3b, WB3c.
    Word_Break raw_prev_wb = Word_Break::Other;

    /// Effective WB property of the most recent non-transparent codepoint (after WB4 filtering).
    /// Used for rules WB5-WB16.
    Word_Break prev_wb = Word_Break::Other;

    /// Effective WB property of the codepoint before prev_wb.
    /// Needed for WB7, WB7c, WB11.
    Word_Break prev_prev_wb = Word_Break::Other;

    /// Regional Indicator counter (modulo 2) for WB15/WB16.
    uint8_t ri_counter = 0;

    /// Whether any transparent (Extend/Format/ZWJ) characters have been seen
    /// since the last non-transparent codepoint. Needed for WB3d which only
    /// matches directly adjacent WSegSpace characters.
    bool saw_transparent = false;
};

/// Initializes word segmentation state for the first codepoint.
void word_process_init(char32_t firstCodepoint, word_segmenter_state& state) noexcept;

/// Tests if there is a word boundary before @p nextCodepoint.
///
/// @param nextCodepoint The codepoint to test.
/// @param rest Pointer to codepoints after @p nextCodepoint (for lookahead).
/// @param restCount Number of codepoints available after @p nextCodepoint.
/// @param state Mutable segmentation state, updated on each call.
///
/// @retval true  There is a word boundary before @p nextCodepoint.
/// @retval false No word boundary; @p nextCodepoint belongs to the same word segment.
bool word_process_breakable(char32_t nextCodepoint, char32_t const* rest, size_t restCount, word_segmenter_state& state) noexcept;

/// Implements https://www.unicode.org/reports/tr29/#Word_Boundary_Rules
class word_segmenter
{
  public:
    word_segmenter(char32_t const* begin, char32_t const* end) noexcept:
        left_ { begin }, right_ { begin }, end_ { end }, state_ {}
    {
        ++*this;
    }

    word_segmenter(std::u32string_view sv) noexcept: word_segmenter(sv.data(), sv.data() + sv.size()) {}

    word_segmenter() noexcept: word_segmenter(static_cast<char32_t const*>(nullptr), static_cast<char32_t const*>(nullptr)) {}

    word_segmenter& operator++() noexcept
    {
        left_ = right_;
        if (right_ == end_)
            return *this;

        word_process_init(*right_++, state_);

        while (right_ != end_)
        {
            auto const* rest = right_ + 1;
            auto const restCount = static_cast<size_t>(end_ - rest);
            if (word_process_breakable(*right_, rest, restCount, state_))
                break;
            ++right_;
        }

        return *this;
    }

    constexpr std::u32string_view operator*() const noexcept
    {
        return std::u32string_view(left_, static_cast<size_t>(right_ - left_));
    }

    constexpr bool codepointsAvailable() const noexcept { return right_ != end_; }

    explicit constexpr operator bool() const noexcept { return codepointsAvailable(); }

    constexpr bool empty() const noexcept { return size() == 0; }
    constexpr size_t size() const noexcept { return static_cast<size_t>(right_ - left_); }

    constexpr bool operator==(word_segmenter const& rhs) const noexcept
    {
        return (!codepointsAvailable() && !rhs.codepointsAvailable()) || (left_ == rhs.left_ && right_ == rhs.right_);
    }

  private:
    char32_t const* left_;
    char32_t const* right_;
    char32_t const* end_;
    word_segmenter_state state_;
};

} // namespace unicode
