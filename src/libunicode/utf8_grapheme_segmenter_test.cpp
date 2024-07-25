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
#include <libunicode/utf8_grapheme_segmenter.h>

#include <fmt/format.h>

#include <catch2/catch_test_macros.hpp>

#include <string_view>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{

std::string escape(std::string const& s)
{
    std::string t;
    for (char ch: s)
        if (ch >= 0x20 && ch <= 0x7E) // printable ASCII (We don't use isprint() due to MSVC)
            t += ch;
        else
            t += fmt::format("\\x{:02X}", ((unsigned) ch) & 0xFF);
    return t;
}

template <typename... Ts>
void test_utf8_grapheme_cluster_segmentation(Ts... expects)
{
    static_assert(sizeof...(expects) != 0);

    auto const text = (unicode::convert_to<char>(expects) + ...);

    auto const grapheme_segmenter = unicode::utf8_grapheme_segmenter(std::string_view(text));
    auto i = grapheme_segmenter.begin();
    auto const e = grapheme_segmenter.end();
    auto const s8 = [](auto const s) -> std::string {
        return escape(unicode::convert_to<char>(std::u32string_view(s)));
    };

    auto const checkOne = [&](std::u32string_view expected) -> void {
        INFO(fmt::format("expects: {}, actual {}", s8(expected), s8(*i)));
        REQUIRE(s8(*i) == s8(expected));
        REQUIRE(i != e);
        ++i;
    };

    (checkOne(expects), ...);

    REQUIRE(i == e);
    REQUIRE(*i == U""sv);

    ++i;
    REQUIRE(i == e);
    REQUIRE(*i == U""sv);
}

} // namespace

TEST_CASE("utf8_grapheme_segmenter.empty")
{
    auto const grapheme_segmenter = unicode::utf8_grapheme_segmenter(""sv);
    auto i = grapheme_segmenter.begin();
    auto const e = grapheme_segmenter.end();
    REQUIRE(i == e);
    REQUIRE(*i == U""sv);
    ++i;
    REQUIRE(i == e);
    REQUIRE(*i == U""sv);
}

TEST_CASE("utf8_grapheme_segmenter.mixed")
{
    test_utf8_grapheme_cluster_segmentation(U"Y"sv, U"e"sv, U"s"sv);
    test_utf8_grapheme_cluster_segmentation(U"├"sv, U"─"sv);
    test_utf8_grapheme_cluster_segmentation(U"├"sv, U"─"sv, U" "sv, U"Y"sv, U"e"sv, U"s"sv);
    test_utf8_grapheme_cluster_segmentation(U"X"sv, U"\U0001F926\U0001F3FC\u200D\u2642\uFE0F"sv, U"5"sv);
}
