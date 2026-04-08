/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2020 Christian Parpart <christian@parpart.family>
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
#include <libunicode/convert.h>
#include <libunicode/support.h>
#include <libunicode/utf8.h>

#include <catch2/catch_test_macros.hpp>

#include <format>
#include <iterator>

using namespace unicode;
using namespace std::string_literals;
using namespace std;

TEST_CASE("convert.same", "[convert]")
{
    auto const s8 = "Hello, 😀"sv;
    auto t8 = string {};
    unicode::convert_to<char>(s8, back_insert_iterator(t8));
    CHECK(s8 == t8);

    auto const s16 = u"Hello, 😀"sv;
    auto t16 = u16string {};
    unicode::convert_to<char16_t>(s16, back_insert_iterator(t16));
    CHECK(s16 == t16);

    auto const s32 = U"Hello, 😀"sv;
    auto t32 = u32string {};
    unicode::convert_to<char32_t>(s32, back_insert_iterator(t32));
    CHECK(s32 == t32);
}

TEST_CASE("convert.8_to_16", "[convert]")
{
    auto constexpr input = string_view {
        "["
        "\xC3\xB6"         // ö  - german o-umlaut
        "\xE2\x82\xAC"     // €  - EURO sign U+20AC
        "\xF0\x9F\x98\x80" // 😀 - U+1F600
    };
    u16string output;
    auto bi = back_inserter(output);
    unicode::convert_to<char16_t>(input, bi); // back_inserter(output));
    CHECK(output.size() == 5);
    CHECK(output == u"[ö€😀");
}

TEST_CASE("convert.8_to_32", "[convert]")
{
    auto constexpr input = string_view {
        "["
        "\xC3\xB6"         // ö  - german o-umlaut
        "\xE2\x82\xAC"     // €  - EURO sign U+20AC
        "\xF0\x9F\x98\x80" // 😀 - U+1F600
    };
    u32string output;
    auto bi = back_inserter(output);
    unicode::convert_to<char32_t>(input, bi); // back_inserter(output));
    CHECK(output.size() == 4);
    CHECK(output == U"[ö€😀");
}

TEST_CASE("convert.16_to_8", "[convert]")
{
    auto constexpr input = u"[ö€😀"sv;
    auto output = string {};
    unicode::convert_to<char>(input, back_inserter(output));
    CHECK(output.size() == 10); // 1 + 2 + 3 + 4
    CHECK(output == "[ö€😀");
}

TEST_CASE("convert.16_to_32", "[convert]")
{
    auto constexpr input = u"[ö€😀"sv;
    auto output = u32string {};
    unicode::convert_to<char32_t>(input, back_inserter(output));
    CHECK(output.size() == 4);
    CHECK(output == U"[ö€😀");
}

TEST_CASE("convert.32_to_8", "[convert]")
{
    auto constexpr input = U"[ö€😀"sv;
    auto output = string {};
    unicode::convert_to<char>(input, back_inserter(output));
    CHECK(output.size() == 10); // 1 + 2 + 3 + 4
    CHECK(output == "[ö€😀");
}

TEST_CASE("convert.32_to_16", "[convert]")
{
    auto constexpr input = U"[ö€😀"sv;
    auto output = u16string {};
    unicode::convert_to<char16_t>(input, back_inserter(output));
    CHECK(output.size() == 5); // 1 + 1 + 1 + 2 (surrogate pair)
    CHECK(output == u"[ö€😀");
}

TEST_CASE("convert.surrogate_pair_boundaries", "[convert]")
{
    // U+10000: minimum supplementary codepoint (first surrogate pair)
    {
        auto const cp = U"\U00010000"sv;
        auto u16 = u16string {};
        unicode::convert_to<char16_t>(cp, back_inserter(u16));
        REQUIRE(u16.size() == 2);
        CHECK(u16[0] == char16_t(0xD800));
        CHECK(u16[1] == char16_t(0xDC00));

        // Round-trip back to UTF-32
        auto u32 = u32string {};
        unicode::convert_to<char32_t>(u16string_view(u16), back_inserter(u32));
        REQUIRE(u32.size() == 1);
        CHECK(u32[0] == U'\U00010000');
    }

    // U+10FFFF: maximum valid Unicode codepoint
    {
        auto const cp = U"\U0010FFFF"sv;
        auto u16 = u16string {};
        unicode::convert_to<char16_t>(cp, back_inserter(u16));
        REQUIRE(u16.size() == 2);
        CHECK(u16[0] == char16_t(0xDBFF));
        CHECK(u16[1] == char16_t(0xDFFF));

        // Round-trip back to UTF-32
        auto u32 = u32string {};
        unicode::convert_to<char32_t>(u16string_view(u16), back_inserter(u32));
        REQUIRE(u32.size() == 1);
        CHECK(u32[0] == U'\U0010FFFF');
    }

    // U+1F600: 😀 emoji (typical supplementary plane character)
    {
        auto const cp = U"\U0001F600"sv;
        auto u16 = u16string {};
        unicode::convert_to<char16_t>(cp, back_inserter(u16));
        REQUIRE(u16.size() == 2);
        CHECK(u16[0] == char16_t(0xD83D));
        CHECK(u16[1] == char16_t(0xDE00));

        auto u32 = u32string {};
        unicode::convert_to<char32_t>(u16string_view(u16), back_inserter(u32));
        REQUIRE(u32.size() == 1);
        CHECK(u32[0] == U'\U0001F600');
    }
}

