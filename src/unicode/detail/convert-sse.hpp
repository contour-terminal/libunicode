#pragma once

#include <unicode/detail/convert-common.hpp>
#include <cassert>

namespace unicode::accelerator
{
    struct sse
    {
        static constexpr std::size_t alignment = 16;
    };
};

namespace unicode::detail
{

#if defined(LIBUNICODE_TARGET_SSE2)

template <>
LIBUNICODE_FORCE_INLINE
void convertAsciiBlockOnce<accelerator::sse>(unsigned char const*& _begin, char32_t*& _output) noexcept
{
    // Ensure 16-byte alignment.
    // If not aligned, _mm_loadu_si128 should be used.
    assert(is_aligned(_begin, accelerator::sse::alignment));

    __m128i zero  = _mm_set1_epi8(0);                        // Zero out the interleave register
    __m128i chunk = _mm_load_si128((__m128i const*) _begin); // Load a register with 8-bit bytes
    int32_t mask  = _mm_movemask_epi8(chunk);                // Determine which octets have high bit set

    __m128i half  = _mm_unpacklo_epi8(chunk, zero);     // Unpack bytes 0-7 into 16-bit words
    __m128i qrtr  = _mm_unpacklo_epi16(half, zero);     // Unpack words 0-3 into 32-bit dwords
    _mm_storeu_si128((__m128i*) _output, qrtr);         // Write to memory
    qrtr = _mm_unpackhi_epi16(half, zero);              // Unpack words 4-7 into 32-bit dwords
    _mm_storeu_si128((__m128i*) (_output + 4), qrtr);   // Write to memory

    half = _mm_unpackhi_epi8(chunk, zero);              // Unpack bytes 8-15 into 16-bit words
    qrtr = _mm_unpacklo_epi16(half, zero);              // Unpack words 8-11 into 32-bit dwords
    _mm_storeu_si128((__m128i*) (_output + 8), qrtr);   // Write to memory
    qrtr = _mm_unpackhi_epi16(half, zero);              // Unpack words 12-15 into 32-bit dwords
    _mm_storeu_si128((__m128i*) (_output + 12), qrtr);  // Write to memory

    auto const incr = mask == 0 ? 16 : trailingZeros(mask);
    _begin += incr;
    _output += incr;
}

#endif

}
