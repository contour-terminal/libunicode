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

using namespace unicode;

TEST_CASE("bidi_mirroring.is_mirrored", "[bidi_mirroring]")
{
    // Common mirrored characters
    CHECK(is_mirrored(U'('));
    CHECK(is_mirrored(U')'));
    CHECK(is_mirrored(U'<'));
    CHECK(is_mirrored(U'>'));
    CHECK(is_mirrored(U'['));
    CHECK(is_mirrored(U']'));
    CHECK(is_mirrored(U'{'));
    CHECK(is_mirrored(U'}'));

    // Non-mirrored characters
    CHECK_FALSE(is_mirrored(U'a'));
    CHECK_FALSE(is_mirrored(U'0'));
    CHECK_FALSE(is_mirrored(U' '));
    CHECK_FALSE(is_mirrored(U'A'));
    CHECK_FALSE(is_mirrored(U'!'));
}

TEST_CASE("bidi_mirroring.bidi_mirroring_glyph", "[bidi_mirroring]")
{
    // Parentheses
    CHECK(bidi_mirroring_glyph(U'(') == U')');
    CHECK(bidi_mirroring_glyph(U')') == U'(');

    // Angle brackets
    CHECK(bidi_mirroring_glyph(U'<') == U'>');
    CHECK(bidi_mirroring_glyph(U'>') == U'<');

    // Square brackets
    CHECK(bidi_mirroring_glyph(U'[') == U']');
    CHECK(bidi_mirroring_glyph(U']') == U'[');

    // Curly brackets
    CHECK(bidi_mirroring_glyph(U'{') == U'}');
    CHECK(bidi_mirroring_glyph(U'}') == U'{');

    // Math symbols: ELEMENT OF (U+2208) <-> CONTAINS AS MEMBER (U+220B)
    CHECK(bidi_mirroring_glyph(U'\u2208') == U'\u220B');
    CHECK(bidi_mirroring_glyph(U'\u220B') == U'\u2208');

    // Identity for unmapped codepoints
    CHECK(bidi_mirroring_glyph(U'a') == U'a');
    CHECK(bidi_mirroring_glyph(U'0') == U'0');
    CHECK(bidi_mirroring_glyph(U' ') == U' ');
}
