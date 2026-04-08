// SPDX-License-Identifier: Apache-2.0
#include <libunicode/convert_simd_impl.h>

namespace unicode::detail
{

size_t convert_utf8_to_utf32_256(char const* input, size_t inputSize, char32_t* output) noexcept
{
    return convert_utf8_to_utf32_simd<256>(input, inputSize, output);
}

size_t convert_utf8_to_utf16_256(char const* input, size_t inputSize, char16_t* output) noexcept
{
    return convert_utf8_to_utf16_simd<256>(input, inputSize, output);
}

} // namespace unicode::detail
