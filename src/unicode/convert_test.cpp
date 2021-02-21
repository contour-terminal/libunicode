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
#include <unicode/convert.h>
#include <unicode/utf8.h>
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <iterator>

using namespace unicode;
using namespace std::string_literals;
using namespace std;

TEST_CASE("convert.same", "[convert]")
{
    auto const s8 = "Hello, 😀"sv;
    auto t8 = string{};
    unicode::convert_to<char>(s8, back_insert_iterator(t8));
    CHECK(s8 == t8);

    auto const s16 = u"Hello, 😀"sv;
    auto t16 = u16string{};
    unicode::convert_to<char16_t>(s16, back_insert_iterator(t16));
    CHECK(s16 == t16);

    auto const s32 = U"Hello, 😀"sv;
    auto t32 = u32string{};
    unicode::convert_to<char32_t>(s32, back_insert_iterator(t32));
    CHECK(s32 == t32);
}

TEST_CASE("convert.8_to_16", "[convert]")
{
    auto constexpr input = string_view{
        "["
        "\xC3\xB6"          // ö  - german o-umlaut
        "\xE2\x82\xAC"      // €  - EURO sign U+20AC
        "\xF0\x9F\x98\x80"  // 😀 - U+1F600
    };
    u16string output;
    auto bi = back_inserter(output);
    unicode::convert_to<char16_t>(input, bi); // back_inserter(output));
    CHECK(output.size() == 5);
    CHECK(output == u"[ö€😀");
}

TEST_CASE("convert.8_to_32", "[convert]")
{
    auto constexpr input = string_view{
        "["
        "\xC3\xB6"          // ö  - german o-umlaut
        "\xE2\x82\xAC"      // €  - EURO sign U+20AC
        "\xF0\x9F\x98\x80"  // 😀 - U+1F600
    };
    u32string output;
    auto bi = back_inserter(output);
    unicode::convert_to<char32_t>(input, bi); // back_inserter(output));
    CHECK(output.size() == 4);
    CHECK(output == U"[ö€😀");
}

TEST_CASE("convert.utf8.incremental_decode", "[utf8]")
{
    auto constexpr values = string_view{
        "["
        "\xC3\xB6"          // ö  - german o-umlaut
        "\xE2\x82\xAC"      // €  - EURO sign U+20AC
        "\xF0\x9F\x98\x80"  // 😀 - U+1F600
    };
    char const* p = values.data();
    auto decode = unicode::decoder<char>{};

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
