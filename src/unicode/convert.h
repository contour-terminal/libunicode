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
#include <array>
#include <type_traits>
#include <iterator>
#include <optional>

namespace unicode {

template <typename T> struct decoder;
template <typename T> struct encoder;

template<> struct encoder<char> // {{{
{
    template <typename OutputIterator>
    constexpr size_t operator()(char32_t _input, OutputIterator& _output)
    {
        if (_input <= 0x7F)
        {
            *_output++ = static_cast<uint8_t>(_input & 0b0111'1111);
            return 1;
        }
        else if (_input <= 0x07FF)
        {
            *_output++ = static_cast<uint8_t>(((_input >> 6) & 0b0001'1111) | 0b1100'0000);
            *_output++ = static_cast<uint8_t>(((_input >> 0) & 0b0011'1111) | 0b1000'0000);
            return 2;
        }
        if (_input <= 0xFFFF)
        {
            *_output++ = static_cast<uint8_t>(((_input >> 12) & 0b0000'1111) | 0b1110'0000);
            *_output++ = static_cast<uint8_t>(((_input >>  6) & 0b0011'1111) | 0b1000'0000);
            *_output++ = static_cast<uint8_t>(((_input >>  0) & 0b0011'1111) | 0b1000'0000);
            return 3;
        }
        else
        {
            *_output++ = static_cast<uint8_t>(((_input >> 18) & 0b0000'0111) | 0b1111'0000);
            *_output++ = static_cast<uint8_t>(((_input >> 12) & 0b0011'1111) | 0b1000'0000);
            *_output++ = static_cast<uint8_t>(((_input >>  6) & 0b0011'1111) | 0b1000'0000);
            *_output++ = static_cast<uint8_t>(((_input >>  0) & 0b0011'1111) | 0b1000'0000);
            return 4;
        }
    }
}; // }}}
template<> struct decoder<char> // {{{
{
    char32_t character = 0;
    unsigned expectedLength = 0;
    unsigned currentLength = 0;

    constexpr std::optional<char32_t> operator()(uint8_t _byte)
    {
        if (!expectedLength)
        {
            if ((_byte & 0b1000'0000) == 0)
            {
                currentLength = 1;
                return char32_t(_byte);
            }
            else if ((_byte & 0b1110'0000) == 0b1100'0000)
            {
                currentLength = 1;
                expectedLength = 2;
                character = _byte & 0b0001'1111;
            }
            else if ((_byte & 0b1111'0000) == 0b1110'0000)
            {
                currentLength = 1;
                expectedLength = 3;
                character = _byte & 0b0000'1111;
            }
            else if ((_byte & 0b1111'1000) == 0b1111'0000)
            {
                currentLength = 1;
                expectedLength = 4;
                character = _byte & 0b0000'0111;
            }
            else
                return std::nullopt; // invalid
        }
        else
        {
            character <<= 6;
            character |= _byte & 0b0011'1111;
            currentLength++;
        }

        if  (currentLength < expectedLength)
            return std::nullopt; // incomplete

        expectedLength = 0; // reset state
        return character;
    }

    template <
        typename InputIterator,
        std::enable_if_t<sizeof(decltype(*std::declval<InputIterator>())) == 1, int> = 0
    >
    constexpr std::optional<char32_t> operator()(InputIterator& _input)
    {
        using std::nullopt;

        auto const ch0 = uint8_t(*_input++);
        if (ch0 < 0x80) // 0xxx_xxxx
            return char32_t(ch0);

        if (ch0 < 0xC0)
            throw nullopt;

        if (ch0 < 0xE0) // 110x_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*_input++);
            if ((ch1 >> 6) != 2)
                return nullopt;
            return char32_t((ch0 << 6) + ch1 - 0x3080);
        }

        if (ch0 < 0xF0) // 1110_xxxx 10xx_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*_input++);
            if (ch1 >> 6 != 2)
                return nullopt;
            auto const ch2 = uint8_t(*_input++);
            if (ch2 >> 6 != 2)
                return nullopt;
            return char32_t((ch0 << 12) + (ch1 << 6) + ch2 - 0xE2080);
        }
        if (ch0 < 0xF8) // 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*_input++);
            if (ch1 >> 6 != 2)
                return nullopt;
            auto const ch2 = uint8_t(*_input++);
            if (ch2 >> 6 != 2)
                return nullopt;
            auto const ch3 = uint8_t(*_input++);
            if (ch3 >> 6 != 2)
                return nullopt;
            return char32_t((ch0 << 18) + (ch1 << 12) + (ch2 << 6) + ch3 - 0x3C82080);
        }
        if (ch0 < 0xFC) // 1111_10xx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*_input++);
            if (ch1 >> 6 != 2)
                return nullopt;
            auto const ch2 = uint8_t(*_input++);
            if (ch2 >> 6 != 2)
                return nullopt;
            auto const ch3 = uint8_t(*_input++);
            if (ch3 >> 6 != 2)
                return nullopt;
            auto const ch4 = uint8_t(*_input++);
            if (ch4 >> 6 != 2)
                return nullopt;
            return char32_t((ch0 << 24) + (ch1 << 18) + (ch2 << 12) + (ch3 << 6) + ch4 - 0xFA082080);
        }
        if (ch0 < 0xFE) // 1111_110x 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            auto const ch1 = uint8_t(*_input++);
            if (ch1 >> 6 != 2)
                return nullopt;
            auto const ch2 = uint8_t(*_input++);
            if (ch2 >> 6 != 2)
                return nullopt;
            auto const ch3 = uint8_t(*_input++);
            if (ch3 >> 6 != 2)
                return nullopt;
            auto const ch4 = uint8_t(*_input++);
            if (ch4 >> 6 != 2)
                return nullopt;
            auto const ch5 = uint8_t(*_input++);
            if (ch5 >> 6 != 2)
                return nullopt;
            return char32_t((ch0 << 30) + (ch1 << 24) + (ch2 << 18) + (ch3 << 12) + (ch4 << 6) + ch5 - 0x82082080);
        }
        return nullopt;
    }
}; // }}}
template<> struct encoder<char16_t> // {{{
{
    using char_type = char16_t;

    template <typename OutputIterator>
    constexpr size_t operator()(char32_t _input, OutputIterator& _output)
    {
        if (_input < 0xD800) // [0x0000 .. 0xD7FF]
        {
            *_output++ = char_type(_input);
            return 1;
        }
        else if (_input < 0x10000)
        {
            if (_input < 0xE000)
                return 0; // The UTF-16 code point can not be in surrogate range.

            // [0xE000 .. 0xFFFF]
            *_output++ = char_type(_input);
            return 1;
        }
        else if (_input < 0x110000) // [0xD800 .. 0xDBFF] [0xDC00 .. 0xDFFF]
        {
            *_output++ = char_type(0xD7C0 + (_input >> 10));
            *_output++ = char_type(0xDC00 + (_input & 0x3FF));
            return 2;
        }
        else
            return 0; // Too large the UTF-16  code point.
    }
}; // }}}
template<> struct decoder<char16_t> // {{{
{
    template <typename InputIterator>
    constexpr std::optional<char32_t> operator()(InputIterator& _input)
    {
        auto const ch0 = *_input++;

        if (ch0 < 0xD800) // [0x0000 .. 0xD7FF]
            return ch0;

        if (ch0 < 0xDC00) // [0xD800 .. 0xDBFF], [0xDC00 .. 0xDFFF]
        {
            auto const ch1 = *_input++;
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
template<> struct encoder<char32_t> // {{{ (no-op)
{
    using char_type = char32_t;

    template <typename OutputIterator>
    constexpr size_t operator()(char32_t _input, OutputIterator& _output)
    {
        *_output++ = _input;
        return 1;
    }
}; // }}}
template<> struct decoder<char32_t> // {{{ (no-op)
{

    template <typename InputIterator>
    constexpr std::optional<char32_t> operator()(InputIterator& _input)
    {
        return *_input++;
    }
}; // }}}
template<> struct encoder<wchar_t> // {{{
{
    using char_type = wchar_t;

    template <typename OutputIterator>
    constexpr size_t operator()(char32_t _input, OutputIterator&& _output)
    {
        static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4);

        if constexpr (sizeof(wchar_t) == 2)
            return encoder<char16_t>{}(_input, _output);
        else
            return encoder<char32_t>{}(_input, _output);
    }
}; // }}}
template<> struct decoder<wchar_t> // {{{
{
    template <typename InputIterator>
    constexpr std::optional<char32_t> operator()(InputIterator& _input)
    {
        static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4);

        if constexpr (sizeof(wchar_t) == 2)
            return decoder<char16_t>{}(_input);
        else
            return decoder<char32_t>{}(_input);
    }
}; // }}}

namespace detail // {{{
{
    template <typename SourceRange, typename OutputIterator>
    void convert_identity(SourceRange&& s, OutputIterator&& t)
    {
        for (auto const c : s)
            *t++ = c;
    }
} // }}}

/// @p _input with element type @p S to the appropricate type of @p _output.
template <typename T, typename OutputIterator, typename S>
void convert_to(std::basic_string_view<S> _input, OutputIterator&& _output)
{
    if constexpr (std::is_same_v<S, T>)
        detail::convert_identity(_input, _output);
    else
    {
        auto i = begin(_input);
        auto e = end(_input);
        decoder<S> read{};
        encoder<T> write{};
        while (i != e)
        {
            auto const outChar = read(i);
            if (outChar.has_value())
                write(outChar.value(), _output);
        }
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

}
