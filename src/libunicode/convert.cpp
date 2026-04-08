// SPDX-License-Identifier: Apache-2.0
#include <libunicode/convert.h>
#include <libunicode/convert_simd_impl.h>

#if (defined(LIBUNICODE_USE_STD_SIMD) || defined(LIBUNICODE_USE_INTRINSICS)) && (defined(__x86_64__) || defined(_M_AMD64))
    #include <libunicode/simd_detector.h>
#endif

namespace unicode::detail
{

size_t convert_utf8_to_utf32(char const* input, size_t inputSize, char32_t* output) noexcept
{
#if (defined(LIBUNICODE_USE_STD_SIMD) || defined(LIBUNICODE_USE_INTRINSICS)) && (defined(__x86_64__) || defined(_M_AMD64))
    static auto const simdSize = max_simd_size();
    if (simdSize == 512)
        return convert_utf8_to_utf32_512(input, inputSize, output);
    if (simdSize == 256)
        return convert_utf8_to_utf32_256(input, inputSize, output);
#endif
    return convert_utf8_to_utf32_simd<128>(input, inputSize, output);
}

size_t convert_utf8_to_utf16(char const* input, size_t inputSize, char16_t* output) noexcept
{
#if (defined(LIBUNICODE_USE_STD_SIMD) || defined(LIBUNICODE_USE_INTRINSICS)) && (defined(__x86_64__) || defined(_M_AMD64))
    static auto const simdSize = max_simd_size();
    if (simdSize == 512)
        return convert_utf8_to_utf16_512(input, inputSize, output);
    if (simdSize == 256)
        return convert_utf8_to_utf16_256(input, inputSize, output);
#endif
    return convert_utf8_to_utf16_simd<128>(input, inputSize, output);
}

} // namespace unicode::detail
