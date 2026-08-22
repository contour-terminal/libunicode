/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2021 Christian Parpart <christian@parpart.family>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <libunicode/capi.h>

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <format>
#include <utility>

using namespace std;
using namespace std::string_view_literals;

TEST_CASE("capi.gc_count")
{
    CHECK(0 == u32_gc_count((u32_char_t const*) U"", 0));
    CHECK(1 == u32_gc_count((u32_char_t const*) U"\U0001F600\uFE0E", 2));
    CHECK(2 == u32_gc_count((u32_char_t const*) U"\U0001F600\uFE0E\U0001F600", 3));
    CHECK(3 == u32_gc_count((u32_char_t const*) U"Yeo", 3));
    CHECK(4 == u32_gc_count((u32_char_t const*) U"Hi \U0001F600\uFE0E", 5));
    CHECK(4 == u32_gc_count((u32_char_t const*) U"1234", 4));
    CHECK(1 == u32_gc_count((u32_char_t const*) U"\U0001F468\U0001F3FE\u200D\U0001F9B3", 4));
}

TEST_CASE("capi.u8u32_stream_convert_and_inverse")
{
    auto constexpr input = "[\xC3\xB6\xE2\x82\xAC\xF0\x9F\x98\x80"sv;
    auto constexpr expected = U"[ö€😀"sv;

    u8u32_stream_state_t conv = u8u32_stream_convert_create();
    u32string output;
    for (size_t i = 0; i < input.size(); ++i)
    {
        u32_char_t out {};
        if (u8u32_stream_convert_run(conv, input.at(i), &out))
            output.push_back(out);
    }
    CHECK(output == expected);
    u8u32_stream_convert_destroy(&conv);

    // Verify inverse conversion (UTF-32 to UTF-8) works, too.
    array<u8_char_t, 32> inverse {};
    auto const ilen = u32u8_convert((u32_char_t const*) expected.data(), expected.size(), inverse.data(), inverse.size());
    CHECK(ilen == (int) input.size());
    auto const inverseSV = string_view { inverse.data(), static_cast<size_t>(ilen) };
    CHECK(inverseSV == input);
}

TEST_CASE("capi.u32_gc_width_modifiable")
{
    // ☝ (U+261D) alone is width 1; with VS16 (emoji presentation) the whole
    // cluster becomes width 2. MODIFIABLE mode measures the cluster as a
    // whole, so it must see the VS16 effect.
    u32_char_t const text[] = { 0x261D, 0xFE0F };
    CHECK(u32_gc_width(text, 2, GC_WIDTH_MODE_MODIFIABLE) == 2);

    // "A" + the same cluster: 1 + 2 = 3.
    u32_char_t const text2[] = { 'A', 0x261D, 0xFE0F };
    CHECK(u32_gc_width(text2, 3, GC_WIDTH_MODE_MODIFIABLE) == 3);
}

TEST_CASE("capi.u32_gc_width_non_modifiable")
{
    // Same input, but NON_MODIFIABLE mode measures only the base codepoint
    // (☝ alone is width 1) and ignores the VS16 that follows it -- the
    // trailing VS16 contributes nothing of its own (it's part of the same
    // cluster, whose width was already taken from the base), so the total
    // stays 1, not 2.
    u32_char_t const text[] = { 0x261D, 0xFE0F };
    CHECK(u32_gc_width(text, 2, GC_WIDTH_MODE_NON_MODIFIABLE) == 1);
}

TEST_CASE("capi.u8_gc_width")
{
    // UTF-8 encoding of the same ☝ U+FE0F pair: \xE2\x98\x9D\xEF\xB8\x8F
    auto constexpr input = "\xE2\x98\x9D\xEF\xB8\x8F"sv;
    CHECK(u8_gc_width((u8_char_t const*) input.data(), input.size(), GC_WIDTH_MODE_MODIFIABLE) == 2);
    CHECK(u8_gc_width((u8_char_t const*) input.data(), input.size(), GC_WIDTH_MODE_NON_MODIFIABLE) == 1);
}

TEST_CASE("capi.u32_grapheme_unbreakable")
{
    // CR x LF (GB3): unbreakable.
    CHECK(u32_grapheme_unbreakable(0x000D, 0x000A) == 1);

    // 'a' + combining diaeresis (GB9, Extend): unbreakable.
    CHECK(u32_grapheme_unbreakable('a', 0x0308) == 1);

    // Two plain ASCII letters: breakable.
    CHECK(u32_grapheme_unbreakable('a', 'b') == 0);
}

// TODO more C-API tests
