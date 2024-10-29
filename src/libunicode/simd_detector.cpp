#include "simd_detector.h"

#include <cstdint>

// AVX512 required:
// AVX512_BITALG : popcnt
// AVX512_BW : compare greater (less is needed)
// AVX512_F : and
//
// auto max_simd_size() -> size_t;

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
    #if _WIN32
    // clang-format off
        #include <Windows.h>
        #include <intrin.h>
        void cpuid(int32_t out[4], int32_t eax, int32_t ecx)
        {
            __cpuidex(out, eax, ecx);
        }
        __int64 xgetbv(unsigned int x)
        {
            return _xgetbv(x);
        }
    // clang-format on
    #elif defined(__GNUC__) || defined(__clang__)
    // clang-format off
        #include <cpuid.h>
        void cpuid(int32_t out[4], int32_t eax, int32_t ecx)
        {
            __cpuid_count(eax, ecx, out[0], out[1], out[2], out[3]);
        }
        uint64_t xgetbv(unsigned int index)
        {
            uint32_t eax, edx;
            __asm__ __volatile__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(index));
            return ((uint64_t) edx << 32) | eax;
        }
        #define XCR_XFEATURE_ENABLED_MASK 0
    // clang-format on
    #else
        #error "No cpuid intrinsic defined for compiler."
    #endif

auto detect_os_avx() -> bool
{
    //  Copied from: http://stackoverflow.com/a/22521619/922184
    bool avxSupported = false;
    int32_t cpuInfo[4];
    cpuid(cpuInfo, 1, 0);

    bool osUsesXSAVE_XRSTORE = (cpuInfo[2] & (1 << 27)) != 0;
    bool cpuAVXSuport = (cpuInfo[2] & (1 << 28)) != 0;

    if (osUsesXSAVE_XRSTORE && cpuAVXSuport)
    {
        uint64_t xcrFeatureMask = xgetbv(XCR_XFEATURE_ENABLED_MASK);
        avxSupported = (xcrFeatureMask & 0x6) == 0x6;
    }

    return avxSupported;
}

auto detect_os_avx512() -> bool
{
    if (!detect_os_avx())
        return false;
    uint64_t xcrFeatureMask = xgetbv(XCR_XFEATURE_ENABLED_MASK);
    return (xcrFeatureMask & 0xe6) == 0xe6;
}

auto unicode::detail::max_simd_size() -> size_t
{
    if (!detect_os_avx())
        return 128;

    int32_t info[4];
    cpuid(info, 0, 0);
    int nIds = info[0];

    // cpuid(info, 0x80000000, 0);
    // uint32_t nExIds = info[0];

    //  Detect Features
    // if (nIds >= 0x00000001)
    // {
    //     cpuid(info, 0x00000001, 0);
    //     bool HW_MMX = (info[3] & ((int) 1 << 23)) != 0;
    //     bool HW_SSE = (info[3] & ((int) 1 << 25)) != 0;
    //     bool HW_SSE2 = (info[3] & ((int) 1 << 26)) != 0;
    //     bool HW_SSE3 = (info[2] & ((int) 1 << 0)) != 0;
    //
    //     bool HW_SSSE3 = (info[2] & ((int) 1 << 9)) != 0;
    //     bool HW_SSE41 = (info[2] & ((int) 1 << 19)) != 0;
    //     bool HW_SSE42 = (info[2] & ((int) 1 << 20)) != 0;
    //     bool HW_AES = (info[2] & ((int) 1 << 25)) != 0;
    //
    //     bool HW_AVX = (info[2] & ((int) 1 << 28)) != 0;
    //     bool HW_FMA3 = (info[2] & ((int) 1 << 12)) != 0;
    //
    //     bool HW_RDRAND = (info[2] & ((int) 1 << 30)) != 0;
    // }
    if (nIds >= 0x00000007)
    {
        cpuid(info, 0x00000007, 0);
        bool HW_AVX2 = (info[1] & ((int) 1 << 5)) != 0;
        if (!HW_AVX2)
            return 128;

        // bool HW_BMI1 = (info[1] & ((int) 1 << 3)) != 0;
        // bool HW_BMI2 = (info[1] & ((int) 1 << 8)) != 0;
        // bool HW_ADX = (info[1] & ((int) 1 << 19)) != 0;
        // bool HW_MPX = (info[1] & ((int) 1 << 14)) != 0;
        // bool HW_SHA = (info[1] & ((int) 1 << 29)) != 0;
        // bool HW_RDSEED = (info[1] & ((int) 1 << 18)) != 0;
        // bool HW_PREFETCHWT1 = (info[2] & ((int) 1 << 0)) != 0;
        // bool HW_RDPID = (info[2] & ((int) 1 << 22)) != 0;

        bool HW_AVX512_F = (info[1] & ((int) 1 << 16)) != 0;
        // bool HW_AVX512_CD = (info[1] & ((int) 1 << 28)) != 0;
        // bool HW_AVX512_PF = (info[1] & ((int) 1 << 26)) != 0;
        // bool HW_AVX512_ER = (info[1] & ((int) 1 << 27)) != 0;

        // bool HW_AVX512_VL = (info[1] & ((int) 1 << 31)) != 0;
        bool HW_AVX512_BW = (info[1] & ((int) 1 << 30)) != 0;
        // bool HW_AVX512_DQ = (info[1] & ((int) 1 << 17)) != 0;

        // bool HW_AVX512_IFMA = (info[1] & ((int) 1 << 21)) != 0;
        // bool HW_AVX512_VBMI = (info[2] & ((int) 1 << 1)) != 0;

        // bool HW_AVX512_VPOPCNTDQ = (info[2] & ((int) 1 << 14)) != 0;
        // bool HW_AVX512_4VNNIW = (info[3] & ((int) 1 << 2)) != 0;
        // bool HW_AVX512_4FMAPS = (info[3] & ((int) 1 << 3)) != 0;

        // bool HW_AVX512_VNNI = (info[2] & ((int) 1 << 11)) != 0;

        // bool HW_AVX512_VBMI2 = (info[2] & ((int) 1 << 6)) != 0;
        // bool HW_GFNI = (info[2] & ((int) 1 << 8)) != 0;
        // bool HW_VAES = (info[2] & ((int) 1 << 9)) != 0;
        // bool HW_AVX512_VPCLMUL = (info[2] & ((int) 1 << 10)) != 0;
        bool HW_AVX512_BITALG = (info[2] & ((int) 1 << 12)) != 0;

        bool use512 = detect_os_avx512() && HW_AVX512_F && HW_AVX512_BW && HW_AVX512_BITALG;
        if (!use512)
            return 256;
        else
            return 512;

        // cpuid(info, 0x00000007, 1);
        // bool HW_AVX512_BF16 = (info[0] & ((int) 1 << 5)) != 0;
    }
    return 128;
    // if (nExIds >= 0x80000001)
    // {
    //     cpuid(info, 0x80000001, 0);
    //     bool HW_x64 = (info[3] & ((int) 1 << 29)) != 0;
    //     bool HW_ABM = (info[2] & ((int) 1 << 5)) != 0;
    //     bool HW_SSE4a = (info[2] & ((int) 1 << 6)) != 0;
    //     bool HW_FMA4 = (info[2] & ((int) 1 << 16)) != 0;
    //     bool HW_XOP = (info[2] & ((int) 1 << 11)) != 0;
    //     bool HW_PREFETCHW = (info[2] & ((int) 1 << 8)) != 0;
    // }
}

#else
auto unicode::detail::max_simd_size() -> size_t
{
    return 128;
}
#endif
