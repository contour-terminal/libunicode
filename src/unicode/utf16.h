#pragma once

#include <stdexcept>
#include <string_view>
#include <utility>
#include <iterator>

namespace unicode {

// XXX This is experimental API.
template <typename OutIter>
void convert_to_utf16(char32_t _char, OutIter _output)
{
    if (_char < 0xD800) // [0x0000 .. 0xD7FF]
    {
        *_output = _char;
        ++_output;
    }
    else if (_char < 0x10000)
    {
        if (_char < 0xE000)
            throw std::invalid_argument("The utf16 code point can not be in surrogate range");
        // [0xE000 .. 0xFFFF]
        *_output = _char;
        ++_output;
    }
    else if (_char < 0x110000) // [0xD800‥0xDBFF] [0xDC00‥0xDFFF]
    {
        *_output = 0xD7C0 + (_char >> 10);
        ++_output;

        *_output = 0xDC00 + (_char & 0x3FF);
        ++_output;
    }
    else
        throw std::invalid_argument("Too large the utf16 code point");
}

// XXX This is experimental API.
template <typename OutIter>
constexpr void convert_to_utf16(std::u32string_view const& _text, OutIter _output)
{
    for (char32_t const ch : _text)
        convert_to_utf16(ch, _output);
}

// XXX This is experimental API.
template <typename InIter>
std::pair<char32_t, InIter> from_utf16(InIter input)
{
    auto const ch0 = char16_t{*input++};

    if (ch0 < 0xD800) // [0x0000 .. 0xD7FF]
        return {ch0, input};

    if (ch0 < 0xDC00) // [0xD800 .. 0xDBFF], [0xDC00 .. 0xDFFF]
    {
        auto const ch1 = char16_t{*input++};
        if ((ch1 >> 10) != 0x37)
            throw std::invalid_argument("The low utf16 surrogate char is expected");

        return {(ch0 << 10) + ch1 - 0x35FDC00, input};
    }

    if (ch0 < 0xE000)
        throw std::invalid_argument("The high utf16 surrogate char is expected");

    // [0xE000‥0xFFFF]
    return {ch0, input};
}

// XXX This is experimental API.
inline size_t length(std::u16string_view _text)
{
    size_t out = 0;
    auto i = _text.begin();
    auto e = _text.end();

    while (i != e)
    {
        auto const increment = [](char16_t ch) -> int {
            if (ch < 0xD800) // [0x0000 .. 0xD7FF]
                return 1;
            if (ch < 0xDC00) // [0xD800 .. 0xDBFF], [0xDC00 .. 0xDFFF]
                return 2;
            if (ch < 0xE000)
                throw std::invalid_argument("The high utf16 surrogate char is expected");
            // [0xE000‥0xFFFF]
            return 1;
        }(*i);
        i += increment;
    }

    return out;
}

// XXX This is experimental API.
inline std::u32string from_utf16(std::u16string_view const& _text)
{
    std::u32string output;
    auto i = _text.begin();
    auto e = _text.end();
    while (i != e)
    {
        auto const [ch, next] = from_utf16(i);
        output.push_back(ch);
        i = next;
    }
    return output;
}

} // namespace unicode
