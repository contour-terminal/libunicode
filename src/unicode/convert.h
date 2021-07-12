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

#include <unicode/detail/convert-common.hpp>
#include <unicode/detail/convert-naive.hpp>
#include <unicode/detail/convert-sse.hpp>
#include <unicode/detail/convert-avx512bw.hpp>
//#include <unicode/detail/convert-avx512.hpp>

#include <string_view>
#include <array>
#include <type_traits>
#include <iterator>
#include <optional>

namespace unicode::accelerator
{
#if defined(__AVX512BW__)
    using optimal = avx512bw;
#elif defined(__SSE__)
    using optimal = sse;
#else
    using optimal = naive;
#endif
}

namespace unicode {

template <typename T> struct decoder;
template <typename T> struct encoder;

template<> struct encoder<char> // {{{
{
    template <typename OutputIterator>
    constexpr OutputIterator operator()(char32_t _input, OutputIterator _output)
    {
        if (_input <= 0x7F)
        {
            *_output++ = static_cast<char>(_input & 0b0111'1111);
        }
        else if (_input <= 0x07FF)
        {
            *_output++ = static_cast<char>(((_input >> 6) & 0b0001'1111) | 0b1100'0000);
            *_output++ = static_cast<char>(((_input >> 0) & 0b0011'1111) | 0b1000'0000);
        }
        else if (_input <= 0xFFFF)
        {
            *_output++ = static_cast<char>(((_input >> 12) & 0b0000'1111) | 0b1110'0000);
            *_output++ = static_cast<char>(((_input >>  6) & 0b0011'1111) | 0b1000'0000);
            *_output++ = static_cast<char>(((_input >>  0) & 0b0011'1111) | 0b1000'0000);
        }
        else
        {
            *_output++ = static_cast<char>(((_input >> 18) & 0b0000'0111) | 0b1111'0000);
            *_output++ = static_cast<char>(((_input >> 12) & 0b0011'1111) | 0b1000'0000);
            *_output++ = static_cast<char>(((_input >>  6) & 0b0011'1111) | 0b1000'0000);
            *_output++ = static_cast<char>(((_input >>  0) & 0b0011'1111) | 0b1000'0000);
        }
        return _output;
    }
}; // }}}
template<> struct decoder<char> // {{{
{
    char32_t character = 0;
    unsigned expectedLength = 0;
    unsigned currentLength = 0;

    LIBUNICODE_ALIGNED_FUNC
    LIBUNICODE_FORCE_INLINE
    decoder_status operator()(uint8_t const* _begin,
                              uint8_t const* _end,
                              char32_t* _output)
    {
        return consume<accelerator::optimal>(_begin, _end, _output);
    }

    template <typename Accelerator>
    LIBUNICODE_ALIGNED_FUNC
    LIBUNICODE_FORCE_INLINE
    decoder_status consume(uint8_t const* _begin,
                           uint8_t const* _end,
                           char32_t* _output)
    {
        uint8_t const* inputBegin = _begin;
        char32_t const* outputBegin = _output;

        if (!consumeUntilAligned<Accelerator>(_begin, _end, _output))
            return decoder_status{
                false,
                static_cast<size_t>(_begin - inputBegin),
                static_cast<size_t>(_output - outputBegin),
            };

        // Accelerated processing (128-bit aligned blocks)
        while (_begin <= _end - Accelerator::alignment)
        {
            if (*_begin < 0x80)
                detail::convertAsciiBlockOnce<Accelerator>(_begin, _output);
            else if (auto const opt = consumeCodepoint(_begin, _end))
                *_output++ = *opt;
            else
                return decoder_status{
                    false,
                    static_cast<size_t>(_begin - inputBegin),
                    static_cast<size_t>(_output - outputBegin),
                };
        }

        return decoder_status{
            consumeTrailingBytes(_begin, _end, _output),
            static_cast<size_t>(_begin - inputBegin),
            static_cast<size_t>(_output - outputBegin),
        };
    }

    constexpr std::optional<char32_t> operator()(uint8_t _byte)
    {
        return consumeCodeunit(_byte);
    }

    constexpr std::optional<char32_t> consumeCodeunit(uint8_t _byte)
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
            {
                expectedLength = 0; // reset state
                return std::nullopt; // invalid
            }
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
        typename InputSentinel,
        std::enable_if_t<std::is_convertible_v<decltype(*std::declval<InputIterator>()), char>, int> = 0
    >
    constexpr std::optional<char32_t> operator()(InputIterator& _input, InputSentinel _end)
    {
        return consumeCodepoint(_input, _end);
    }

    template <
        typename InputIterator,
        typename InputSentinel,
        std::enable_if_t<std::is_convertible_v<decltype(*std::declval<InputIterator>()), char>, int> = 0
    >
    LIBUNICODE_FORCE_INLINE
    constexpr std::optional<char32_t> consumeCodepoint(InputIterator& _input, InputSentinel _end)
    {
        while (_input < _end)
            if (auto codepoint = consumeCodeunit(*_input++))
                return codepoint;

        return std::nullopt;
    }

private:
    template <typename Accelerator>
    LIBUNICODE_FORCE_INLINE
    bool consumeUntilAligned(uint8_t const*& _begin,
                             uint8_t const* _end,
                             char32_t*& _output)
    {
        // Consume until 128-bit aligned.
        while (_begin < _end && !detail::is_aligned(_begin, Accelerator::alignment))
        {
            if (auto const opt = consumeCodepoint(_begin, _end))
                *_output++ = *opt;
            else
                return false;
        }
        return true;
    }

    LIBUNICODE_FORCE_INLINE
    bool consumeTrailingBytes(uint8_t const*& _begin,
                              uint8_t const* _end,
                              char32_t*& _output)
    {
        while (_begin < _end)
        {
            if (*_begin < 0x80)
                *_output++ = *_begin++;
            else if (auto const opt = consumeCodepoint(_begin, _end))
                *_output++ = *opt;
            else
                return false;
        }
        return true;
    }
}; // }}}
template<> struct encoder<char16_t> // {{{
{
    using char_type = char16_t;

    template <typename OutputIterator>
    constexpr OutputIterator operator()(char32_t _input, OutputIterator _output)
    {
        if (_input < 0xD800) // [0x0000 .. 0xD7FF]
        {
            *_output++ = char_type(_input);
            return _output;
        }
        else if (_input < 0x10000)
        {
            if (_input < 0xE000)
                return _output; // The UTF-16 code point can not be in surrogate range.

            // [0xE000 .. 0xFFFF]
            *_output++ = char_type(_input);
            return _output;
        }
        else if (_input < 0x110000) // [0xD800 .. 0xDBFF] [0xDC00 .. 0xDFFF]
        {
            *_output++ = char_type(0xD7C0 + (_input >> 10));
            *_output++ = char_type(0xDC00 + (_input & 0x3FF));
            return _output;
        }
        else
            return _output; // Too large the UTF-16  code point.
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
    constexpr OutputIterator operator()(char32_t _input, OutputIterator _output)
    {
        *_output++ = _input;
        return _output;
    }
}; // }}}
template<> struct decoder<char32_t> // {{{ (no-op)
{

    template <typename InputIterator, typename InputSentinel>
    constexpr std::optional<char32_t> operator()(InputIterator& _input, InputSentinel _end)
    {
        if (_input != _end)
            return *_input++;
        else
            return std::nullopt;
    }
}; // }}}
template<> struct encoder<wchar_t> // {{{
{
    using char_type = wchar_t;

    template <typename OutputIterator>
    constexpr OutputIterator operator()(char32_t _input, OutputIterator _output)
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
    OutputIterator  convert_identity(SourceRange&& s, OutputIterator t)
    {
        for (auto const c : s)
            *t++ = c;
        return t;
    }
} // }}}

/// @p _input with element type @p S to the appropricate type of @p _output.
template <typename T, typename OutputIterator, typename S>
OutputIterator convert_to(std::basic_string_view<S> _input, OutputIterator _output)
{
    if constexpr (std::is_same_v<S, T>)
        return detail::convert_identity(_input, _output);
    else
    {
        auto i = begin(_input);
        auto e = end(_input);
        decoder<S> read{};
        encoder<T> write{};
        while (i != e)
        {
            auto const outChar = read(i, e);
            if (outChar.has_value())
                _output = write(outChar.value(), _output);
        }
        return _output;
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

template <typename T, typename S,
    std::enable_if_t<
        std::is_same_v<S, char> ||
        std::is_same_v<S, char16_t> ||
        std::is_same_v<S, char32_t>
        , int
    > = 0
>
std::basic_string<T> convert_to(S _in)
{
    std::basic_string_view<S> in(&_in, 1);
    std::basic_string<T> out;
    convert_to<T>(in, std::back_inserter(out));
    return out;
}

}
