/**
 * This file is part of the "libterminal" project
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
#include <unicode/script_segmenter.h>

#include <catch2/catch.hpp>

#include <string>
#include <string_view>

using namespace std::string_view_literals;
using namespace std::string_view_literals;
using std::optional;
using unicode::script_segmenter;

TEST_CASE("script_segmenter.private_use_area", "[script_segmenter]")
{
    auto constexpr str = U"\uE0B0"sv; // some PUA codepoint
    auto seg = unicode::script_segmenter { str.data(), str.size() };

    auto const r1 = seg.consume();
    REQUIRE(r1.has_value());
    auto const res1 = r1.value();
    CHECK(res1.size == 1);
    CHECK(res1.script == unicode::Script::Unknown);
}

TEST_CASE("script_segmenter.greek_kanji_greek", "[script_segmenter]")
{
    char32_t const* str = U"λ 合気道 λ;";
    auto seg = script_segmenter { str };

    // greek text
    optional<script_segmenter::result> const r1 = seg.consume();
    REQUIRE(r1.has_value());
    script_segmenter::result const res1 = r1.value();
    CHECK(res1.size == 2);
    CHECK(res1.script == unicode::Script::Greek);

    // japanese (Kanji-only)
    auto const r2 = seg.consume();
    REQUIRE(r2.has_value());
    auto const res2 = r2.value();
    CHECK(res2.size == 6);
    CHECK(res2.script == unicode::Script::Han);

    // greek
    auto const r3 = seg.consume();
    REQUIRE(r3.has_value());
    auto const res3 = r3.value();
    CHECK(res3.size == 8);
    CHECK(res3.script == unicode::Script::Greek);

    // end of stream
    auto const r4 = seg.consume();
    REQUIRE_FALSE(r4.has_value());
}

TEST_CASE("script_segmenter.latin_and_greek", "[script_segmenter]")
{
    auto constexpr str = U"AB λ;"sv;
    auto seg = unicode::script_segmenter { str.data(), str.size() };

    // latin text
    std::optional<unicode::script_segmenter::result> const r1 = seg.consume();
    REQUIRE(r1.has_value());
    auto const res1 = r1.value();
    CHECK(res1.size == 3);
    CHECK(res1.script == unicode::Script::Latin);

    // greek
    auto const r2 = seg.consume();
    REQUIRE(r2.has_value());
    auto const res2 = r2.value();
    CHECK(res2.size == 5);
    CHECK(res2.script == unicode::Script::Greek);

    // end of stream
    auto const r3 = seg.consume();
    REQUIRE_FALSE(r3.has_value());
}
