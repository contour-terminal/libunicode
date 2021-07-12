#pragma once

#include <cstdint>
#include <cstddef>

#include <emmintrin.h>
#include <xmmintrin.h>
#include <immintrin.h>

#if !defined(_MSC_VER)
    #define LIBUNICODE_ALIGNED_FUNC  __attribute__((aligned (128)))
    #ifdef __OPTIMIZE__
        #define LIBUNICODE_FORCE_INLINE inline __attribute__((always_inline))
    #else
        #define LIBUNICODE_FORCE_INLINE inline
    #endif
#else
    #define LIBUNICODE_ALIGNED_FUNC
    #define LIBUNICODE_FORCE_INLINE inline
#endif

#if defined(__SSE2__) || defined(_M_AMD64) || defined(_M_IX86_FP)
#define LIBUNICODE_TARGET_SSE2 1
#endif

namespace unicode
{
    struct decoder_status
    {
        bool success;
        std::size_t read_offset;
        std::size_t write_offset;
    };
}

namespace unicode::detail
{

template <typename T>
LIBUNICODE_FORCE_INLINE
bool is_ascii(T _char) noexcept
{
    return _char < 0x80;
}

inline bool is_aligned(void const* _pointer, size_t _byte_count) noexcept
{
    return uintptr_t(_pointer) % _byte_count == 0;
}

LIBUNICODE_FORCE_INLINE uint32_t trailingZeros(int32_t _value) noexcept
{
#if ((defined(__linux__) && (defined(__clang__) || defined(__GNUC__))) || \
        (defined(__WIN32) && !defined(_MSVC)) || \
        defined(__APPLE__))
    return  __builtin_ctz((unsigned int) _value);
#elif defined(_WIN32) // && defined(_MSVC)
    unsigned long count;
    _BitScanForward(&count, (unsigned long) _value);
    return static_cast<int32_t>(count);
#else
    #error Unsupported Platform / Compiler
#endif
}

template <typename Accelerator>
void convertAsciiBlockOnce(unsigned char const*& _begin, char32_t*& _output) noexcept;

}
