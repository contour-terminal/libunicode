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
#include <libunicode/grapheme_segmenter.h>

#include <catch2/catch_test_macros.hpp>

using namespace unicode;
using namespace std::string_literals;
using namespace std;

TEST_CASE("latin_common", "[grapheme_segmenter]")
{
    // Each ASCII character is its own grapheme cluster
    auto const text = u32string_view { U"ab!." };
    auto gs = grapheme_segmenter { text };
    CHECK(*gs == U"a");
    CHECK(gs.codepointsAvailable());
    ++gs;
    CHECK(*gs == U"b");
    CHECK(gs.codepointsAvailable());
    ++gs;
    CHECK(*gs == U"!");
    CHECK(gs.codepointsAvailable());
    ++gs;
    CHECK(*gs == U".");
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("combining character sequences", "[grapheme_segmenter]")
{
    // g + combining diaeresis = one cluster
    auto const text = u32string_view { U"\u0067\u0308" };
    auto gs = grapheme_segmenter { text };
    CHECK(*gs == text);
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("emoji.speaking-eye", "[grapheme_segmenter]")
{
    // 👁️‍🗨️ = Eye + VS16 + ZWJ + Left Speech Bubble + VS16
    auto const text = u32string_view { U"\U0001F441\uFE0F\u200D\U0001F5E8\uFE0F" };
    auto gs = grapheme_segmenter { text };
    CHECK(*gs == text);
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("emoji", "[grapheme_segmenter]")
{
    // 👨‍🦰 = Man + ZWJ + Red Hair
    auto const text1 = u32string_view { U"\U0001F468\u200D\U0001F9B0" };
    auto gs1 = grapheme_segmenter { text1 };
    CHECK(*gs1 == text1);
    CHECK_FALSE(gs1.codepointsAvailable());

    // 👨‍👩‍👧 = Man + ZWJ + Woman + ZWJ + Girl
    auto const text2 = u32string_view { U"\U0001F468\u200D\U0001F469\u200D\U0001F467" };
    auto gs2 = grapheme_segmenter { text2 };
    CHECK(*gs2 == text2);
    CHECK_FALSE(gs2.codepointsAvailable());
}

TEST_CASE("emoji: Man Facepalming: Medium-Light Skin Tone", "[grapheme_segmenter]")
{
    // 🤦🏼‍♂️ = Facepalm + Skin Tone + ZWJ + Male Sign + VS16
    auto const text = u32string_view { U"\U0001F926\U0001F3FC\u200D\u2642\uFE0F" };
    auto gs = grapheme_segmenter { text };
    CHECK(*gs == text);
    CHECK_FALSE(gs.codepointsAvailable());
}

// ---- GB11: Extended Pictographic + ZWJ sequence tests ----

TEST_CASE("grapheme_segmenter.gb11_extpic_zwj_extpic", "[grapheme_segmenter]")
{
    // ExtPic + ZWJ + ExtPic = one cluster (GB11 applies)
    // 🛑 + ZWJ + 🛑
    auto const text = u32string_view { U"\U0001F6D1\u200D\U0001F6D1" };
    auto gs = grapheme_segmenter { text };
    CHECK(*gs == text);
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb11_extpic_extend_zwj_extpic", "[grapheme_segmenter]")
{
    // ExtPic + Extend + ZWJ + ExtPic = one cluster (GB11 with Extend chain)
    // 🛑 + combining diaeresis + ZWJ + 🛑
    auto const text = u32string_view { U"\U0001F6D1\u0308\u200D\U0001F6D1" };
    auto gs = grapheme_segmenter { text };
    CHECK(*gs == text);
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb11_no_preceding_extpic", "[grapheme_segmenter]")
{
    // ZWJ + ExtPic without preceding ExtPic = two clusters (GB11 does NOT apply)
    // ZWJ + © (copyright sign, which is ExtPic)
    auto const text = u32string_view { U"\u200D\u00A9" };
    auto gs = grapheme_segmenter { text };
    CHECK(*gs == u32string_view { U"\u200D" });
    CHECK(gs.codepointsAvailable());
    ++gs;
    CHECK(*gs == u32string_view { U"\u00A9" });
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb11_non_extpic_zwj_extpic", "[grapheme_segmenter]")
{
    // 'a' + ZWJ + ExtPic = two clusters: [a ZWJ] [ExtPic]
    // GB9 keeps ZWJ with 'a', but GB11 does not fire (no preceding ExtPic)
    auto const text = u32string_view { U"\u0061\u200D\U0001F6D1" };
    auto gs = grapheme_segmenter { text };
    CHECK(*gs == u32string_view { U"\u0061\u200D" }); // 'a' + ZWJ
    CHECK(gs.codepointsAvailable());
    ++gs;
    CHECK(*gs == u32string_view { U"\U0001F6D1" }); // 🛑
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb11_ascii_resets_extpic_chain", "[grapheme_segmenter]")
{
    // ExtPic + ASCII + ZWJ + ExtPic = three clusters: [ExtPic] [a ZWJ] [ExtPic]
    // ASCII breaks the ExtPic chain, so the later ZWJ + ExtPic must not trigger GB11
    auto const text = u32string_view { U"\U0001F6D1\u0061\u200D\U0001F6D1" };
    auto gs = grapheme_segmenter { text };
    CHECK(*gs == u32string_view { U"\U0001F6D1" }); // 🛑
    CHECK(gs.codepointsAvailable());
    ++gs;
    CHECK(*gs == u32string_view { U"\u0061\u200D" }); // 'a' + ZWJ
    CHECK(gs.codepointsAvailable());
    ++gs;
    CHECK(*gs == u32string_view { U"\U0001F6D1" }); // 🛑
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.iterator_1", "[grapheme_segmenter]")
{
    auto const codepoints = u32string_view { U"\U0001F926\U0001F3FC\u200D\u2642\uFE0F" };
    auto gs = grapheme_segmenter { codepoints };

    // initially first token already process
    CHECK(*gs == codepoints);
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.iterator_2", "[grapheme_segmenter]")
{
    auto const grapheme_cluster2 = u32string_view { U"\U0001F926\U0001F3FC\u200D\u2642\uFE0F" };
    auto const codepoints = u32string_view { U"X\U0001F926\U0001F3FC\u200D\u2642\uFE0F5" };
    auto gs = grapheme_segmenter { codepoints };

    // first grapheme cluster
    CHECK(*gs == U"X");
    CHECK(gs.codepointsAvailable());

    // second grapheme cluster
    ++gs;
    CHECK(*gs == grapheme_cluster2);
    CHECK(gs.codepointsAvailable());

    // 3rd grapheme cluster
    ++gs;
    CHECK(*gs == U"5");
    CHECK_FALSE(gs.codepointsAvailable());

    // incrementing beyond end of stream
    ++gs;
    CHECK(*gs == U"");
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.iterator_3: regional flags", "[grapheme_segmenter]")
{
    auto const ri_DE = u32string { U"\U0001F1E9\U0001F1E9" };
    auto const ri_JP = u32string { U"\U0001F1EF\U0001F1F5" };
    auto const codepoints = ri_DE + ri_DE + ri_JP;
    auto gs = grapheme_segmenter { codepoints };

    // first grapheme cluster
    REQUIRE(*gs == ri_DE);
    REQUIRE(gs.codepointsAvailable());

    // second grapheme cluster
    ++gs;
    REQUIRE(*gs == ri_DE);
    REQUIRE(gs.codepointsAvailable());

    // 3rd grapheme cluster
    ++gs;
    REQUIRE(*gs == ri_JP);
    REQUIRE_FALSE(gs.codepointsAvailable());

    // incrementing beyond end of stream
    ++gs;
    REQUIRE(*gs == U"");
    REQUIRE_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.iterator_3: regional flags invalid 1", "[grapheme_segmenter]")
{
    auto const ri_DE = u32string { U"\U0001F1E9\U0001F1E9" };
    auto const ri_J = u32string { U"\U0001F1EF" };
    auto const codepoints = ri_DE + ri_DE + ri_J + U"P";
    auto gs = grapheme_segmenter { codepoints };

    // first grapheme cluster
    REQUIRE(*gs == ri_DE);
    REQUIRE(gs.codepointsAvailable());

    // second grapheme cluster
    ++gs;
    REQUIRE(*gs == ri_DE);
    REQUIRE(gs.codepointsAvailable());

    // 3rd grapheme cluster
    ++gs;
    REQUIRE(*gs == ri_J);
    REQUIRE(gs.codepointsAvailable());

    // 4th grapheme cluster
    ++gs;
    REQUIRE(*gs == U"P");
    REQUIRE_FALSE(gs.codepointsAvailable());

    // incrementing beyond end of stream
    ++gs;
    REQUIRE(*gs == U"");
    REQUIRE_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.iterator_3: regional flags invalid 2", "[grapheme_segmenter]")
{
    auto const ri_DE = u32string { U"\U0001F1E9\U0001F1E9" };
    auto const ri_J = u32string { U"\U0001F1EF" };
    auto const codepoints = ri_DE + ri_DE + U"Q" + ri_J + U"P";
    auto gs = grapheme_segmenter { codepoints };

    // 1
    REQUIRE(*gs == ri_DE);
    REQUIRE(gs.codepointsAvailable());

    // 2
    ++gs;
    REQUIRE(*gs == ri_DE);
    REQUIRE(gs.codepointsAvailable());

    // 3
    ++gs;
    REQUIRE(*gs == U"Q");
    REQUIRE(gs.codepointsAvailable());

    // 4
    ++gs;
    REQUIRE(*gs == ri_J);
    REQUIRE(gs.codepointsAvailable());

    // 5
    ++gs;
    REQUIRE(*gs == U"P");
    REQUIRE_FALSE(gs.codepointsAvailable());

    // incrementing beyond end of stream
    ++gs;
    REQUIRE(*gs == U"");
    REQUIRE_FALSE(gs.codepointsAvailable());
}

// ---- GB9c: Indic conjunct break rule tests ----

TEST_CASE("grapheme_segmenter.gb9c_basic_devanagari", "[grapheme_segmenter]")
{
    // KA + VIRAMA + TA should be one cluster
    auto const conjunct = u32string_view { U"\u0915\u094D\u0924" };
    auto gs = grapheme_segmenter { conjunct };
    CHECK(*gs == conjunct);
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb9c_chained_devanagari", "[grapheme_segmenter]")
{
    // KA + VIRAMA + TA + VIRAMA + YA should be one cluster
    auto const conjunct = u32string_view { U"\u0915\u094D\u0924\u094D\u092F" };
    auto gs = grapheme_segmenter { conjunct };
    CHECK(*gs == conjunct);
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb9c_with_extend", "[grapheme_segmenter]")
{
    // KA + NUKTA + ZWJ + VIRAMA + TA should be one cluster
    auto const conjunct = u32string_view { U"\u0915\u093C\u200D\u094D\u0924" };
    auto gs = grapheme_segmenter { conjunct };
    CHECK(*gs == conjunct);
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb9c_double_virama", "[grapheme_segmenter]")
{
    // KA + VIRAMA + VIRAMA + TA should be one cluster
    auto const conjunct = u32string_view { U"\u0915\u094D\u094D\u0924" };
    auto gs = grapheme_segmenter { conjunct };
    CHECK(*gs == conjunct);
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb9c_virama_zwj", "[grapheme_segmenter]")
{
    // KA + VIRAMA + ZWJ + TA should be one cluster
    auto const conjunct = u32string_view { U"\u0915\u094D\u200D\u0924" };
    auto gs = grapheme_segmenter { conjunct };
    CHECK(*gs == conjunct);
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb9c_negative_no_consonant_start", "[grapheme_segmenter]")
{
    // 'a' + VIRAMA + TA should be two clusters: [a, VIRAMA] and [TA]
    // ('a' is not InCB=Consonant, so GB9c does not apply)
    auto const seq = u32string_view { U"\u0061\u094D\u0924" };
    auto gs = grapheme_segmenter { seq };
    CHECK(*gs == u32string_view { U"\u0061\u094D" }); // 'a' + VIRAMA (GB9 keeps VIRAMA with 'a')
    CHECK(gs.codepointsAvailable());
    ++gs;
    CHECK(*gs == u32string_view { U"\u0924" }); // TA alone
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb9c_negative_consonant_without_linker", "[grapheme_segmenter]")
{
    // KA + TA should be two clusters (no virama/linker between them)
    auto const seq = u32string_view { U"\u0915\u0924" };
    auto gs = grapheme_segmenter { seq };
    CHECK(*gs == u32string_view { U"\u0915" });
    CHECK(gs.codepointsAvailable());
    ++gs;
    CHECK(*gs == u32string_view { U"\u0924" });
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb9c_negative_virama_latin", "[grapheme_segmenter]")
{
    // KA + VIRAMA + 'a': should be two clusters [KA, VIRAMA] and [a]
    // ('a' is not InCB=Consonant)
    auto const seq = u32string_view { U"\u0915\u094D\u0061" };
    auto gs = grapheme_segmenter { seq };
    CHECK(*gs == u32string_view { U"\u0915\u094D" }); // KA + VIRAMA
    CHECK(gs.codepointsAvailable());
    ++gs;
    CHECK(*gs == u32string_view { U"\u0061" }); // 'a'
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb9c_myanmar", "[grapheme_segmenter]")
{
    // MYANMAR LETTER MA + MYANMAR SIGN VIRAMA + MYANMAR LETTER BHA
    auto const conjunct = u32string_view { U"\u1019\u1039\u1018" };
    auto gs = grapheme_segmenter { conjunct };
    CHECK(*gs == conjunct);
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb9c_khmer", "[grapheme_segmenter]")
{
    // KHMER LETTER SA + KHMER SIGN COENG + KHMER LETTER TA + KHMER SIGN COENG + KHMER LETTER RO
    auto const conjunct = u32string_view { U"\u179F\u17D2\u178F\u17D2\u179A" };
    auto gs = grapheme_segmenter { conjunct };
    CHECK(*gs == conjunct);
    CHECK_FALSE(gs.codepointsAvailable());
}

TEST_CASE("grapheme_segmenter.gb9c_gujarati_with_shadda", "[grapheme_segmenter]")
{
    // GUJARATI LETTER SA + GUJARATI SIGN SHADDA + GUJARATI SIGN VIRAMA + GUJARATI LETTER SA + GUJARATI SIGN SHADDA
    auto const conjunct = u32string_view { U"\u0AB8\u0AFB\u0ACD\u0AB8\u0AFB" };
    auto gs = grapheme_segmenter { conjunct };
    CHECK(*gs == conjunct);
    CHECK_FALSE(gs.codepointsAvailable());
}

// TODO: Add data-driven test from official Unicode GraphemeBreakTest.txt once
// the multistage table generator correctly preserves all GCB property values.
// The tablegen correctly populates per-codepoint records, but the multistage
// table compression loses several GCB values (CR, LF, L, V, T, LV, etc.),
// causing them to fall back to GCB::Other at runtime.
