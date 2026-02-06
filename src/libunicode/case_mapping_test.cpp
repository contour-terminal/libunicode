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
#include <libunicode/case_mapping.h>

#include <catch2/catch_test_macros.hpp>

using namespace unicode;
using namespace std::string_view_literals;

TEST_CASE("case_mapping.simple_uppercase", "[case_mapping]")
{
    // ASCII
    CHECK(simple_uppercase('a') == 'A');
    CHECK(simple_uppercase('z') == 'Z');
    CHECK(simple_uppercase('A') == 'A');
    CHECK(simple_uppercase('0') == '0');

    // Latin Extended
    CHECK(simple_uppercase(U'\u00E0') == U'\u00C0'); // à -> À
    CHECK(simple_uppercase(U'\u00E9') == U'\u00C9'); // é -> É

    // Greek
    CHECK(simple_uppercase(U'\u03B1') == U'\u0391'); // α -> Α
    CHECK(simple_uppercase(U'\u03C9') == U'\u03A9'); // ω -> Ω
}

TEST_CASE("case_mapping.simple_lowercase", "[case_mapping]")
{
    // ASCII
    CHECK(simple_lowercase('A') == 'a');
    CHECK(simple_lowercase('Z') == 'z');
    CHECK(simple_lowercase('a') == 'a');
    CHECK(simple_lowercase('0') == '0');

    // Latin Extended
    CHECK(simple_lowercase(U'\u00C0') == U'\u00E0'); // À -> à
    CHECK(simple_lowercase(U'\u00C9') == U'\u00E9'); // É -> é

    // Greek
    CHECK(simple_lowercase(U'\u0391') == U'\u03B1'); // Α -> α
    CHECK(simple_lowercase(U'\u03A9') == U'\u03C9'); // Ω -> ω
}

TEST_CASE("case_mapping.simple_titlecase", "[case_mapping]")
{
    // ASCII (titlecase == uppercase)
    CHECK(simple_titlecase('a') == 'A');
    CHECK(simple_titlecase('z') == 'Z');

    // Titlecase letters (like Dz digraph)
    CHECK(simple_titlecase(U'\u01C4') == U'\u01C5'); // DŽ -> Dž
    CHECK(simple_titlecase(U'\u01C6') == U'\u01C5'); // dž -> Dž
}

TEST_CASE("case_mapping.simple_casefold", "[case_mapping]")
{
    // ASCII
    CHECK(simple_casefold('A') == 'a');
    CHECK(simple_casefold('Z') == 'z');
    CHECK(simple_casefold('a') == 'a');

    // Greek
    CHECK(simple_casefold(U'\u0391') == U'\u03B1'); // Α -> α
}

TEST_CASE("case_mapping.full_uppercase", "[case_mapping]")
{
    // German sharp s -> SS
    auto result = full_uppercase(U'\u00DF');
    CHECK(result.length == 2);
    CHECK(result.codepoints[0] == 'S');
    CHECK(result.codepoints[1] == 'S');

    // Regular character (identity or simple mapping)
    result = full_uppercase('a');
    CHECK((result.is_identity() || (result.length == 1 && result.codepoints[0] == 'A')));
}

TEST_CASE("case_mapping.full_casefold", "[case_mapping]")
{
    // German sharp s -> ss
    auto result = full_casefold(U'\u00DF');
    CHECK(result.length == 2);
    CHECK(result.codepoints[0] == 's');
    CHECK(result.codepoints[1] == 's');
}

TEST_CASE("case_mapping.to_uppercase_string", "[case_mapping]")
{
    CHECK(to_uppercase("hello"sv) == "HELLO");
    CHECK(to_uppercase("Hello World"sv) == "HELLO WORLD");
    CHECK(to_uppercase("123"sv) == "123");

    // German sharp s
    CHECK(to_uppercase("straße"sv) == "STRASSE");

    // UTF-8 with accents
    CHECK(to_uppercase("café"sv) == "CAFÉ");
}

TEST_CASE("case_mapping.to_lowercase_string", "[case_mapping]")
{
    CHECK(to_lowercase("HELLO"sv) == "hello");
    CHECK(to_lowercase("Hello World"sv) == "hello world");
    CHECK(to_lowercase("123"sv) == "123");

    // UTF-8 with accents
    CHECK(to_lowercase("CAFÉ"sv) == "café");
}

TEST_CASE("case_mapping.to_titlecase_string", "[case_mapping]")
{
    CHECK(to_titlecase("hello world"sv) == "Hello World");
    CHECK(to_titlecase("HELLO WORLD"sv) == "Hello World");
    CHECK(to_titlecase("hello-world"sv) == "Hello-World");
}

TEST_CASE("case_mapping.casefold_string", "[case_mapping]")
{
    CHECK(casefold("HELLO"sv) == "hello");
    CHECK(casefold("Hello"sv) == "hello");

    // German sharp s
    CHECK(casefold("STRASSE"sv) == "strasse");
    CHECK(casefold("straße"sv) == "strasse");
}

TEST_CASE("case_mapping.casefold_compare", "[case_mapping]")
{
    CHECK(casefold_compare("hello"sv, "HELLO"sv) == 0);
    CHECK(casefold_compare("Hello"sv, "hello"sv) == 0);
    CHECK(casefold_compare("abc"sv, "abd"sv) < 0);
    CHECK(casefold_compare("abd"sv, "abc"sv) > 0);

    // German sharp s
    CHECK(casefold_compare("straße"sv, "STRASSE"sv) == 0);
}

TEST_CASE("case_mapping.casefold_equals", "[case_mapping]")
{
    CHECK(casefold_equals("hello"sv, "HELLO"sv));
    CHECK(casefold_equals("Hello"sv, "hElLo"sv));
    CHECK_FALSE(casefold_equals("hello"sv, "world"sv));

    // German sharp s
    CHECK(casefold_equals("straße"sv, "STRASSE"sv));
}

TEST_CASE("case_mapping.is_cased", "[case_mapping]")
{
    CHECK(is_cased('A'));
    CHECK(is_cased('a'));
    CHECK(is_cased(U'\u00C0')); // À
    CHECK_FALSE(is_cased('0'));
    CHECK_FALSE(is_cased(' '));
    CHECK_FALSE(is_cased('!'));
}

TEST_CASE("case_mapping.changes_when_uppercased", "[case_mapping]")
{
    CHECK(changes_when_uppercased('a'));
    CHECK(changes_when_uppercased('z'));
    CHECK_FALSE(changes_when_uppercased('A'));
    CHECK_FALSE(changes_when_uppercased('0'));

    // German sharp s expands to SS
    CHECK(changes_when_uppercased(U'\u00DF'));
}

TEST_CASE("case_mapping.changes_when_lowercased", "[case_mapping]")
{
    CHECK(changes_when_lowercased('A'));
    CHECK(changes_when_lowercased('Z'));
    CHECK_FALSE(changes_when_lowercased('a'));
    CHECK_FALSE(changes_when_lowercased('0'));
}
