// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <libunicode/convert.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>

// clang-format off
#if __has_include(<experimental/simd>) && defined(LIBUNICODE_USE_STD_SIMD) && !defined(__APPLE__) && !defined(__FreeBSD__)
    #define USE_STD_SIMD_CONVERT
    #include <experimental/simd>
    namespace convert_stdx = std::experimental;
#elif __has_include(<simd>) && defined(LIBUNICODE_USE_STD_SIMD)
    #define USE_STD_SIMD_CONVERT
    #include <simd>
    namespace convert_stdx = std;
#elif defined(LIBUNICODE_USE_INTRINSICS)
    #include "intrinsics.h"
#endif
// clang-format on

namespace unicode::detail
{

// =====================================================================================
// UTF-8 -> UTF-32 SIMD-accelerated conversion
// =====================================================================================

/// Converts UTF-8 input to UTF-32 output using SIMD acceleration for ASCII runs.
///
/// @param input     Pointer to UTF-8 input bytes.
/// @param inputSize Number of input bytes.
/// @param output    Pointer to pre-allocated output buffer (must hold at least inputSize elements).
/// @return Number of char32_t values written to output.
template <size_t SimdBitWidth>
size_t convert_utf8_to_utf32_simd(char const* input, size_t inputSize, char32_t* output) noexcept
{
    [[maybe_unused]] constexpr int simd_size = SimdBitWidth / 8;
    auto const* src = reinterpret_cast<uint8_t const*>(input);
    auto const* src_end = src + inputSize;
    auto* dst = output;

    // --- SIMD ASCII fast path ---
#if defined(USE_STD_SIMD_CONVERT)
    while (src + simd_size <= src_end)
    {
        auto batch = convert_stdx::fixed_size_simd<signed char, simd_size> {};
        batch.copy_from(reinterpret_cast<signed char const*>(src), convert_stdx::element_aligned);
        // Check if any byte has high bit set (non-ASCII)
        auto const has_high_bit = batch < static_cast<signed char>(0);
        if (convert_stdx::any_of(has_high_bit))
            break;
        // All ASCII: widen each byte to char32_t
        for (int i = 0; i < simd_size; ++i)
            *dst++ = static_cast<char32_t>(src[i]);
        src += simd_size;
    }
#elif defined(LIBUNICODE_USE_INTRINSICS)
    #if defined(__x86_64__) || defined(_M_AMD64)
    using simd = intrinsics<SimdBitWidth>;
    while (src + simd_size <= src_end)
    {
        auto const batch = simd::load(reinterpret_cast<char const*>(src));
        if (!simd::all_ascii(batch))
            break;

        // All ASCII: zero-extend bytes to 32-bit using SIMD widening
        if constexpr (SimdBitWidth == 128)
        {
            // 16 bytes -> 4 rounds of 4 x char32_t
            simd::store(dst + 0, simd::cvtepu8_epi32(batch));
            simd::store(dst + 4, simd::cvtepu8_epi32(simd::template shift_right_bytes<4>(batch)));
            simd::store(dst + 8, simd::cvtepu8_epi32(simd::template shift_right_bytes<8>(batch)));
            simd::store(dst + 12, simd::cvtepu8_epi32(simd::template shift_right_bytes<12>(batch)));
        }
        else if constexpr (SimdBitWidth == 256)
        {
            // 32 bytes -> extract two 128-bit halves, each produces 8 x char32_t via two rounds
            using simd128 = intrinsics<128>;
            auto const lo = simd::extract_lo128(batch);
            auto const hi = simd::extract_hi128(batch);
            // Lower 16 bytes: 2 rounds of 8
            simd::store(dst + 0, simd::cvtepu8_epi32(lo));
            simd::store(dst + 8, simd::cvtepu8_epi32(simd128::template shift_right_bytes<8>(lo)));
            // Upper 16 bytes: 2 rounds of 8
            simd::store(dst + 16, simd::cvtepu8_epi32(hi));
            simd::store(dst + 24, simd::cvtepu8_epi32(simd128::template shift_right_bytes<8>(hi)));
        }
        else if constexpr (SimdBitWidth == 512)
        {
            // 64 bytes -> extract 4 x 128-bit lanes, each produces 16 x char32_t
            simd::store(dst + 0, simd::cvtepu8_epi32(simd::template extract_i32x4<0>(batch)));
            simd::store(dst + 16, simd::cvtepu8_epi32(simd::template extract_i32x4<1>(batch)));
            simd::store(dst + 32, simd::cvtepu8_epi32(simd::template extract_i32x4<2>(batch)));
            simd::store(dst + 48, simd::cvtepu8_epi32(simd::template extract_i32x4<3>(batch)));
        }
        src += simd_size;
        dst += simd_size;
    }
    #elif defined(__aarch64__) || defined(_M_ARM64)
    using simd = intrinsics<128>;
    static_assert(SimdBitWidth == 128, "ARM64 NEON only supports 128-bit SIMD");
    while (src + 16 <= src_end)
    {
        auto const batch = simd::load(reinterpret_cast<char const*>(src));
        if (!simd::all_ascii(batch))
            break;

        // 16 bytes -> 4 rounds of 4 x char32_t via NEON two-stage widening
        auto const lo8 = simd::get_low_u8(batch);
        auto const hi8 = simd::get_high_u8(batch);
        simd::store_u32(dst + 0, simd::cvtepu8_epi32_neon(lo8));
        simd::store_u32(dst + 4, simd::cvtepu8_epi32_high_neon(lo8));
        simd::store_u32(dst + 8, simd::cvtepu8_epi32_neon(hi8));
        simd::store_u32(dst + 12, simd::cvtepu8_epi32_high_neon(hi8));
        src += 16;
        dst += 16;
    }
    #endif
#endif // LIBUNICODE_USE_INTRINSICS

    // --- Scalar tail: process remaining bytes ---
    decoder<char> utf8_decoder {};
    while (src < src_end)
    {
        auto const result = utf8_decoder(static_cast<uint8_t>(*src++));
        if (result.has_value())
            *dst++ = result.value();
    }

    return static_cast<size_t>(dst - output);
}

// =====================================================================================
// UTF-8 -> UTF-16 SIMD-accelerated conversion
// =====================================================================================

/// Converts UTF-8 input to UTF-16 output using SIMD acceleration for ASCII runs.
///
/// @param input     Pointer to UTF-8 input bytes.
/// @param inputSize Number of input bytes.
/// @param output    Pointer to pre-allocated output buffer (must hold at least inputSize elements).
/// @return Number of char16_t values written to output.
template <size_t SimdBitWidth>
size_t convert_utf8_to_utf16_simd(char const* input, size_t inputSize, char16_t* output) noexcept
{
    [[maybe_unused]] constexpr int simd_size = SimdBitWidth / 8;
    auto const* src = reinterpret_cast<uint8_t const*>(input);
    auto const* src_end = src + inputSize;
    auto* dst = output;

    // --- SIMD ASCII fast path ---
#if defined(USE_STD_SIMD_CONVERT)
    while (src + simd_size <= src_end)
    {
        auto batch = convert_stdx::fixed_size_simd<signed char, simd_size> {};
        batch.copy_from(reinterpret_cast<signed char const*>(src), convert_stdx::element_aligned);
        auto const has_high_bit = batch < static_cast<signed char>(0);
        if (convert_stdx::any_of(has_high_bit))
            break;
        for (int i = 0; i < simd_size; ++i)
            *dst++ = static_cast<char16_t>(src[i]);
        src += simd_size;
    }
#elif defined(LIBUNICODE_USE_INTRINSICS)
    #if defined(__x86_64__) || defined(_M_AMD64)
    using simd = intrinsics<SimdBitWidth>;
    while (src + simd_size <= src_end)
    {
        auto const batch = simd::load(reinterpret_cast<char const*>(src));
        if (!simd::all_ascii(batch))
            break;

        // All ASCII: zero-extend bytes to 16-bit using SIMD widening
        if constexpr (SimdBitWidth == 128)
        {
            using simd128 = intrinsics<128>;
            // 16 bytes -> 2 rounds of 8 x char16_t
            simd128::store(dst + 0, simd128::cvtepu8_epi16(batch));
            simd128::store(dst + 8, simd128::cvtepu8_epi16(simd128::template shift_right_bytes<8>(batch)));
        }
        else if constexpr (SimdBitWidth == 256)
        {
            // 32 bytes -> extract two 128-bit halves, each produces 16 x char16_t
            auto const lo = simd::extract_lo128(batch);
            auto const hi = simd::extract_hi128(batch);
            simd::store(dst + 0, simd::cvtepu8_epi16(lo));
            simd::store(dst + 16, simd::cvtepu8_epi16(hi));
        }
        else if constexpr (SimdBitWidth == 512)
        {
            // 64 bytes -> extract two 256-bit halves, each produces 32 x char16_t
            auto const lo256 = _mm512_castsi512_si256(batch);
            auto const hi256 = _mm512_extracti64x4_epi64(batch, 1);
            simd::store(dst + 0, simd::cvtepu8_epi16(lo256));
            simd::store(dst + 32, simd::cvtepu8_epi16(hi256));
        }
        src += simd_size;
        dst += simd_size;
    }
    #elif defined(__aarch64__) || defined(_M_ARM64)
    using simd = intrinsics<128>;
    static_assert(SimdBitWidth == 128, "ARM64 NEON only supports 128-bit SIMD");
    while (src + 16 <= src_end)
    {
        auto const batch = simd::load(reinterpret_cast<char const*>(src));
        if (!simd::all_ascii(batch))
            break;

        // 16 bytes -> 2 rounds of 8 x char16_t via NEON widening
        auto const lo8 = simd::get_low_u8(batch);
        auto const hi8 = simd::get_high_u8(batch);
        simd::store_u16(dst + 0, simd::cvtepu8_epi16_neon(lo8));
        simd::store_u16(dst + 8, simd::cvtepu8_epi16_neon(hi8));
        src += 16;
        dst += 16;
    }
    #endif
#endif // LIBUNICODE_USE_INTRINSICS

    // --- Scalar tail: decode UTF-8, encode to UTF-16 ---
    decoder<char> utf8_decoder {};
    encoder<char16_t> utf16_encoder {};
    while (src < src_end)
    {
        auto const result = utf8_decoder(static_cast<uint8_t>(*src++));
        if (result.has_value())
            dst = utf16_encoder(result.value(), dst);
    }

    return static_cast<size_t>(dst - output);
}

} // namespace unicode::detail
