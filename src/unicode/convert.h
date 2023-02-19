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

#include <array>
#include <iterator>
#include <optional>
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace unicode
{

// clang-format off
template <typename T> struct decoder;
template <typename T> struct encoder;
// clang-format on

template <>
struct encoder<char> // {{{
{
    template <typename OutputIterator>
    constexpr OutputIterator operator()(char32_t input, OutputIterator output)
    {
        if (input <= 0x7F)
        {
            *output++ = static_cast<char>(input & 0b0111'1111);
        }
        else if (input <= 0x07FF)
        {
            *output++ = static_cast<char>(((input >> 6) & 0b0001'1111) | 0b1100'0000);
            *output++ = static_cast<char>(((input >> 0) & 0b0011'1111) | 0b1000'0000);
        }
        else if (input <= 0xFFFF)
        {
            *output++ = static_cast<char>(((input >> 12) & 0b0000'1111) | 0b1110'0000);
            *output++ = static_cast<char>(((input >> 6) & 0b0011'1111) | 0b1000'0000);
            *output++ = static_cast<char>(((input >> 0) & 0b0011'1111) | 0b1000'0000);
        }
        else
        {
            *output++ = static_cast<char>(((input >> 18) & 0b0000'0111) | 0b1111'0000);
            *output++ = static_cast<char>(((input >> 12) & 0b0011'1111) | 0b1000'0000);
            *output++ = static_cast<char>(((input >> 6) & 0b0011'1111) | 0b1000'0000);
            *output++ = static_cast<char>(((input >> 0) & 0b0011'1111) | 0b1000'0000);
        }
        return output;
    }
}; // }}}
template <>
struct decoder<char> // {{{
{
    char32_t character = 0;
    unsigned expectedLength = 0;
    unsigned currentLength = 0;

    constexpr std::optional<char32_t> operator()(uint8_t byte)
    {
        if (!expectedLength)
        {
            if ((byte & 0b1000'0000) == 0)
            {
                currentLength = 1;
                return char32_t(byte);
            }
            else if ((byte & 0b1110'0000) == 0b1100'0000)
            {
                currentLength = 1;
                expectedLength = 2;
                character = byte & 0b0001'1111;
            }
            else if ((byte & 0b1111'0000) == 0b1110'0000)
            {
                currentLength = 1;
                expectedLength = 3;
                character = byte & 0b0000'1111;
            }
            else if ((byte & 0b1111'1000) == 0b1111'0000)
            {
                currentLength = 1;
                expectedLength = 4;
                character = byte & 0b0000'0111;
            }
            else
                return std::nullopt; // invalid
        }
        else
        {
            character <<= 6;
            character |= byte & 0b0011'1111;
            currentLength++;
        }

        if (currentLength < expectedLength)
            return std::nullopt; // incomplete

        expectedLength = 0; // reset state
        return character;
    }

    template <
        typename InputIterator,
        std::enable_if_t<std::is_convertible_v<decltype(*std::declval<InputIterator>()), char>, int> = 0>
    constexpr std::optional<char32_t> operator()(InputIterator& input)
    {
        using std::nullopt;

        auto const ch0 = uint8_t(*input++);
        if (ch0 < 0x80) // 0xxx_xxxx
            return static_cast<char32_t>(ch0);

        if (ch0 < 0xC0)
            return nullopt;

        if (ch0 < 0xE0) // 110x_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*input++);
            if ((ch1 >> 6) != 2)
                return nullopt;
            return static_cast<char32_t>((ch0 << 6) + ch1 - 0x3080);
        }

        if (ch0 < 0xF0) // 1110_xxxx 10xx_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*input++);
            if (ch1 >> 6 != 2)
                return nullopt;
            auto const ch2 = uint8_t(*input++);
            if (ch2 >> 6 != 2)
                return nullopt;
            return static_cast<char32_t>((ch0 << 12) + (ch1 << 6) + ch2 - 0xE2080);
        }
        if (ch0 < 0xF8) // 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*input++);
            if (ch1 >> 6 != 2)
                return nullopt;
            auto const ch2 = uint8_t(*input++);
            if (ch2 >> 6 != 2)
                return nullopt;
            auto const ch3 = uint8_t(*input++);
            if (ch3 >> 6 != 2)
                return nullopt;
            return static_cast<char32_t>((ch0 << 18) + (ch1 << 12) + (ch2 << 6) + ch3 - 0x3C82080);
        }
        if (ch0 < 0xFC) // 1111_10xx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*input++);
            if (ch1 >> 6 != 2)
                return nullopt;
            auto const ch2 = uint8_t(*input++);
            if (ch2 >> 6 != 2)
                return nullopt;
            auto const ch3 = uint8_t(*input++);
            if (ch3 >> 6 != 2)
                return nullopt;
            auto const ch4 = uint8_t(*input++);
            if (ch4 >> 6 != 2)
                return nullopt;
            auto const a =
                static_cast<uint32_t>((ch0 << 24u) + (ch1 << 18u) + (ch2 << 12u) + (ch3 << 6u) + ch4);
            return static_cast<char32_t>(a - 0xFA082080lu);
        }
        if (ch0 < 0xFE) // 1111_110x 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*input++);
            if (ch1 >> 6 != 2)
                return nullopt;
            auto const ch2 = uint8_t(*input++);
            if (ch2 >> 6 != 2)
                return nullopt;
            auto const ch3 = uint8_t(*input++);
            if (ch3 >> 6 != 2)
                return nullopt;
            auto const ch4 = uint8_t(*input++);
            if (ch4 >> 6 != 2)
                return nullopt;
            auto const ch5 = uint8_t(*input++);
            if (ch5 >> 6 != 2)
                return nullopt;
            auto const a = static_cast<uint32_t>((ch0 << 30) + (ch1 << 24) + (ch2 << 18) + (ch3 << 12)
                                                 + (ch4 << 6) + ch5);
            return static_cast<char32_t>(a - 0x82082080);
        }
        return nullopt;
    }
}; // }}}
template <>
struct encoder<char16_t> // {{{
{
    using char_type = char16_t;

    template <typename OutputIterator>
    constexpr OutputIterator operator()(char32_t input, OutputIterator output)
    {
        if (input < 0xD800) // [0x0000 .. 0xD7FF]
        {
            *output++ = char_type(input);
            return output;
        }
        else if (input < 0x10000)
        {
            if (input < 0xE000)
                return output; // The UTF-16 code point can not be in surrogate range.

            // [0xE000 .. 0xFFFF]
            *output++ = char_type(input);
            return output;
        }
        else if (input < 0x110000) // [0xD800 .. 0xDBFF] [0xDC00 .. 0xDFFF]
        {
            *output++ = char_type(0xD7C0 + (input >> 10));
            *output++ = char_type(0xDC00 + (input & 0x3FF));
            return output;
        }
        else
            return output; // Too large the UTF-16  code point.
    }
}; // }}}
template <>
struct decoder<char16_t> // {{{
{
    template <typename InputIterator>
    constexpr std::optional<char32_t> operator()(InputIterator& input)
    {
        auto const ch0 = *input++;

        if (ch0 < 0xD800) // [0x0000 .. 0xD7FF]
            return ch0;

        if (ch0 < 0xDC00) // [0xD800 .. 0xDBFF], [0xDC00 .. 0xDFFF]
        {
            auto const ch1 = *input++;
            if ((ch1 >> 10) != 0x37)
                return std::nullopt; // The low UTF-16 surrogate character is expected.

            return (ch0 << 10) + ch1 - 0x35FDC00;
        }

        if (ch0 < 0xE000)
            return std::nullopt; // The high UTF-16 surrogate character is expected.

        // [0xE000 .. 0xFFFF]
        return ch0;
    }
}; // }}}
template <>
struct encoder<char32_t> // {{{ (no-op)
{
    using char_type = char32_t;

    template <typename OutputIterator>
    constexpr OutputIterator operator()(char32_t input, OutputIterator output)
    {
        *output++ = input;
        return output;
    }
}; // }}}
template <>
struct decoder<char32_t> // {{{ (no-op)
{

    template <typename InputIterator>
    constexpr std::optional<char32_t> operator()(InputIterator& input)
    {
        return *input++;
    }
}; // }}}
template <>
struct encoder<wchar_t> // {{{
{
    using char_type = wchar_t;

    template <typename OutputIterator>
    constexpr OutputIterator operator()(char32_t input, OutputIterator output)
    {
        static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4);

        if constexpr (sizeof(wchar_t) == 2)
            return encoder<char16_t> {}(input, output);
        else
            return encoder<char32_t> {}(input, output);
    }
}; // }}}
template <>
struct decoder<wchar_t> // {{{
{
    template <typename InputIterator>
    constexpr std::optional<char32_t> operator()(InputIterator& input)
    {
        static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4);

        if constexpr (sizeof(wchar_t) == 2)
            return decoder<char16_t> {}(input);
        else
            return decoder<char32_t> {}(input);
    }
}; // }}}

namespace detail // {{{
{
    template <typename SourceRange, typename OutputIterator>
    OutputIterator convert_identity(SourceRange&& s, OutputIterator t)
    {
        for (auto const c: s)
            *t++ = c;
        return t;
    }
} // namespace detail

/// @p _input with element type @p S to the appropricate type of @p _output.
template <typename T, typename OutputIterator, typename S>
OutputIterator convert_to(std::basic_string_view<S> input, OutputIterator output)
{
    if constexpr (std::is_same_v<S, T>)
        return detail::convert_identity(input, output);
    else
    {
        auto i = begin(input);
        auto e = end(input);
        decoder<S> read {};
        encoder<T> write {};
        while (i != e)
        {
            auto const outChar = read(i);
            if (outChar.has_value())
                output = write(outChar.value(), output);
        }
        return output;
    }
}

/// Converts a string of element type @p <S> into string of element type @p <T>.
template <typename T, typename S>
std::basic_string<T> convert_to(std::basic_string_view<S> in)
{
    std::basic_string<T> out;
    convert_to<T>(in, std::back_inserter(out));
    return out;
}

template <
    typename T,
    typename S,
    std::enable_if_t<std::is_same_v<S, char> || std::is_same_v<S, char16_t> || std::is_same_v<S, char32_t>,
                     int> = 0>
std::basic_string<T> convert_to(S input)
{
    std::basic_string_view<S> in(&input, 1);
    std::basic_string<T> out;
    convert_to<T>(in, std::back_inserter(out));
    return out;
}

} // namespace unicode
