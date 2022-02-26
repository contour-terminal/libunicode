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

#include <unicode/emoji_segmenter.h>
#include <unicode/script_segmenter.h>
#include <unicode/support.h>
#include <unicode/ucd.h>
#include <unicode/ucd_ostream.h>

#include <array>
#include <iterator>
#include <ostream>
#include <tuple>

namespace unicode
{

template <typename T>
using segmenter_property_t = typename T::property_type;

template <typename... Ts>
using segmenter_property_tuple = std::tuple<segmenter_property_t<Ts>...>;

namespace detail
{
    template <typename Prepend, typename... Ts>
    inline void _continuePrintList(std::ostream& os, Prepend const& prep, std::tuple<Ts...> const& p)
    {
        ((os << prep << std::get<Ts>(p)), ...);
    }
} // namespace detail

/// API for segmenting incoming text into small runs.
///
/// A ``run`` is a unit suitable for text shaping, but may as well be used
/// for other purposes, too.
///
/// @see script_segmenter
/// @see emoji_segmenter
/// @see grapheme_segmenter
template <typename... Segmenter>
class basic_run_segmenter
{
  public:
    using property_tuple = std::tuple<segmenter_property_t<Segmenter>...>;

    /// Contains the extracted information of run_segmenter's single run.
    struct range
    {
        /// start-offset of the current segment that has been extracted
        size_t start = 0;

        /// end-offset (excluding) of the current segment that has been extracted
        size_t end = 0;

        /// the script (writing system) this segment has been identified with
        /// presentation style of the underlying segment
        property_tuple properties;

        constexpr bool operator==(range other) const noexcept
        {
            return start == other.start && end == other.end && properties == other.properties;
        }

        constexpr bool operator!=(range other) const noexcept { return !(*this == other); }

        friend inline std::ostream& operator<<(std::ostream& os, range r)
        {
            os << '(' << r.start << ".." << r.end;
            detail::_continuePrintList(os, ", ", r.properties);
            os << ')';
            return os;
        }
    };

    explicit basic_run_segmenter(std::u32string_view _sv): basic_run_segmenter(_sv.data(), _sv.size()) {}

    basic_run_segmenter(char32_t const* _text, size_t _size): segmenter_ {}, size_ { _size }
    {
        initialize<0, Segmenter...>(_text, _size);
    }

    constexpr bool finished() const noexcept { return lastSplit_ >= size_; }

    /// Splits input text into segments, such as pure text by script, emoji-emoji, or emoji-text.
    ///
    /// @retval true more data can be processed
    /// @retval false end of input data has been reached.
    bool consume(out<range> _result)
    {
        if (finished())
            return false;

        consumeAllUntilSplitPosition<0, Segmenter...>();

        auto const minPosition = std::min_element(begin(positions_), end(positions_));

        lastSplit_ = *minPosition;

        candidate_.start = candidate_.end;
        candidate_.end = lastSplit_;
        candidate_.properties = properties_;

        *_result = candidate_;
        return true;
    }

  private:
    template <size_t I>
    void initialize(char32_t const*, size_t)
    {
    }

    template <size_t I, typename Current, typename... Remaining>
    void initialize(char32_t const* _text, size_t _size)
    {
        std::get<I>(segmenter_) = Current { _text, _size };
        initialize<I + 1, Remaining...>(_text, _size);
    }

    template <size_t I>
    void consumeAllUntilSplitPosition()
    {
    }

    template <size_t I, typename Current, typename... Remaining>
    void consumeAllUntilSplitPosition()
    {
        consumeUntilSplitPosition(
            std::get<Current>(segmenter_), out(positions_[I]), out(std::get<I>(properties_)));
        consumeAllUntilSplitPosition<I + 1, Remaining...>();
    }

    template <typename TheSegmenter, typename Property>
    void consumeUntilSplitPosition(TheSegmenter& _segmenter, out<size_t> _position, out<Property> _property)
    {
        if (*_position > lastSplit_)
            return;

        if (*_position >= size_)
            return;

        for (;;)
        {
            if (!_segmenter.consume(_position, _property))
                break;

            if (*_position > lastSplit_)
                break;
        }
    }

  private:
    using position_list = std::array<size_t, sizeof...(Segmenter)>;
    using segmenter_tuple = std::tuple<Segmenter...>;

    size_t lastSplit_ = 0;
    range candidate_ = {};
    position_list positions_ {};
    property_tuple properties_ {};
    segmenter_tuple segmenter_;
    size_t const size_;
};

using run_segmenter = basic_run_segmenter<script_segmenter, emoji_segmenter>;

} // namespace unicode
