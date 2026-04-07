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
#include <libunicode/width.h>

#include <catch2/catch_test_macros.hpp>

#include <string_view>

using namespace std::string_view_literals;

TEST_CASE("width.single_codepoint", "[width]")
{
    // C0
    CHECK(unicode::width(0x07) == 0);

    // US-ASCII
    for (char32_t i = 0x20; i <= 0x7E; ++i)
        CHECK(unicode::width(i) == 1);

    CHECK(unicode::width(U'\u00A9') == 1); // Copyright symbol

    CHECK(unicode::width(U'\u200D') == 0); // ZWJ
    CHECK(unicode::width(U'\uFE0E') == 0); // emoji modifier
    CHECK(unicode::width(U'\uFE0F') == 0); // emoji modifier

    // emoji
    CHECK(unicode::width(U'\U0001F60A') == 2); // 😊 :blush:
    CHECK(unicode::width(U'\U0001F480') == 2); // 💀 :skull:
}

TEST_CASE("grapheme_cluster_width.empty", "[width]")
{
    CHECK(unicode::grapheme_cluster_width(std::u32string_view {}) == 0);
    CHECK(unicode::grapheme_cluster_width(std::string_view {}) == 0);
}

TEST_CASE("grapheme_cluster_width.single_codepoint", "[width]")
{
    CHECK(unicode::grapheme_cluster_width(U"A"sv) == 1);
    CHECK(unicode::grapheme_cluster_width(U"\U0001F600"sv) == 2); // 😀 grinning face
    CHECK(unicode::grapheme_cluster_width(U"\u200D"sv) == 0);     // ZWJ
}

TEST_CASE("grapheme_cluster_width.VS16_emoji_presentation", "[width]")
{
    // The issue's example: ☝ (U+261D) is width 1, but ☝️ (U+261D U+FE0F) should be width 2
    CHECK(unicode::grapheme_cluster_width(U"\u261D\uFE0F"sv) == 2);

    // Copyright sign + VS16 → emoji presentation
    CHECK(unicode::grapheme_cluster_width(U"\u00A9\uFE0F"sv) == 2);

    // Heavy heart + VS16
    CHECK(unicode::grapheme_cluster_width(U"\u2764\uFE0F"sv) == 2);
}

TEST_CASE("grapheme_cluster_width.VS15_text_presentation", "[width]")
{
    // Grinning face + VS15 → text presentation
    CHECK(unicode::grapheme_cluster_width(U"\U0001F600\uFE0E"sv) == 1);
}

TEST_CASE("grapheme_cluster_width.ZWJ_sequence", "[width]")
{
    // Man + ZWJ + red hair
    CHECK(unicode::grapheme_cluster_width(U"\U0001F468\u200D\U0001F9B0"sv) == 2);

    // Family: man + ZWJ + woman + ZWJ + girl
    CHECK(unicode::grapheme_cluster_width(U"\U0001F468\u200D\U0001F469\u200D\U0001F467"sv) == 2);
}

TEST_CASE("grapheme_cluster_width.emoji_modifier", "[width]")
{
    // Person facepalming + medium-light skin tone
    CHECK(unicode::grapheme_cluster_width(U"\U0001F926\U0001F3FC"sv) == 2);

    // Man facepalming, medium-light skin tone (full ZWJ sequence with VS16)
    CHECK(unicode::grapheme_cluster_width(U"\U0001F926\U0001F3FC\u200D\u2642\uFE0F"sv) == 2);
}

TEST_CASE("grapheme_cluster_width.regional_indicators", "[width]")
{
    // Flag: Germany (DE)
    CHECK(unicode::grapheme_cluster_width(U"\U0001F1E9\U0001F1EA"sv) == 2);
}

TEST_CASE("grapheme_cluster_width.combining_marks", "[width]")
{
    // g + combining diaeresis → single cluster, width 1
    CHECK(unicode::grapheme_cluster_width(U"g\u0308"sv) == 1);
}

TEST_CASE("grapheme_cluster_width.keycap_sequence", "[width]")
{
    // 2 + VS16 + combining enclosing keycap → keycap "2"
    CHECK(unicode::grapheme_cluster_width(U"2\uFE0F\u20E3"sv) == 2);
}

TEST_CASE("grapheme_cluster_width.utf8_string", "[width]")
{
    CHECK(unicode::grapheme_cluster_width("Hello"sv) == 5);

    // CJK: U+4E2D (中) and U+6587 (文) are each width 2
    CHECK(unicode::grapheme_cluster_width("a\xe4\xb8\xad\xe6\x96\x87"sv) == 5); // a + 中 + 文

    // Mixed: ASCII + emoji with VS16
    // "A☝️" = 'A' (width 1) + U+261D U+FE0F (width 2) = 3
    CHECK(unicode::grapheme_cluster_width("A\xe2\x98\x9d\xef\xb8\x8f"sv) == 3);
}
