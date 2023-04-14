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

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

namespace unicode
{

/// Converts an UTF-32 codepoint into a UTF-8 sequence.
///
/// @param character UTF-32 character to encode to UTF-8
/// @param result target memory location to start writing to (up to 4 chars)
///
/// @return number of bytes written to.
constexpr inline unsigned to_utf8(char32_t character, uint8_t* result)
{
    if (character <= 0x7F)
    {
        result[0] = static_cast<uint8_t>(character & 0b0111'1111);
        return 1;
    }
    else if (character <= 0x07FF)
    {
        result[0] = static_cast<uint8_t>(((character >> 6) & 0b0001'1111) | 0b1100'0000);
        result[1] = static_cast<uint8_t>(((character >> 0) & 0b0011'1111) | 0b1000'0000);
        return 2;
    }
    if (character <= 0xFFFF)
    {
        result[0] = static_cast<uint8_t>(((character >> 12) & 0b0000'1111) | 0b1110'0000);
        result[1] = static_cast<uint8_t>(((character >> 6) & 0b0011'1111) | 0b1000'0000);
        result[2] = static_cast<uint8_t>(((character >> 0) & 0b0011'1111) | 0b1000'0000);
        return 3;
    }
    else
    {
        result[0] = static_cast<uint8_t>(((character >> 18) & 0b0000'0111) | 0b1111'0000);
        result[1] = static_cast<uint8_t>(((character >> 12) & 0b0011'1111) | 0b1000'0000);
        result[2] = static_cast<uint8_t>(((character >> 6) & 0b0011'1111) | 0b1000'0000);
        result[3] = static_cast<uint8_t>(((character >> 0) & 0b0011'1111) | 0b1000'0000);
        return 4;
    }
}

/// Converts a UTF-32 string into an UTF-8 sring.
inline std::string to_utf8(char32_t const* characters, size_t n)
{
    std::string s;
    s.reserve(n);
    for (size_t i = 0; i < n; ++i)
    {
        uint8_t bytes[4];
        unsigned const len = to_utf8(characters[i], bytes);
        s.append((char const*) bytes, len);
    }

    return s;
}

inline std::string to_utf8(char32_t character)
{
    return to_utf8(&character, 1);
}

inline std::string to_utf8(std::u32string const& characters)
{
    return to_utf8(characters.data(), characters.size());
}

inline std::string to_utf8(std::u32string_view const& characters)
{
    return to_utf8(characters.data(), characters.size());
}

struct utf8_decoder_state
{
    char32_t character = 0;
    unsigned expectedLength = 0;
    unsigned currentLength = 0;
};

// clang-format off
// NOLINTBEGIN(readability-identifier-naming)
struct Invalid { };
struct Incomplete { };
struct Success { char32_t value; };
// NOLINTEND(readability-identifier-naming)
// clang-format on

using ConvertResult = std::variant<Invalid, Incomplete, Success>;

/// Progressively decodes a UTF-8 codepoint.
ConvertResult from_utf8(utf8_decoder_state& state, uint8_t value) noexcept;

inline unsigned from_utf8i(utf8_decoder_state& state, uint8_t value)
{
    auto const result = from_utf8(state, value);

    if (std::holds_alternative<Incomplete>(result))
        return static_cast<unsigned>(-1);

    if (std::holds_alternative<Invalid>(result))
        return static_cast<unsigned>(-2);

    return std::get<Success>(result).value;
}

inline ConvertResult from_utf8(uint8_t const* bytes, size_t* size)
{
    auto state = utf8_decoder_state {};
    auto result = ConvertResult {};

    do
        result = from_utf8(state, *bytes++);
    while (std::holds_alternative<Incomplete>(result));

    if (size)
        *size = state.currentLength;

    return result;
}

#if 0 // TODO(do that later) __cplusplus > 201703L // C++20 (char8_t)
inline ConvertResult from_utf8(char8_t const* bytes, size_t* size)
{
    return from_utf8((uint8_t const*)(bytes), size);
}
#endif

inline ConvertResult from_utf8(char const* bytes, size_t* size)
{
    return from_utf8((uint8_t const*) (bytes), size);
}

template <typename T = char32_t>
inline std::basic_string<T> from_utf8(std::string_view bytes)
{
    static_assert(sizeof(T) == 4);
    std::basic_string<T> s;
    size_t offset = 0;
    while (offset < bytes.size())
    {
        size_t i {};
        ConvertResult const result = from_utf8(bytes.data() + offset, &i);
        if (std::holds_alternative<Success>(result))
            s += T(std::get<Success>(result).value);
        offset += i;
    }
    return s;
}

} // namespace unicode
