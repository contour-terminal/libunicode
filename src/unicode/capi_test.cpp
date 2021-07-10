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
#include <unicode/capi.h>
#include <catch2/catch_all.hpp>

#include <fmt/format.h>

#include <array>
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
        u32_char_t out{};
        if (u8u32_stream_convert_run(conv, input.at(i), &out))
            output.push_back(out);
    }
    CHECK(output == expected);
    u8u32_stream_convert_destroy(&conv);

    // Verify inverse conversion (UTF-32 to UTF-8) works, too.
    array<u8_char_t, 32> inverse{};
    auto const ilen = u32u8_convert((u32_char_t const*) expected.data(), expected.size(),
                                    inverse.data(), inverse.size());
    CHECK(ilen == (int)input.size());
    auto const inverseSV = string_view{inverse.data(), static_cast<size_t>(ilen)};
    CHECK(inverseSV == input);
}

// TODO more C-API tests
