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
