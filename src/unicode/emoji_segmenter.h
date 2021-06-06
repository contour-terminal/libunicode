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

#include <unicode/ucd.h>
#include <unicode/support.h>

#include <algorithm>
#include <array>
#include <ostream>
#include <utility>

namespace unicode {

/// Used to distinguish between standard text and emoji text.
enum class PresentationStyle {
    Text,
    Emoji
};

/**
 * emoji_segmenter API for segmenting emojis into text-emoji and emoji-emoji presentations.
 *
 * This segmenter is segmenting emojis by their presentation property (text or emoji), that is,
 * whether an emoji is to be rendered in text mode or in emoji (colored) mode.
 *
 * It must be segmenting only emojis and not any other codepoints.
 */
class emoji_segmenter {
  private:
    char32_t const* buffer_ = U"";
    size_t size_ = 0;

    size_t currentCursorBegin_ = 0;
    size_t currentCursorEnd_ = 0;
    size_t nextCursorBegin_ = 0;

    bool isEmoji_ = false;
    bool isNextEmoji_ = false;

  public:
    using property_type = PresentationStyle;

    constexpr emoji_segmenter() noexcept = default;
    constexpr emoji_segmenter& operator=(emoji_segmenter const&) noexcept = default;
    constexpr emoji_segmenter& operator=(emoji_segmenter&&) noexcept = default;
    constexpr emoji_segmenter(emoji_segmenter const&) noexcept = default;
    constexpr emoji_segmenter(emoji_segmenter&&) noexcept = default;

    emoji_segmenter(char32_t const* _buffer, size_t _size) noexcept;

    emoji_segmenter(std::u32string_view const& _sv) noexcept
      : emoji_segmenter(_sv.data(), _sv.size())
    {}

    constexpr char32_t const* buffer() const noexcept { return buffer_; }
    constexpr size_t size() const noexcept { return size_; }
    constexpr size_t currentCursorBegin() const noexcept { return currentCursorBegin_; }
    constexpr size_t currentCursorEnd() const noexcept { return currentCursorEnd_; }

    bool consume(out<size_t> _size, out<PresentationStyle> _emoji) noexcept;

    /// @returns whether or not the currently segmented emoji is to be rendered in text-presentation or not.
    constexpr bool isText() const noexcept { return !isEmoji_; }

    /// @returns whether or not the currently segmented emoji is to be rendered in emoji-presentation or not.
    constexpr bool isEmoji() const noexcept { return isEmoji_; }

    /// @returns the underlying current segment that has been processed the last.
    constexpr std::u32string_view substr() const noexcept
    {
        // TODO: provide such an accessor in text_run_segmenter
        if (currentCursorEnd_ > 0)
            return std::u32string_view(buffer_ + currentCursorBegin_, currentCursorEnd_ - currentCursorBegin_);
        else
            return std::u32string_view{};
    }

    /// @returns the underlying current segment that has been processed the last.
    constexpr std::u32string_view operator*() const noexcept { return substr(); }

  private:
    size_t consume_once();
};

inline std::ostream& operator<<(std::ostream& os, PresentationStyle ps)
{
    switch (ps)
    {
        case PresentationStyle::Text:
            return os << "Text";
        case PresentationStyle::Emoji:
            return os << "Emoji";
    }
    return os;
}

} // end namespace
