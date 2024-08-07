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

#include <string_view>

namespace unicode
{

class word_segmenter
{
  public:
    using char_type = char32_t;
    using iterator = char_type const*;
    using view_type = std::basic_string_view<char_type>;

    constexpr word_segmenter(std::basic_string_view<char_type> const& str): word_segmenter(str.data(), str.data() + str.size()) {}

    constexpr word_segmenter(): word_segmenter({}, {}) {}

    constexpr bool empty() const noexcept { return size() == 0; }
    constexpr std::size_t size() const noexcept { return static_cast<size_t>(_right - _left); }
    constexpr view_type operator*() const noexcept { return view_type(_left, size()); }

    constexpr word_segmenter& operator++() noexcept
    {
        _left = _right;
        while (_right != _end)
        {
            switch (_state)
            {
                case State::NoWord:
                    if (!isDelimiter(*_right))
                    {
                        _state = State::Word;
                        return *this;
                    }
                    break;
                case State::Word:
                    if (isDelimiter(*_right))
                    {
                        _state = State::NoWord;
                        return *this;
                    }
                    break;
            }
            ++_right;
        }
        return *this;
    }

    constexpr bool operator==(word_segmenter const& rhs) const noexcept { return _left == rhs._left && _right == rhs._right; }

    constexpr bool operator!=(word_segmenter const& rhs) const noexcept { return !(*this == rhs); }

  private:
    constexpr word_segmenter(iterator begin, iterator end):
        _left { begin },
        _right { begin },
        _state { begin != end ? (isDelimiter(*_right) ? State::NoWord : State::Word) : State::NoWord },
        _end { end }
    {
        ++*this;
    }

    constexpr bool isDelimiter(char_type character) const noexcept
    {
        switch (character)
        {
            case ' ':
            case '\r':
            case '\n':
            case '\t': return true;
            default: return false;
        }
    }

    // private fields
    //
    enum class State
    {
        Word,
        NoWord
    };

    iterator _left;
    iterator _right;
    State _state;
    iterator _end;
};

} // namespace unicode