TEST_CASE("convert.utf16_invalid_surrogates", "[convert]")
{
    // Lone high surrogate (0xD800) without a following low surrogate
    {
        auto const input = u16string { char16_t(0xD800), char16_t(0x0041) }; // high surrogate + 'A'
        auto output = u32string {};
        unicode::convert_to<char32_t>(u16string_view(input), back_inserter(output));
        // The decoder should reject the invalid pair and skip to 'A'
        CHECK(output.empty()); // decoder returns nullopt for bad pair, then 'A' is consumed as ch1
    }

    // Lone low surrogate (0xDC00) at start
    {
        auto const input = u16string { char16_t(0xDC00) };
        auto output = u32string {};
        unicode::convert_to<char32_t>(u16string_view(input), back_inserter(output));
        CHECK(output.empty()); // standalone low surrogate is invalid
    }

    // Surrogate range codepoint fed to UTF-16 encoder should be silently dropped
    {
        auto const input = U"\xD800"sv; // surrogate codepoint
        auto output = u16string {};
        unicode::convert_to<char16_t>(input, back_inserter(output));
        CHECK(output.empty()); // encoder rejects codepoints in surrogate range
    }
}

// =====================================================================================
// SIMD conversion boundary tests
// =====================================================================================

TEST_CASE("convert.simd.utf8_to_utf32_ascii_boundaries", "[convert][simd]")
{
    // Test pure ASCII strings at various lengths to exercise SIMD boundaries
    // (128-bit: 16 bytes, 256-bit: 32 bytes, 512-bit: 64 bytes)
    for (auto const len: { 0, 1, 7, 15, 16, 17, 31, 32, 33, 63, 64, 65, 127, 128, 255, 256, 1000 })
    {
        auto const input = std::string(static_cast<size_t>(len), 'A');
        auto const expected = std::u32string(static_cast<size_t>(len), U'A');
        auto const result = unicode::convert_to<char32_t>(std::string_view(input));
        CHECK(result.size() == expected.size());
        CHECK(result == expected);
    }
}

TEST_CASE("convert.simd.utf8_to_utf16_ascii_boundaries", "[convert][simd]")
{
    for (auto const len: { 0, 1, 7, 15, 16, 17, 31, 32, 33, 63, 64, 65, 127, 128, 255, 256, 1000 })
    {
        auto const input = std::string(static_cast<size_t>(len), 'B');
        auto const expected = std::u16string(static_cast<size_t>(len), u'B');
        auto const result = unicode::convert_to<char16_t>(std::string_view(input));
        CHECK(result.size() == expected.size());
        CHECK(result == expected);
    }
}

TEST_CASE("convert.simd.utf8_to_utf32_ascii_then_multibyte", "[convert][simd]")
{
    // ASCII prefix (exercises SIMD) + multi-byte tail (exercises scalar)
    SECTION("16 ASCII + 2-byte")
    {
        auto const input = std::string(16, 'x') + "\xC3\xB6"; // ö
        auto const result = unicode::convert_to<char32_t>(std::string_view(input));
        CHECK(result.size() == 17);
        CHECK(result.back() == U'\u00F6');
    }
    SECTION("32 ASCII + 3-byte")
    {
        auto const input = std::string(32, 'y') + "\xE2\x82\xAC"; // €
        auto const result = unicode::convert_to<char32_t>(std::string_view(input));
        CHECK(result.size() == 33);
        CHECK(result.back() == U'\u20AC');
    }
    SECTION("64 ASCII + 4-byte emoji")
    {
        auto const input = std::string(64, 'z') + "\xF0\x9F\x98\x80"; // 😀
        auto const result = unicode::convert_to<char32_t>(std::string_view(input));
        CHECK(result.size() == 65);
        CHECK(result.back() == U'\U0001F600');
    }
}

TEST_CASE("convert.simd.utf8_to_utf16_ascii_then_multibyte", "[convert][simd]")
{
    SECTION("32 ASCII + surrogate-pair emoji")
    {
        auto const input = std::string(32, 'w') + "\xF0\x9F\x98\x80"; // 😀
        auto const result = unicode::convert_to<char16_t>(std::string_view(input));
        CHECK(result.size() == 34); // 32 + 2 (surrogate pair)
        CHECK(result[32] == char16_t(0xD83D));
        CHECK(result[33] == char16_t(0xDE00));
    }
}

