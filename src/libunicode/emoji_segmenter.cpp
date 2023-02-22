/**
 * This file is part of the "libterminal" project
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

#include <libunicode/codepoint_properties_data.h>
#include <libunicode/emoji_segmenter.h>
#include <libunicode/ucd.h>

#include <cassert>
#include <iostream>

namespace unicode
{

namespace
{

    class RagelIterator
    {
        EmojiSegmentationCategory category_;
        char32_t const* buffer_;
        size_t size_;
        size_t currentCursorEnd_;

      public:
        RagelIterator(char32_t const* buffer, size_t size, size_t cursor) noexcept:
            category_ { EmojiSegmentationCategory::Invalid },
            buffer_ { buffer },
            size_ { size },
            currentCursorEnd_ { cursor }
        {
            updateCategory();
        }

        RagelIterator() noexcept: RagelIterator(U"", 0, 0) {}

        constexpr char32_t codepoint() const noexcept { return buffer_[currentCursorEnd_]; }
        constexpr EmojiSegmentationCategory category() const noexcept { return category_; }
        constexpr size_t cursor() const noexcept { return currentCursorEnd_; }

        void updateCategory() noexcept
        {
            if (currentCursorEnd_ < size_)
                category_ = codepoint_properties::get(codepoint()).emoji_segmentation_category;
            else
                category_ = EmojiSegmentationCategory::Invalid;
        }

        constexpr int operator*() const noexcept { return static_cast<int>(category_); }

        RagelIterator& operator++() noexcept
        {
            currentCursorEnd_++;
            updateCategory();
            return *this;
        }
        RagelIterator& operator--(int) noexcept
        {
            currentCursorEnd_--;
            updateCategory();
            return *this;
        }

        RagelIterator operator+(long v) const noexcept
        {
            // TODO: assert() on integer overflow
            return { buffer_, size_, currentCursorEnd_ + (size_t) v };
        }

        RagelIterator operator-(long v) const noexcept
        {
            if (v >= 0)
            {
                assert(currentCursorEnd_ >= static_cast<size_t>(v));
                return { buffer_, size_, currentCursorEnd_ - (size_t) v };
            }
            else
            {
                return *this + (-v);
            }
        }

        RagelIterator& operator=(int v) noexcept
        {
            assert(v >= 0);
            currentCursorEnd_ = static_cast<size_t>(v);
            updateCategory();
            return *this;
        }

        constexpr bool operator==(RagelIterator const& rhs) const noexcept
        {
            return buffer_ == rhs.buffer_ && size_ == rhs.size_ && currentCursorEnd_ == rhs.currentCursorEnd_;
        }

        constexpr bool operator!=(RagelIterator const& rhs) const noexcept { return !(*this == rhs); }
    };

    using emoji_text_iter_t = RagelIterator;

#include "emoji_presentation_scanner.c"
} // namespace

emoji_segmenter::emoji_segmenter(char32_t const* buffer, size_t size) noexcept:
    buffer_ { buffer }, size_ { size }
{
    if (size_)
        consume_once();
}

bool emoji_segmenter::consume(out<size_t> size, out<PresentationStyle> emoji) noexcept
{
    // 01234567890123456
    // "A EMOJI"
    //  []     |
    //   []    |
    //    [----]

    // "ABC EMOJI DEFGH"
    //  [---]    |     |
    //      [----]     |
    //           [-----]

    currentCursorBegin_ = currentCursorEnd_;
    currentCursorEnd_ = nextCursorBegin_;
    isEmoji_ = isNextEmoji_;

    if (nextCursorBegin_ >= size_)
        return false;

    do
    {
        auto const o = consume_once();

        if (isEmoji_ != isNextEmoji_)
        {
            nextCursorBegin_ = o;
            break;
        }

        currentCursorEnd_ = o;
    } while (currentCursorEnd_ < size_);

    size.assign(currentCursorEnd_);
    emoji.assign(isEmoji_ ? PresentationStyle::Emoji : PresentationStyle::Text);
    nextCursorBegin_ = currentCursorEnd_;

    return true;
}

size_t emoji_segmenter::consume_once()
{
    auto const i = RagelIterator(buffer_, size_, currentCursorEnd_);
    auto const e = RagelIterator(buffer_, size_, size_);
    auto const o = scan_emoji_presentation(i, e, &isNextEmoji_);
    return o.cursor();
}

} // namespace unicode
