/**
 * This file is part of the "libunicode" project
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

#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>
#include <variant>

namespace unicode {

/// Converts an UTF-32 codepoint into a UTF-8 sequence.
///
/// @param _character UTF-32 character to encode to UTF-8
/// @param _result target memory location to start writing to (up to 4 chars)
///
/// @return number of bytes written to.
constexpr inline unsigned to_utf8(char32_t _character, uint8_t* _result)
{
    if (_character <= 0x7F)
    {
        _result[0] = static_cast<uint8_t>(_character & 0b0111'1111);
        return 1;
    }
    else if (_character <= 0x07FF)
    {
        _result[0] = static_cast<uint8_t>(((_character >> 6) & 0b0001'1111) | 0b1100'0000);
        _result[1] = static_cast<uint8_t>(((_character >> 0) & 0b0011'1111) | 0b1000'0000);
        return 2;
    }
    if (_character <= 0xFFFF)
    {
        _result[0] = static_cast<uint8_t>(((_character >> 12) & 0b0000'1111) | 0b1110'0000);
        _result[1] = static_cast<uint8_t>(((_character >>  6) & 0b0011'1111) | 0b1000'0000);
        _result[2] = static_cast<uint8_t>(((_character >>  0) & 0b0011'1111) | 0b1000'0000);
        return 3;
    }
    else
    {
        _result[0] = static_cast<uint8_t>(((_character >> 18) & 0b0000'0111) | 0b1111'0000);
        _result[1] = static_cast<uint8_t>(((_character >> 12) & 0b0011'1111) | 0b1000'0000);
        _result[2] = static_cast<uint8_t>(((_character >>  6) & 0b0011'1111) | 0b1000'0000);
        _result[3] = static_cast<uint8_t>(((_character >>  0) & 0b0011'1111) | 0b1000'0000);
        return 4;
    }
}

/// Converts a UTF-32 string into an UTF-8 sring.
inline std::string to_utf8(char32_t const* _characters, size_t n)
{
    std::string s;
    s.reserve(n);
    for (size_t i = 0; i < n; ++i)
    {
        uint8_t bytes[4];
        unsigned const len = to_utf8(_characters[i], bytes);
        s.append((char const*) bytes, len);
    }

    return s;
}

inline std::string to_utf8(char32_t _character)
{
    return to_utf8(&_character, 1);
}

inline std::string to_utf8(std::u32string const& _characters)
{
    return to_utf8(_characters.data(), _characters.size());
}

inline std::string to_utf8(std::u32string_view const& _characters)
{
    return to_utf8(_characters.data(), _characters.size());
}

struct utf8_decoder_state {
    char32_t character = 0;
    unsigned expectedLength = 0;
    unsigned currentLength = 0;
};

struct Invalid{};
struct Incomplete{};
struct Success{ char32_t value; };

using ConvertResult = std::variant<Invalid, Incomplete, Success>;

/// Progressively decodes a UTF-8 codepoint.
constexpr ConvertResult from_utf8(utf8_decoder_state& _state, char _byte)
{
    if (_state.expectedLength == 0)
    {
        _state.currentLength = 1;

        if ((_byte >> 7) == 0)
        {
            _state.expectedLength = 1;
            _state.character = _byte;
            return Success{static_cast<char32_t>(_byte)};
        }
        else if (((_byte >> 5) & 0b111) == 0b110)
        {
            _state.expectedLength = 2;
            _state.character = _byte & 0b0001'1111;
            return Incomplete{};
        }
        else if (((_byte >> 4) & 0b1111) == 0b1110)
        {
            _state.expectedLength = 3;
            _state.character = _byte & 0b0000'1111;
            return Incomplete{};
        }
        else if (((_byte >> 3) & 0b11111) == 0b1111'0)
        {
            _state.expectedLength = 4;
            _state.character = _byte & 0b0000'0111;
            return Incomplete{};
        }
        else
            return Invalid{};
    }

    _state.character <<= 6;
    _state.character |= _byte & 0b0011'1111;
    _state.currentLength++;

    if (_state.currentLength < _state.expectedLength)
        return Incomplete{};

    auto const result = _state.character;
    _state.expectedLength = 0;
    return Success{result};
}

constexpr int from_utf8i(utf8_decoder_state& _state, char _byte)
{
    auto const result = from_utf8(_state, _byte);

    if (std::holds_alternative<Incomplete>(result))
        return -1;

    if (std::holds_alternative<Invalid>(result))
        return -2;

    return std::get<Success>(result).value;
}

constexpr ConvertResult from_utf8(utf8_decoder_state& _state, uint8_t const* _bytes)
{
    if (auto const result = from_utf8(_state, *_bytes); !std::holds_alternative<Incomplete>(result))
        return result;

    return from_utf8(_state, _bytes + 1);
}

constexpr ConvertResult from_utf8(uint8_t const* _bytes, size_t* _size)
{
    auto state = utf8_decoder_state{};
    auto const result = from_utf8(state, _bytes);
    if (_size)
        *_size = state.currentLength;
    return result;
}

inline ConvertResult from_utf8(char const* _bytes, size_t* _size)
{
    return from_utf8((uint8_t const*)(_bytes), _size);
}

inline std::u32string from_utf8(std::string const& _bytes)
{
    std::u32string s;
    size_t offset = 0;
    while (offset < _bytes.size())
    {
        size_t i{};
        ConvertResult const result = from_utf8(_bytes.data() + offset, &i);
        if (std::holds_alternative<Success>(result))
            s += (std::get<Success>(result).value);
        offset += i;
    }
    return s;
}

} // end namespace