TEST_CASE("convert.simd.utf8_to_utf32_non_ascii_only", "[convert][simd]")
{
    // No ASCII at all -- SIMD loop should be skipped entirely
    SECTION("2-byte codepoints")
    {
        auto constexpr input = "\xC3\xB6\xC3\xBC\xC3\xA4"sv; // öüä
        auto const result = unicode::convert_to<char32_t>(input);
        CHECK(result == U"öüä");
    }
    SECTION("3-byte CJK")
    {
        auto constexpr input = "\xE4\xB8\xAD\xE6\x96\x87"sv; // 中文
        auto const result = unicode::convert_to<char32_t>(input);
        CHECK(result == U"中文");
    }
    SECTION("4-byte emoji")
    {
        auto constexpr input = "\xF0\x9F\x98\x80\xF0\x9F\x98\x82"sv; // 😀😂
        auto const result = unicode::convert_to<char32_t>(input);
        CHECK(result == U"😀😂");
    }
}

TEST_CASE("convert.simd.utf8_to_utf32_interleaved", "[convert][simd]")
{
    // Alternating ASCII and non-ASCII: SIMD enters and exits repeatedly
    auto const input =
        std::string(20, 'A') + "\xF0\x9F\x98\x80" + std::string(20, 'B') + "\xF0\x9F\x98\x82" + std::string(20, 'C');
    auto const result = unicode::convert_to<char32_t>(std::string_view(input));
    CHECK(result.size() == 62); // 20 + 1 + 20 + 1 + 20
    CHECK(result[20] == U'\U0001F600');
    CHECK(result[41] == U'\U0001F602');
}

TEST_CASE("convert.simd.utf8_to_utf32_roundtrip", "[convert][simd]")
{
    // Verify roundtrip: UTF-8 -> UTF-32 -> UTF-8
    auto constexpr input = "Hello \xC3\xB6 World \xE2\x82\xAC \xF0\x9F\x98\x80 end"sv;
    auto const utf32 = unicode::convert_to<char32_t>(input);
    auto const roundtrip = unicode::convert_to<char>(std::u32string_view(utf32));
    CHECK(roundtrip == input);
}

TEST_CASE("convert.simd.utf8_to_utf16_roundtrip", "[convert][simd]")
{
    auto constexpr input = "Hello \xC3\xB6 World \xE2\x82\xAC \xF0\x9F\x98\x80 end"sv;
    auto const utf16 = unicode::convert_to<char16_t>(input);
    auto const roundtrip = unicode::convert_to<char>(std::u16string_view(utf16));
    CHECK(roundtrip == input);
}

TEST_CASE("convert.simd.from_utf8_consistency", "[convert][simd]")
{
    // Verify from_utf8() and convert_to<char32_t>() produce identical results
    auto constexpr input = "ABCDEFGHIJKLMNOP\xC3\xB6\xE2\x82\xAC\xF0\x9F\x98\x80 tail"sv;
    auto const via_convert = unicode::convert_to<char32_t>(input);
    auto const via_from_utf8 = unicode::from_utf8(input);
    CHECK(via_convert == via_from_utf8);
}

TEST_CASE("convert.simd.utf8_to_utf32_full_ascii_range", "[convert][simd]")
{
    // All printable ASCII characters (0x20..0x7E)
    std::string input;
    std::u32string expected;
    for (auto ch = 0x20; ch <= 0x7E; ++ch)
    {
        input += static_cast<char>(ch);
        expected += static_cast<char32_t>(ch);
    }
    auto const result = unicode::convert_to<char32_t>(std::string_view(input));
    CHECK(result == expected);
}

TEST_CASE("convert.simd.utf8_to_utf32_string_returning", "[convert][simd]")
{
    // Ensure the string-returning overload works correctly (uses SIMD path)
    auto const result = unicode::convert_to<char32_t>("Hello, World!"sv);
    CHECK(result == U"Hello, World!");
}

TEST_CASE("convert.utf8.incremental_decode", "[utf8]")
{
    auto constexpr values = string_view {
        "["
        "\xC3\xB6"         // ö  - german o-umlaut
        "\xE2\x82\xAC"     // €  - EURO sign U+20AC
        "\xF0\x9F\x98\x80" // 😀 - U+1F600
    };
    auto const* p = (char8_type const*) (values.data());
    auto decode = unicode::decoder<char> {};

    // single-byte
    auto result = decode(*p++);
    REQUIRE(result.has_value());
    REQUIRE(result.value() == '[');

    // double-byte
    result = decode(*p++);
    REQUIRE(!result.has_value());
    result = decode(*p++);
    REQUIRE(result.has_value());
    REQUIRE(result.value() == U'\u00F6'); // ö

    // 3 bytes
    result = decode(*p++);
    REQUIRE(!result.has_value());
    result = decode(*p++);
    REQUIRE(!result.has_value());
    result = decode(*p++);
    REQUIRE(result.has_value());
    REQUIRE(result.value() == U'\u20AC'); // €

    // 4 bytes
    result = decode(*p++);
    REQUIRE(!result.has_value());
    result = decode(*p++);
    REQUIRE(!result.has_value());
    result = decode(*p++);
    REQUIRE(!result.has_value());
    result = decode(*p++);
    REQUIRE(result.has_value());
    REQUIRE(result.value() == U'\U0001F600'); // 😀
}
