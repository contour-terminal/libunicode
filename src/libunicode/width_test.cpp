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
    // VS15 narrows only a base an emoji variation sequence is actually defined for. U+231A WATCH is
    // one, so its text presentation is a single column.
    CHECK(unicode::grapheme_cluster_width(U"\u231A\uFE0E"sv) == 1);

    // U+1F600 GRINNING FACE has NO variation sequence -- there is no text presentation of it to
    // request -- so a trailing VS15 changes nothing. wcwidth 0.8.2 measures this as two columns.
    CHECK(unicode::grapheme_cluster_width(U"\U0001F600\uFE0E"sv) == 2);
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

    // A Devanagari conjunct written with a ZWJ (ka + virama + ZWJ + ssa) is two columns, but the ZWJ
    // is not what widens it: the consonant following the virama is. See
    // grapheme_cluster_width.indic_spacing_mark_and_conjunct, which pins that rule directly.
    CHECK(unicode::grapheme_cluster_width(U"क्‍ष"sv) == 2);
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

    // Conjuncts across the Brahmic scripts, including the Invisible_Stacker ones.
    CHECK(unicode::grapheme_cluster_width(U"ព្រ"sv) == 2);  // Khmer, U+17D2
    CHECK(unicode::grapheme_cluster_width(U"ᬦ᭄ᬤ"sv) == 2); // Javanese, U+A9C0
    CHECK(unicode::grapheme_cluster_width(U"မ္မ"sv) == 2);  // Myanmar, U+1039
    CHECK(unicode::grapheme_cluster_width(U"മ്മ"sv) == 2);  // Malayalam, U+0D4D

    // The virama set is ALL viramas rather than Indic_Conjunct_Break=Linker. Linker is not
    // script-limited -- it already covers Khmer, Myanmar, Javanese, Chakma and Tai Tham above -- but
    // it is smaller: 20 codepoints against 41. These four conjuncts are exactly what the larger set
    // buys, and each would collapse to one column if the implementation keyed off Linker.
    CHECK(unicode::grapheme_cluster_width(U"ਕ੍ਨ"sv) == 2); // Gurmukhi, U+0A4D
    CHECK(unicode::grapheme_cluster_width(U"க்ந"sv) == 2); // Tamil, U+0BCD
    CHECK(unicode::grapheme_cluster_width(U"ಕ್ನ"sv) == 2); // Kannada, U+0CCD
    CHECK(unicode::grapheme_cluster_width(U"ක්න"sv) == 2); // Sinhala, U+0DCA
}

TEST_CASE("grapheme_cluster_width.variation_selector_needs_a_variation_base", "[width]")
{
    // A variation selector is a presentation request, not a width modifier: it applies only where an
    // emoji variation sequence is defined. Every expectation here was read off wcwidth 0.8.2.

    // VS16 widens U+2714 HEAVY CHECK MARK, for which a variation sequence exists...
    CHECK(unicode::grapheme_cluster_width(U"✔️"sv) == 2);
    // ...but not U+2713 CHECK MARK, for which none does, and not ordinary text.
    CHECK(unicode::grapheme_cluster_width(U"✓️"sv) == 1);
    CHECK(unicode::grapheme_cluster_width(U"x️"sv) == 1); // 'x'
    CHECK(unicode::grapheme_cluster_width(U"•️"sv) == 1); // bullet
    CHECK(unicode::grapheme_cluster_width(U"—️"sv) == 1); // em dash

    // VS15 must not narrow a wide character that is not an emoji at all.
    CHECK(unicode::grapheme_cluster_width(U"漢︎"sv) == 2); // CJK ideograph
    CHECK(unicode::grapheme_cluster_width(U"ᄀ︎"sv) == 2); // HANGUL CHOSEONG KIYEOK

    // Nor may it undo the widening that a spacing mark or a conjunct performed.
    CHECK(unicode::grapheme_cluster_width(U"का︎"sv) == 2); // ka + AA matra + VS15
    CHECK(unicode::grapheme_cluster_width(U"क्न︎"sv) == 2); // ka + virama + na + VS15
}

TEST_CASE("grapheme_cluster_width.variation_selector_after_zwj", "[width]")
{
    // A ZWJ consumes the codepoint behind it but leaves the base it swallowed still open to a
    // variation selector. wcwidth resolves VS16 purely by looking the base up in its narrow-to-wide
    // table, so the selector still widens even though the ZWJ zeroed the recorded width.
    // Regression: gating VS16 on that zeroed width made the selector's position change the answer,
    // so U+2764 U+200D U+1F525 U+FE0F measured 1 while the RGI spelling measured 2.
    CHECK(unicode::grapheme_cluster_width(U"❤‍\U0001F525️"sv) == 2); // heart ZWJ fire VS16
    CHECK(unicode::grapheme_cluster_width(U"❤️‍\U0001F525"sv) == 2); // ...RGI spelling
    CHECK(unicode::grapheme_cluster_width(U"✔‍\U0001F525️"sv) == 2); // U+2714 is a base

    // ...but only where a variation sequence is actually defined: U+2713 has none.
    CHECK(unicode::grapheme_cluster_width(U"✓‍\U0001F525️"sv) == 1);

    // VS15 is deliberately NOT symmetric with VS16 here. wcwidth's VS15 branch additionally requires
    // the *recorded* width to be 2, and the ZWJ zeroed it, so the watch is not narrowed.
    CHECK(unicode::grapheme_cluster_width(U"⌚‍\U0001F525︎"sv) == 2); // watch ZWJ fire VS15
}

