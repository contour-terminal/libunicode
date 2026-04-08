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
#include <libunicode/ucd.h>

#include <catch2/catch_test_macros.hpp>

#include <algorithm>

using namespace unicode;

TEST_CASE("script_extensions.has_extensions", "[script_extensions]")
{
    // U+3001 IDEOGRAPHIC COMMA has script extensions (Bopo, Hang, Hani, Hira, Kana, Yiii)
    auto const exts = script_extensions(U'\u3001');
    REQUIRE(exts.has_value());
    CHECK(exts->size() > 1);
    CHECK(std::ranges::find(*exts, Script::Han) != exts->end());
}

TEST_CASE("script_extensions.no_extensions", "[script_extensions]")
{
    // U+0041 LATIN CAPITAL LETTER A has no script extensions
    auto const exts = script_extensions(U'A');
    CHECK_FALSE(exts.has_value());
}

TEST_CASE("script_extensions.stable_pointer", "[script_extensions]")
{
    // Returned spans should point to the same static data on repeated calls.
    auto const a = script_extensions(U'\u3001');
    auto const b = script_extensions(U'\u3001');
    REQUIRE(a.has_value());
    REQUIRE(b.has_value());
    CHECK(a->data() == b->data());
}
