#include <algorithm>
#include <cassert>
#include <iterator>
#include <string_view>

#include "scan.h"
#include <experimental/simd>

namespace stdx = std::experimental;
using std::distance;
using std::get;
using std::holds_alternative;
using std::max;
using std::min;
using std::string_view;

constexpr bool is_control(char ch) noexcept
{
    return static_cast<uint8_t>(ch) < 0x20;
}

// Tests if given UTF-8 byte is part of a complex Unicode codepoint, that is, a value greater than U+7E.
constexpr bool is_complex(char ch) noexcept
{
    return static_cast<uint8_t>(ch) & 0x80;
}

// Tests if given UTF-8 byte is a single US-ASCII text codepoint. This excludes control characters.
constexpr bool is_ascii(char ch) noexcept
{
    return !is_control(ch) && !is_complex(ch);
}

namespace unicode
{
size_t detail::scan_for_text_ascii_256(string_view text, size_t maxColumnCount) noexcept
{
    auto input = text.data();
    auto const end = text.data() + min(text.size(), maxColumnCount);
    constexpr int numberOfElements = 256 / 8;
    stdx::fixed_size_simd<char, numberOfElements> simd_text {};
    while (input < end - numberOfElements)
    {
        simd_text.copy_from(input, stdx::element_aligned);

        // check for control
        // TODO check for complex
        auto const simd_mask_text = (simd_text < 0x20);
        if (stdx::popcount(simd_mask_text) > 0)
        {
            input += stdx::find_first_set(simd_mask_text);
            break;
        }
        input += numberOfElements;
    }
    while (input != end && is_ascii(*input))
        ++input;

    // if (static_cast<size_t>(distance(text.data(), input)))
    //     std::print(
    //         "countAsciiTextChars: {} bytes: \"{}\"\n",
    //         static_cast<size_t>(distance(text.data(), input)),
    //         (string_view(text.data(), static_cast<size_t>(distance(text.data(), input)))));

    return static_cast<size_t>(distance(text.data(), input));
}
} // namespace unicode
