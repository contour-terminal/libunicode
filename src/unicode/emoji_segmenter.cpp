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

#include <unicode/emoji_segmenter.h>
#include <unicode/ucd.h>

#include <cassert>
#include <iostream>

namespace unicode
{

enum class EmojiSegmentationCategory
{
    Invalid = -1,

    Emoji = 0,
    EmojiTextPresentation = 1,
    EmojiEmojiPresentation = 2,
    EmojiModifierBase = 3,
    EmojiModifier = 4,
    EmojiVSBase = 5,
    RegionalIndicator = 6,
    KeyCapBase = 7,
    CombiningEnclosingKeyCap = 8,
    CombiningEnclosingCircleBackslash = 9,
    ZWJ = 10,
    VS15 = 11,
    VS16 = 12,
    TagBase = 13,
    TagSequence = 14,
    TagTerm = 15,
};

inline EmojiSegmentationCategory toCategory(char32_t _codepoint) noexcept
{
    auto isEmojiKeycapBase = [](char32_t _codepoint) constexpr noexcept->bool
    {
        return ('0' <= _codepoint && _codepoint <= '9') || _codepoint == '#' || _codepoint == '*';
    };

    if (_codepoint == 0x20e3)
        return EmojiSegmentationCategory::CombiningEnclosingKeyCap;
    if (_codepoint == 0x20e0)
        return EmojiSegmentationCategory::CombiningEnclosingCircleBackslash;
    if (_codepoint == 0x200d)
        return EmojiSegmentationCategory::ZWJ;
    if (_codepoint == 0xfe0e)
        return EmojiSegmentationCategory::VS15;
    if (_codepoint == 0xfe0f)
        return EmojiSegmentationCategory::VS16;
    if (_codepoint == 0x1f3f4)
        return EmojiSegmentationCategory::TagBase;
    if ((_codepoint >= 0xE0030 && _codepoint <= 0xE0039) || (_codepoint >= 0xE0061 && _codepoint <= 0xE007A))
        return EmojiSegmentationCategory::TagSequence;
    if (_codepoint == 0xE007F)
        return EmojiSegmentationCategory::TagTerm;
    if (emoji_modifier_base(_codepoint))
        return EmojiSegmentationCategory::EmojiModifierBase;
    if (emoji_modifier(_codepoint))
        return EmojiSegmentationCategory::EmojiModifier;
    if (grapheme_cluster_break::regional_indicator(_codepoint))
        return EmojiSegmentationCategory::RegionalIndicator;
    if (isEmojiKeycapBase(_codepoint))
        return EmojiSegmentationCategory::KeyCapBase;
    if (emoji_presentation(_codepoint))
        return EmojiSegmentationCategory::EmojiEmojiPresentation;
    if (emoji(_codepoint) && !emoji_presentation(_codepoint))
        return EmojiSegmentationCategory::EmojiTextPresentation;
    if (emoji(_codepoint))
        return EmojiSegmentationCategory::Emoji;

    return EmojiSegmentationCategory::Invalid;
}

class RagelIterator
{
    EmojiSegmentationCategory category_;
    char32_t const* buffer_;
    size_t size_;
    size_t currentCursorEnd_;

  public:
    RagelIterator(char32_t const* _buffer, size_t _size, size_t _cursor) noexcept:
        category_ { EmojiSegmentationCategory::Invalid },
        buffer_ { _buffer },
        size_ { _size },
        currentCursorEnd_ { _cursor }
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
            category_ = toCategory(codepoint());
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

    RagelIterator operator+(int v) const noexcept
    {
        // TODO: assert() on integer overflow
        return { buffer_, size_, currentCursorEnd_ + static_cast<size_t>(v) };
    }

    RagelIterator operator-(int v) const noexcept
    {
        if (v >= 0)
        {
            assert(currentCursorEnd_ >= static_cast<size_t>(v));
            return { buffer_, size_, currentCursorEnd_ - static_cast<size_t>(v) };
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

    constexpr bool operator==(RagelIterator const& _rhs) const noexcept
    {
        return buffer_ == _rhs.buffer_ && size_ == _rhs.size_ && currentCursorEnd_ == _rhs.currentCursorEnd_;
    }

    constexpr bool operator!=(RagelIterator const& _rhs) const noexcept { return !(*this == _rhs); }
};

namespace
{
    using emoji_text_iter_t = RagelIterator;
#include "emoji_presentation_scanner.c"
} // namespace

emoji_segmenter::emoji_segmenter(char32_t const* _buffer, size_t _size) noexcept:
    buffer_ { _buffer }, size_ { _size }
{
    if (size_)
        consume_once();
}

bool emoji_segmenter::consume(out<size_t> _size, out<PresentationStyle> _emoji) noexcept
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

    _size.assign(currentCursorEnd_);
    _emoji.assign(isEmoji_ ? PresentationStyle::Emoji : PresentationStyle::Text);
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
