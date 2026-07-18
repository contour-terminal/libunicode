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

#include <array>
#include <cstdint>
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

TEST_CASE("width.hangul_jamo", "[width]")
{
    // Leading Jamo (Choseong) — EAW=Wide → width 2
    CHECK(unicode::width(U'\u1100') == 2); // HANGUL CHOSEONG KIYEOK
    CHECK(unicode::width(U'\u115F') == 2); // HANGUL CHOSEONG FILLER
    CHECK(unicode::width(U'\uA960') == 2); // HANGUL CHOSEONG TIKEUT-MIEUM (Extended-A)
    CHECK(unicode::width(U'\uA97C') == 2); // HANGUL CHOSEONG SSANGYEORINHIEUH (Extended-A)

    // Vowel Jamo (Jungseong) — conjoining → width 0
    CHECK(unicode::width(U'\u1160') == 0); // HANGUL JUNGSEONG FILLER
    CHECK(unicode::width(U'\u1161') == 0); // HANGUL JUNGSEONG A
    CHECK(unicode::width(U'\u11A7') == 0); // HANGUL JUNGSEONG O-YAE
    CHECK(unicode::width(U'\uD7B0') == 0); // HANGUL JUNGSEONG O-YEO (Extended-B)
    CHECK(unicode::width(U'\uD7C6') == 0); // HANGUL JUNGSEONG ARAEA-E (Extended-B)

    // Trailing Jamo (Jongseong) — conjoining → width 0
    CHECK(unicode::width(U'\u11A8') == 0); // HANGUL JONGSEONG KIYEOK
    CHECK(unicode::width(U'\u11FF') == 0); // HANGUL JONGSEONG SSANGNIEUN
    CHECK(unicode::width(U'\uD7CB') == 0); // HANGUL JONGSEONG NIEUN-RIEUL (Extended-B)
    CHECK(unicode::width(U'\uD7FB') == 0); // HANGUL JONGSEONG PHIEUPH-THIEUTH (Extended-B)

    // Precomposed syllables — EAW=Wide → width 2
    CHECK(unicode::width(U'\uAC00') == 2); // HANGUL SYLLABLE GA (first)
    CHECK(unicode::width(U'\uD7A3') == 2); // HANGUL SYLLABLE HIH (last)

    // Compatibility Jamo — EAW=Wide → width 2
    CHECK(unicode::width(U'\u3131') == 2); // HANGUL LETTER KIYEOK

    // Halfwidth Jamo — no conjoining → width 1
    CHECK(unicode::width(U'\uFFA0') == 1); // HALFWIDTH HANGUL FILLER
    CHECK(unicode::width(U'\uFFBE') == 1); // HALFWIDTH HANGUL LETTER KIYEOK
}

