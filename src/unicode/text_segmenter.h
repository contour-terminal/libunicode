/**
 * This file is part of the "libterminal" project
 *   Copyright (c) 2019 Christian Parpart <christian@parpart.family>
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

#include <unicode/script_segmenter.h>
#include <unicode/emoji_segmenter.h>
#include <unicode/ucd.h>
#include <unicode/support.h>

#include <iterator>

namespace unicode {

enum class FontFallbackPriority {
    Invalid = 0,
    Text,
    EmojiInText,
    EmojiInEmoji,
};

struct segment
{
    size_t start = 0;
    size_t end = 0;

    Script script = Script::Unknown;

    bool emoji = false;
    FontFallbackPriority fontFallbackPriority = FontFallbackPriority::Text;
};

class text_segmenter {
  public:
    text_segmenter(char32_t const* _text, size_t _size, size_t _startOffset = 0);

    text_segmenter(std::u32string_view const& _sv, size_t _startOffset = 0) :
        text_segmenter(_sv.data(), _sv.size(), _startOffset) {}

    /// Splits input text into segments, such as pure text by script, emoji-emoji, or emoji-text.
    ///
    /// @retval true more data can be processed
    /// @retval false end of input data has been reached.
    bool consume(out<segment> _result);

  private:
    template <typename Segmenter, typename Property>
    void consumeUntilSplitPosition(Segmenter& _segmenter,
                                   out<size_t> _position,
                                   out<Property> _property);

    constexpr bool finished() const noexcept { return lastSplit_ == size_; }

  private:
    size_t startOffset_ = 0;
    size_t lastSplit_ = 0;

    //size_t wordRunPosition_ = 0; // TODO
    size_t scriptRunPosition_ = 0;
    size_t emojiRunPosition_ = 0;

    segment candidate_{};

    size_t size_;
    script_segmenter scriptSegmenter_;
    emoji_segmenter emojiSegmenter_;
};

} // end namespace
