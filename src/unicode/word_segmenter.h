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

namespace unicode {

class word_segmenter
{
  public:
    using char_type = char32_t;
    using iterator = char_type const*;
    using view_type = std::basic_string_view<char_type>;

    constexpr word_segmenter(std::basic_string_view<char_type> const& _str)
      : word_segmenter(_str.data(), _str.data() + _str.size())
    {}

    constexpr word_segmenter()
      : word_segmenter({}, {})
    {}

    constexpr bool empty() const noexcept { return size() == 0; }
    constexpr std::size_t size() const noexcept { return static_cast<size_t>(right_ - left_); }
    constexpr view_type operator*() const noexcept { return view_type(left_, size()); }

    constexpr word_segmenter& operator++() noexcept
    {
        left_ = right_;
        while (right_ != end_)
        {
            switch (state_)
            {
                case State::NoWord:
                    if (!isDelimiter(*right_))
                    {
                        state_ = State::Word;
                        return *this;
                    }
                    break;
                case State::Word:
                    if (isDelimiter(*right_))
                    {
                        state_ = State::NoWord;
                        return *this;
                    }
                    break;
            }
            ++right_;
        }
        return *this;
    }

    constexpr bool operator==(word_segmenter const& _rhs) const noexcept
    {
        return left_ == _rhs.left_ && right_ == _rhs.right_;
    }

    constexpr bool operator!=(word_segmenter const& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }

  private:
    constexpr word_segmenter(iterator _begin, iterator _end)
      : left_{ _begin },
        right_{ _begin },
        state_{ _begin != _end ? (isDelimiter(*right_) ? State::NoWord : State::Word) : State::NoWord },
        end_{ _end }
    {
        ++*this;
    }

    constexpr bool isDelimiter(char_type _char) const noexcept
    {
        switch (_char)
        {
            case ' ':
            case '\r':
            case '\n':
            case '\t':
                return true;
            default:
                return false;
        }
    }

    // private fields
    //
    enum class State { Word, NoWord };

    iterator left_;
    iterator right_;
    State state_;
    iterator end_;
};

} // end namespace
