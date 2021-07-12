#pragma once

#include <unicode/detail/convert-common.hpp>
#include <cassert>

namespace unicode::accelerator
{
    struct avx512bw
    {
        static constexpr std::size_t alignment = 16;
    };
}

namespace unicode::detail
{

#if defined(__AVX512BW__)

template <>
LIBUNICODE_FORCE_INLINE
void convertAsciiBlockOnce<accelerator::avx512bw>(unsigned char const*& _begin, char32_t*& _output) noexcept
{
    assert(is_aligned(_begin, accelerator::avx512bw::alignment));

    // VMOVDQU: load 16 bytes
    __m128i input = _mm_loadu_si128((__m128i const*) _begin);

    // VPMOVMSKB: Determine which octets have high bit set
    uint32_t mask = _mm_movemask_epi8(input);

    // VPMOVXZBD: packed zero-extend bytes to DWORD's
    __m512i extended = _mm512_cvtepu8_epi32(input);

    // VMOVDQA32: Write to memory
    _mm512_store_epi64(_output, extended);

    auto const incr = mask == 0 ? 16 : trailingZeros(mask);
    _begin += incr;
    _output += incr;
}

#endif // __AVX512BW__

}
