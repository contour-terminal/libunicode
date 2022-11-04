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

/// Grapheme segmentation state struct, used to keep state
/// while processing each Unicode codepoint,
/// allow proper processing of regional flags
/// as well as reducing the number of invocations
/// to codepoint_properties::get().
struct grapheme_segmenter_state
{
    char32_t previousCodepoint = {};
    codepoint_properties previousProperties = codepoint_properties::get(0);

    uint8_t ri_counter = 0; // modulo 2
};

void grapheme_process_init(char32_t nextCodepoint, grapheme_segmenter_state& state) noexcept;

/// Tests if codepoint @p a and @p b are breakable, and thus, two different grapheme clusters.
///
/// @retval true both codepoints to not belong to the same grapheme cluster
/// @retval false both codepoints belong to the same grapheme cluster
bool grapheme_process_breakable(char32_t nextCodepoint, grapheme_segmenter_state& state) noexcept;

/// Implements http://www.unicode.org/reports/tr29/tr29-27.html#Grapheme_Cluster_Boundary_Rules
class grapheme_segmenter
{
  public:
    grapheme_segmenter(char32_t const* begin, char32_t const* end) noexcept:
        left_ { begin }, right_ { begin }, end_ { end }, state_ {}
    {
        ++*this;
    }

    grapheme_segmenter(std::u32string_view sv) noexcept:
        grapheme_segmenter(sv.data(), sv.data() + sv.size())
    {
    }

    grapheme_segmenter() noexcept: grapheme_segmenter({}, {}) {}

    grapheme_segmenter& operator++() noexcept
    {
        left_ = right_;
        if (right_ == end_)
            return *this;

        grapheme_process_init(*right_++, state_);

        while (right_ != end_ && !grapheme_process_breakable(*right_, state_))
            ++right_;

        return *this;
    }

    constexpr std::u32string_view operator*() const noexcept
    {
        return std::u32string_view(left_, static_cast<size_t>(right_ - left_));
    }

    constexpr bool codepointsAvailable() const noexcept { return right_ != end_; }

    constexpr operator bool() const noexcept { return codepointsAvailable(); }

    constexpr bool operator==(grapheme_segmenter const& rhs) const noexcept
    {
        return (!codepointsAvailable() && !rhs.codepointsAvailable())
               || (left_ == rhs.left_ && right_ == rhs.right_);
    }

    /// Tests if codepoint @p a and @p b are breakable, and thus, two different grapheme clusters.
    ///
    /// @retval true both codepoints to not belong to the same grapheme cluster
    /// @retval false both codepoints belong to the same grapheme cluster
    static bool breakable(char32_t a, char32_t b) noexcept
    {
        auto state = grapheme_segmenter_state {};
        state.previousCodepoint = a;
        state.previousProperties = codepoint_properties::get(a);
        state.ri_counter =
            (state.previousProperties.grapheme_cluster_break == Grapheme_Cluster_Break::Regional_Indicator)
                ? 1
                : 0;
        return grapheme_process_breakable(b, state);
    }

    static bool nonbreakable(char32_t a, char32_t b) noexcept { return !breakable(a, b); }

  private:
    char32_t const* left_;
    char32_t const* right_;
    char32_t const* end_;
    grapheme_segmenter_state state_;
};

} // namespace unicode
