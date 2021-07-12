#pragma once

#include <unicode/detail/convert-common.hpp>
#include <cassert>

#include <immintrin.h>

namespace unicode::accelerator
{
    struct avx512
    {
        static constexpr std::size_t alignment = 16;
    };
};

namespace unicode::detail
{

#if defined(__AVX512BW__)

template <>
LIBUNICODE_FORCE_INLINE
void convertAsciiBlockOnce<accelerator::avx512>(unsigned char const*& _begin, char32_t*& _output) noexcept
{
    assert(is_aligned(_begin, accelerator::avx512::alignment));

    // VPUNPCKLBW: _mm256_unpacklo_epi8

    // Input: 16 codepoints input (128 bit)
    // Output: up to 16 zero-extended 32-bit values, and pointers incremented accordingly

    __m128i  input    = _mm_loadu_si128((__m128i const*) _begin); // VMOVDQU: load 16 bytes
    uint32_t mask     = _mm_movemask_epi8(input);                 // VPMOVMSKB: Determine which octets have high bit set

    //__m256i  extended = _mm128_cvtepu8_epi16(input);              // VPMOVXZBD: packed zero-extend bytes to DWORD's
    //_mm256_store_epi32(_output, extended);                        // VMOVDQA32: Write to memory
    __m512i extended = _mm512_cvtepu8_epi32(input); // zero extend input bytes to words.

    auto zero = _mm_set1_epi8(0);                           //- Zero out the interleave register
    //auto zero = _mm256_set1_epi16(0);                           //- Zero out the interleave register
    auto a    = _mm_unpacklo_epi8(input, zero);
    _mm256_storeu_si256((__m256i *) _output, a);            //- Write to memory

#if 1
    auto const incr = /* mask == 0 ? 16 : */ trailingZeros(mask);
    _begin += incr;
    _output += incr;
#else
    if (mask == 0) {
        _begin += 16;
        _output += 16;
    } else {
        auto const incr = trailingZeros(mask);
        _begin += incr;
        _output += incr;
    }
#endif
}

#endif // __AVX512BW__

}