TEST_CASE("width.hangul_decomposed_syllable", "[width]")
{
    // L + V = grapheme cluster of width 2
    CHECK(unicode::grapheme_cluster_width(U"\u1100\u1161"sv) == 2);

    // L + V + T = grapheme cluster of width 2
    CHECK(unicode::grapheme_cluster_width(U"\u1100\u1161\u11A8"sv) == 2);
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

TEST_CASE("grapheme_cluster_width.ZWJ_does_not_widen", "[width]")
{
    // A ZWJ sequence is measured by its FIRST segment, not by its widest member. Python wcwidth's
    // wcswidth() consumes the ZWJ together with the codepoint that follows it, so the trailing
    // pictograph contributes nothing. Measured against wcwidth 0.8.2, these are one column.
    CHECK(unicode::width(U'❤') == 1);

    // ❤‍🔥 heart on fire and ❤‍🩹 mending heart, both written WITHOUT VS16.
    CHECK(unicode::grapheme_cluster_width(U"❤‍\U0001F525"sv) == 1);
    CHECK(unicode::grapheme_cluster_width(U"❤‍\U0001FA79"sv) == 1);

    // The RGI forms carry VS16, which is what actually makes them two columns.
    CHECK(unicode::grapheme_cluster_width(U"❤️‍\U0001F525"sv) == 2);

    // ZWJ likewise does not widen a non-emoji ligature request: Arabic lam + ZWJ + alef.
    CHECK(unicode::grapheme_cluster_width(U"ل‍ا"sv) == 1);

    // NOTE: a Devanagari conjunct (ka + virama + ZWJ + ssa) is deliberately NOT asserted here. It is
    // two columns in wcwidth -- not because of the ZWJ, but because a consonant following a virama
    // promotes its cluster. That rule is not implemented yet, so pinning either answer here would
    // encode a belief rather than the oracle.
}

TEST_CASE("grapheme_cluster_width.indic_spacing_mark_and_conjunct", "[width]")
{
    // A Devanagari cluster is one column only while nothing beside the base takes room of its own.
    CHECK(unicode::grapheme_cluster_width(U"क"sv) == 1); // ka
    CHECK(unicode::grapheme_cluster_width(U"कं"sv) == 1); // ka + anusvara (non-spacing)
    CHECK(unicode::grapheme_cluster_width(U"क्"sv) == 1); // ka + dangling virama

    // A spacing mark sits beside its base rather than over it.
    CHECK(unicode::grapheme_cluster_width(U"का"sv) == 2); // ka + AA matra (Mc)

    // A virama joins two consonants into a conjunct, which is wider than one cell.
    CHECK(unicode::grapheme_cluster_width(U"क्न"sv) == 2);           // ka + virama + na
    CHECK(unicode::grapheme_cluster_width(U"क्नि"sv) == 2);          // ...+ i matra
    CHECK(unicode::grapheme_cluster_width(U"क्‍ष"sv) == 2); // ka + virama + ZWJ + ssa

    // The virama set must be ALL viramas, not Indic_Conjunct_Break=Linker, which covers six scripts.
    // Each of these is a conjunct whose virama is outside that set.
    CHECK(unicode::grapheme_cluster_width(U"ព្រ"sv) == 2);  // Khmer, U+17D2
    CHECK(unicode::grapheme_cluster_width(U"ᬦ᭄ᬤ"sv) == 2); // Javanese, U+A9C0
    CHECK(unicode::grapheme_cluster_width(U"မ္မ"sv) == 2);  // Myanmar, U+1039
    CHECK(unicode::grapheme_cluster_width(U"മ്മ"sv) == 2);  // Malayalam, U+0D4D
}

TEST_CASE("grapheme_cluster_width.two_spacing_codepoints_accumulate", "[width]")
{
    // There is no clamp at 2: a cluster holding two codepoints that each take room accumulates.
    // Lao ko + sign AM are both spacing, and wcwidth measures the pair as two columns.
    CHECK(unicode::grapheme_cluster_width(U"ກຳ"sv) == 2);
}

TEST_CASE("grapheme_cluster_width.batch_matches_wcwidth_corpus", "[width]")
{
    // Every expectation below was read off Python wcwidth 0.8.2's wcswidth(), not reasoned about.
    // Reasoning about how a cluster "looks" produced two wrong rules before this table existed.
    struct Case
    {
        std::u32string_view cluster;
        unsigned expected;
        std::string_view what;
    };
    auto const cases = std::array {
        Case { U"A"sv, 1, "plain ASCII" },
        Case { U"\U0001F600"sv, 2, "grinning face" },
        Case { U"\u261D\uFE0F"sv, 2, "index pointing up + VS16" },
        Case { U"\u231A\uFE0E"sv, 1, "watch + VS15" },
        Case { U"\u2764\u200D\U0001F525"sv, 1, "heart on fire, no VS16 -- the ZWJ eats the fire" },
        Case { U"\U0001F468\u200D\U0001F469\u200D\U0001F467"sv, 2, "family" },
        Case { U"\U0001F926\U0001F3FC\u200D\u2642\uFE0F"sv, 2, "man facepalming, skin tone" },
        Case { U"g\u0308"sv, 1, "g + combining diaeresis" },
        Case { U"2\uFE0F\u20E3"sv, 2, "keycap 2" },
        Case { U"\U0001F1E9\U0001F1EA"sv, 2, "flag" },
        Case { U"\u1100\u1161\u11A8"sv, 2, "decomposed Hangul syllable" },
    };

    for (auto const& testCase: cases)
    {
        INFO(testCase.what);
        CHECK(unicode::grapheme_cluster_width(testCase.cluster) == testCase.expected);
    }
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
