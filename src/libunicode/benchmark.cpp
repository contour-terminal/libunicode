#include <libunicode/convert.h>
#include <libunicode/scan.h>
#include <libunicode/utf8.h>

#include <string_view>

#include <benchmark/benchmark.h>

using std::string_view;

template <size_t L>
static void benchmarkWithLength(benchmark::State& benchmarkState)
{
    auto TestText = std::string(L, 'a') + "\u00A9";
    for (auto _: benchmarkState)
    {
        benchmark::DoNotOptimize(unicode::detail::scan_for_text_ascii(TestText, L + 10));
    }
}

template <size_t L>
static void benchmarkWithOffset(benchmark::State& benchmarkState)
{
    auto TestText = std::string(L, 'a') + "\U0001F600" + std::string(1000, 'a');
    for (auto _: benchmarkState)
    {
        benchmark::DoNotOptimize(unicode::detail::scan_for_text_ascii(TestText, L + 10));
    }
}

BENCHMARK(benchmarkWithLength<1>);
BENCHMARK(benchmarkWithLength<10>);
BENCHMARK(benchmarkWithLength<100>);
BENCHMARK(benchmarkWithLength<1000>);
BENCHMARK(benchmarkWithLength<10000>);
BENCHMARK(benchmarkWithLength<100000>);
BENCHMARK(benchmarkWithLength<1000000>);

BENCHMARK(benchmarkWithOffset<5>);
BENCHMARK(benchmarkWithOffset<10>);
BENCHMARK(benchmarkWithOffset<15>);
BENCHMARK(benchmarkWithOffset<20>);
BENCHMARK(benchmarkWithOffset<25>);
BENCHMARK(benchmarkWithOffset<30>);
BENCHMARK(benchmarkWithOffset<35>);
BENCHMARK(benchmarkWithOffset<40>);
BENCHMARK(benchmarkWithOffset<45>);
BENCHMARK(benchmarkWithOffset<50>);
BENCHMARK(benchmarkWithOffset<55>);
BENCHMARK(benchmarkWithOffset<60>);
BENCHMARK(benchmarkWithOffset<65>);
BENCHMARK(benchmarkWithOffset<70>);
BENCHMARK(benchmarkWithOffset<75>);
BENCHMARK(benchmarkWithOffset<80>);
BENCHMARK(benchmarkWithOffset<85>);
BENCHMARK(benchmarkWithOffset<90>);
BENCHMARK(benchmarkWithOffset<95>);
BENCHMARK(benchmarkWithOffset<100>);
BENCHMARK(benchmarkWithOffset<105>);
BENCHMARK(benchmarkWithOffset<110>);
BENCHMARK(benchmarkWithOffset<115>);
BENCHMARK(benchmarkWithOffset<120>);
BENCHMARK(benchmarkWithOffset<125>);
BENCHMARK(benchmarkWithOffset<130>);

// --- UTF-8 -> UTF-32 conversion benchmarks ---

template <size_t L>
static void BM_convert_utf8_to_utf32_ascii(benchmark::State& benchmarkState)
{
    auto const input = std::string(L, 'A');
    auto const sv = std::string_view(input);
    for (auto _: benchmarkState)
    {
        benchmark::DoNotOptimize(unicode::convert_to<char32_t>(sv));
    }
}

template <size_t L>
static void BM_convert_utf8_to_utf32_mixed(benchmark::State& benchmarkState)
{
    // Build input: 16 ASCII chars then a 4-byte emoji, repeating
    std::string input;
    while (input.size() < L)
        input += std::string(16, 'x') + "\xF0\x9F\x98\x80";
    input.resize(L);
    auto const sv = std::string_view(input);
    for (auto _: benchmarkState)
    {
        benchmark::DoNotOptimize(unicode::convert_to<char32_t>(sv));
    }
}

template <size_t L>
static void BM_convert_utf8_to_utf16_ascii(benchmark::State& benchmarkState)
{
    auto const input = std::string(L, 'B');
    auto const sv = std::string_view(input);
    for (auto _: benchmarkState)
    {
        benchmark::DoNotOptimize(unicode::convert_to<char16_t>(sv));
    }
}

BENCHMARK(BM_convert_utf8_to_utf32_ascii<16>);
BENCHMARK(BM_convert_utf8_to_utf32_ascii<64>);
BENCHMARK(BM_convert_utf8_to_utf32_ascii<256>);
BENCHMARK(BM_convert_utf8_to_utf32_ascii<1024>);
BENCHMARK(BM_convert_utf8_to_utf32_ascii<65536>);
BENCHMARK(BM_convert_utf8_to_utf32_ascii<1048576>);

BENCHMARK(BM_convert_utf8_to_utf32_mixed<256>);
BENCHMARK(BM_convert_utf8_to_utf32_mixed<1024>);
BENCHMARK(BM_convert_utf8_to_utf32_mixed<65536>);

BENCHMARK(BM_convert_utf8_to_utf16_ascii<16>);
BENCHMARK(BM_convert_utf8_to_utf16_ascii<64>);
BENCHMARK(BM_convert_utf8_to_utf16_ascii<256>);
BENCHMARK(BM_convert_utf8_to_utf16_ascii<1024>);
BENCHMARK(BM_convert_utf8_to_utf16_ascii<65536>);
BENCHMARK(BM_convert_utf8_to_utf16_ascii<1048576>);

// Run the benchmark
BENCHMARK_MAIN();