TEST_CASE("grapheme_cluster_width.repeated_vs15_underflows_like_wcwidth", "[width]")
{
    // wcwidth's VS15 branch settles its correction against the running total and -- unlike its VS16
    // branch -- does NOT mark the base as consumed, so a repeated VS15 decrements a second time.
    // wcswidth("⌚︎︎") is 0 and a third selector takes it to -1.
    //
    // This asymmetry looks like an oversight and has been "fixed" once already. It is not: this
    // library is a port of wcwidth 0.8, degenerate input included. Only the clamp at zero is ours,
    // because the return type is unsigned.
    CHECK(unicode::grapheme_cluster_width(U"⌚︎"sv) == 1);
    CHECK(unicode::grapheme_cluster_width(U"⌚︎︎"sv) == 0);
    CHECK(unicode::grapheme_cluster_width(U"⌚︎︎︎"sv) == 0);

    // VS16, by contrast, does mark the base consumed, so repeating it is idempotent.
    CHECK(unicode::grapheme_cluster_width(U"©️"sv) == 2);
    CHECK(unicode::grapheme_cluster_width(U"©️️"sv) == 2);
}

TEST_CASE("grapheme_cluster_width.regional_indicator_run_is_contiguous", "[width]")
{
    // A flag is a PAIR of regional indicators, and wcwidth decides whether an RI is the second of a
    // pair by counting the RIs *immediately* before it. Anything else -- a variation selector, a ZWJ
    // -- ends the run, so the RI that follows opens a new pair and is measured in full.
    //
    // Regression: the pair counter was only cleared on the path that measures a codepoint, and the
    // ZWJ/VS15/VS16 branches return before reaching it. An RI after a selector was then mistaken for
    // the second half of a pair and contributed nothing.
    CHECK(unicode::grapheme_cluster_width(U"\U0001F1EA️\U0001F1EA"sv) == 4);
    CHECK(unicode::grapheme_cluster_width(U"\U0001F1EA︎\U0001F1EA"sv) == 4);
    CHECK(unicode::grapheme_cluster_width(U"\U0001F1EA️\U0001F1EA️\U0001F1EA"sv) == 6);

    // An unbroken run still pairs up the way a flag must.
    CHECK(unicode::grapheme_cluster_width(U"\U0001F1E9\U0001F1EA"sv) == 2);
    CHECK(unicode::grapheme_cluster_width(U"\U0001F1E9\U0001F1EA\U0001F1E9"sv) == 4);

    // A ZWJ ends the run too, but it also swallows the RI behind it, so that one is never measured.
    // The run length still counts it: wcwidth scans the raw input backwards, not what it processed.
    CHECK(unicode::grapheme_cluster_width(U"\U0001F1EA‍\U0001F1EA"sv) == 2);
    CHECK(unicode::grapheme_cluster_width(U"\U0001F1EA‍\U0001F1EA\U0001F1EA"sv) == 2);
}

TEST_CASE("grapheme_cluster_width.skin_tone_modifier_is_absorbed", "[width]")
{
    // A skin tone modifier is drawn into the emoji before it, so it takes no column of its own. That
    // holds even when the base is not Emoji_Modifier_Base: such sequences are ill-formed but real,
    // and a terminal that counts the modifier separately corrupts the rest of the line.
    CHECK(unicode::grapheme_cluster_width(U"❤\U0001F3FB"sv) == 1); // heart + light skin tone
    CHECK(unicode::grapheme_cluster_width(U"©\U0001F3FB"sv) == 1); // copyright + light skin tone
    CHECK(unicode::grapheme_cluster_width(U"\U0001F3F3\U0001F3FB‍\U0001F525"sv) == 1);

    // The well-formed case keeps working.
    CHECK(unicode::grapheme_cluster_width(U"\U0001F926\U0001F3FC"sv) == 2);
}

TEST_CASE("width.format_characters", "[width]")
{
    // General_Category=Cf is zero-width as a rule, but the ones that are Grapheme_Cluster_Break=
    // Prepend do render, and wcwidth measures them as one column.
    CHECK(unicode::width(U'؀') == 1);         // ARABIC NUMBER SIGN
    CHECK(unicode::width(U'۝') == 1);         // ARABIC END OF AYAH
    CHECK(unicode::width(U'܏') == 1);         // SYRIAC ABBREVIATION MARK
    CHECK(unicode::width(U'\U000110BD') == 1); // KAITHI NUMBER SIGN

    // SOFT HYPHEN is Cf but not Prepend; a terminal has no line breaking to hide it with and draws
    // it as a hyphen, so it is carved out explicitly. wcwidth agrees.
    CHECK(unicode::width(U'­') == 1);
    CHECK(unicode::grapheme_cluster_width(U"a­b"sv) == 3);

    // The invisible Cf codepoints stay zero-width. These four in particular are NOT
    // Default_Ignorable, so a rule keyed off that property wrongly gives them a column.
    CHECK(unicode::width(U'￹') == 0);        // INTERLINEAR ANNOTATION ANCHOR
    CHECK(unicode::width(U'￺') == 0);        // INTERLINEAR ANNOTATION SEPARATOR
    CHECK(unicode::width(U'￻') == 0);        // INTERLINEAR ANNOTATION TERMINATOR
    CHECK(unicode::width(U'\U00013430') == 0); // EGYPTIAN HIEROGLYPH VERTICAL JOINER

    CHECK(unicode::width(U'‍') == 0); // ZWJ
    CHECK(unicode::width(U'​') == 0); // ZERO WIDTH SPACE
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
