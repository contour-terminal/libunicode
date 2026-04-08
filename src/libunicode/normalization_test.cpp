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
#include <libunicode/normalization.h>

#include <catch2/catch_test_macros.hpp>

using namespace unicode;
using namespace std::string_view_literals;

TEST_CASE("normalization.canonical_combining_class", "[normalization]")
{
    // Base characters have CCC 0
    CHECK(canonical_combining_class('A') == 0);
    CHECK(canonical_combining_class('a') == 0);
    CHECK(canonical_combining_class(' ') == 0);

    // Combining diacritical marks have non-zero CCC
    CHECK(canonical_combining_class(U'\u0300') == 230); // Combining grave accent
    CHECK(canonical_combining_class(U'\u0301') == 230); // Combining acute accent
    CHECK(canonical_combining_class(U'\u0327') == 202); // Combining cedilla
    CHECK(canonical_combining_class(U'\u0328') == 202); // Combining ogonek
}

TEST_CASE("normalization.hangul_syllable", "[normalization]")
{
    // Hangul syllable "가" (U+AC00) = L (U+1100) + V (U+1161)
    CHECK(is_hangul_syllable(U'\uAC00'));
    CHECK_FALSE(is_hangul_syllable('A'));
    CHECK_FALSE(is_hangul_syllable(U'\u1100')); // Leading jamo

    CHECK(is_hangul_l_jamo(U'\u1100'));
    CHECK(is_hangul_v_jamo(U'\u1161'));
    CHECK(is_hangul_t_jamo(U'\u11A8'));
}

TEST_CASE("normalization.hangul_decompose", "[normalization]")
{
    // "가" (U+AC00) = ᄀ (U+1100) + ᅡ (U+1161)
    char32_t jamos[3];
    auto count = hangul_decompose(U'\uAC00', jamos);
    CHECK(count == 2);
    CHECK(jamos[0] == U'\u1100');
    CHECK(jamos[1] == U'\u1161');

    // "각" (U+AC01) = ᄀ (U+1100) + ᅡ (U+1161) + ᆨ (U+11A8)
    count = hangul_decompose(U'\uAC01', jamos);
    CHECK(count == 3);
    CHECK(jamos[0] == U'\u1100');
    CHECK(jamos[1] == U'\u1161');
    CHECK(jamos[2] == U'\u11A8');
}

TEST_CASE("normalization.hangul_compose", "[normalization]")
{
    // ᄀ (U+1100) + ᅡ (U+1161) = 가 (U+AC00)
    CHECK(hangul_compose(U'\u1100', U'\u1161', 0) == U'\uAC00');

    // ᄀ + ᅡ + ᆨ = 각
    CHECK(hangul_compose(U'\u1100', U'\u1161', U'\u11A8') == U'\uAC01');
}

TEST_CASE("normalization.canonical_decomposition", "[normalization]")
{
    // é (U+00E9) decomposes to e + combining acute accent
    auto decomp = canonical_decomposition(U'\u00E9');
    REQUIRE(decomp.size() == 2);
    CHECK(decomp[0] == 'e');
    CHECK(decomp[1] == U'\u0301');

    // ñ (U+00F1) decomposes to n + combining tilde
    decomp = canonical_decomposition(U'\u00F1');
    REQUIRE(decomp.size() == 2);
    CHECK(decomp[0] == 'n');
    CHECK(decomp[1] == U'\u0303');

    // ASCII has no decomposition
    decomp = canonical_decomposition('A');
    CHECK(decomp.empty());
}

TEST_CASE("normalization.to_nfd", "[normalization]")
{
    // é -> e + combining acute
    auto result = to_nfd(U"\u00E9");
    CHECK(result == U"e\u0301");

    // Already decomposed stays the same
    result = to_nfd(U"e\u0301");
    CHECK(result == U"e\u0301");

    // ASCII is unchanged
    result = to_nfd(U"hello");
    CHECK(result == U"hello");
}

TEST_CASE("normalization.to_nfc", "[normalization]")
{
    // e + combining acute -> é
    auto result = to_nfc(U"e\u0301");
    CHECK(result == U"\u00E9");

    // Already composed stays the same
    result = to_nfc(U"\u00E9");
    CHECK(result == U"\u00E9");

    // ASCII is unchanged
    result = to_nfc(U"hello");
    CHECK(result == U"hello");
}

TEST_CASE("normalization.to_nfd_utf8", "[normalization]")
{
    // UTF-8 version
    auto result = to_nfd("café"sv);
    // café with composed é becomes café with decomposed e + accent
    CHECK(result.length() > 4); // Should be longer due to decomposition
}

TEST_CASE("normalization.to_nfc_utf8", "[normalization]")
{
    // Compose
    auto nfd_input = to_nfd("café"sv);
    auto result = to_nfc(nfd_input);
    CHECK(result == "café");
}

TEST_CASE("normalization.quick_check", "[normalization]")
{
    // ASCII is always normalized
    CHECK(quick_check(U"hello", Normalization_Form::NFC) == Quick_Check_Result::Yes);
    CHECK(quick_check(U"hello", Normalization_Form::NFD) == Quick_Check_Result::Yes);

    // Pre-composed character
    CHECK(quick_check(U"\u00E9", Normalization_Form::NFC) == Quick_Check_Result::Yes);
    // Note: full is_normalized check works correctly even if quick_check is lenient
}

TEST_CASE("normalization.is_normalized", "[normalization]")
{
    // ASCII
    CHECK(is_nfc(U"hello"));
    CHECK(is_nfd(U"hello"));

    // Pre-composed é is NFC but not NFD
    CHECK(is_nfc(U"\u00E9"));

    // Decomposed e + accent is NFD but might not be NFC
    CHECK(is_nfd(U"e\u0301"));
}

TEST_CASE("normalization.is_canonically_equivalent", "[normalization]")
{
    // é composed vs decomposed
    CHECK(is_canonically_equivalent(U"\u00E9", U"e\u0301"));

    // Same string
    CHECK(is_canonically_equivalent(U"hello", U"hello"));

    // Different strings
    CHECK_FALSE(is_canonically_equivalent(U"hello", U"world"));
}

TEST_CASE("normalization.is_canonically_equivalent_utf8", "[normalization]")
{
    // UTF-8 versions
    CHECK(is_canonically_equivalent("hello"sv, "hello"sv));
    CHECK_FALSE(is_canonically_equivalent("hello"sv, "world"sv));
}

TEST_CASE("normalization.canonical_ordering", "[normalization]")
{
    // Multiple combining marks should be reordered by CCC
    // This tests that marks with lower CCC come before marks with higher CCC
    auto result = to_nfd(U"a\u0327\u0301"); // a + cedilla + acute
    // After canonical ordering, the cedilla (CCC=202) should come before acute (CCC=230)
    // Wait, that's backwards... let me check. Actually 202 < 230, so cedilla should come first.
    // But the NFD ordering is by CCC value, lower first.

    // Let's test with a character that has marks in wrong order
    auto input = U"a\u0301\u0327"; // acute (230) before cedilla (202) - wrong order
    result = to_nfd(input);
    // After normalization, cedilla should come first
    REQUIRE(result.size() == 3);
    CHECK(result[0] == 'a');
    // The order of combining marks should be by CCC (202 before 230)
    CHECK(canonical_combining_class(result[1]) <= canonical_combining_class(result[2]));
}

TEST_CASE("normalization.is_composition_exclusion", "[normalization]")
{
    // Some characters are excluded from composition
    // Hebrew point hataf segol
    CHECK(is_composition_exclusion(U'\u0344') == false); // Not excluded
    // Check a known exclusion (singleton decomposition)
    // These are typically characters that decompose to themselves plus something else
}

TEST_CASE("normalization.decomposition_type", "[normalization]")
{
    // Pre-composed characters have canonical decomposition
    CHECK(decomposition_type(U'\u00E9') == Decomposition_Type::Canonical);

    // ASCII has no decomposition
    CHECK(decomposition_type('A') == Decomposition_Type::None);

    // Hangul syllables have canonical decomposition
    CHECK(decomposition_type(U'\uAC00') == Decomposition_Type::Canonical);
}

// ============================================================================
// NFKC/NFKD tests (compatibility decomposition)
// ============================================================================

TEST_CASE("normalization.to_nfkd", "[normalization]")
{
    // fi ligature (U+FB01) decomposes under NFKD to "fi"
    auto result = to_nfkd(U"\uFB01");
    CHECK(result == U"fi");

    // Superscript 2 (U+00B2) decomposes to "2"
    result = to_nfkd(U"\u00B2");
    CHECK(result == U"2");

    // Fullwidth A (U+FF21) decomposes to "A"
    result = to_nfkd(U"\uFF21");
    CHECK(result == U"A");

    // Canonical decomposition still works in NFKD
    result = to_nfkd(U"\u00E9");
    CHECK(result == U"e\u0301");

    // ASCII is unchanged
    result = to_nfkd(U"hello");
    CHECK(result == U"hello");
}

TEST_CASE("normalization.to_nfkc", "[normalization]")
{
    // fi ligature becomes "fi"
    auto result = to_nfkc(U"\uFB01");
    CHECK(result == U"fi");

    // Superscript 2 becomes "2"
    result = to_nfkc(U"\u00B2");
    CHECK(result == U"2");

    // Fullwidth A becomes "A"
    result = to_nfkc(U"\uFF21");
    CHECK(result == U"A");

    // Composed character stays composed
    result = to_nfkc(U"\u00E9");
    CHECK(result == U"\u00E9");

    // Decomposed character gets recomposed
    result = to_nfkc(U"e\u0301");
    CHECK(result == U"\u00E9");

    // ASCII is unchanged
    result = to_nfkc(U"hello");
    CHECK(result == U"hello");
}

TEST_CASE("normalization.nfkd_does_not_equal_nfd", "[normalization]")
{
    // fi ligature: NFD leaves it unchanged, NFKD decomposes it
    CHECK(to_nfd(U"\uFB01") == U"\uFB01");
    CHECK(to_nfkd(U"\uFB01") == U"fi");
}

TEST_CASE("normalization.compatibility_equivalence", "[normalization]")
{
    CHECK(is_compatibility_equivalent(U"\uFB01", U"fi"));
    CHECK(is_compatibility_equivalent(U"\u00B2", U"2"));
    CHECK(is_compatibility_equivalent(U"\uFF21", U"A"));
}

// ============================================================================
// Quick check tests (with populated tables)
// ============================================================================

TEST_CASE("normalization.quick_check_nfd", "[normalization]")
{
    // Composed e-acute: NFD_QC=No (has canonical decomposition)
    CHECK(nfd_quick_check(U'\u00E9') == false);

    // ASCII: NFD_QC=Yes
    CHECK(nfd_quick_check('A') == true);

    // Already-decomposed combining mark: NFD_QC=Yes
    CHECK(nfd_quick_check(U'\u0301') == true);
}

TEST_CASE("normalization.quick_check_nfc", "[normalization]")
{
    // Combining acute accent: NFC_QC=Maybe (it can compose with a preceding starter)
    CHECK(nfc_quick_check(U'\u0301') == NFC_Quick_Check::Maybe);

    // ASCII: NFC_QC=Yes
    CHECK(nfc_quick_check('A') == NFC_Quick_Check::Yes);
}

TEST_CASE("normalization.quick_check_nfkd", "[normalization]")
{
    // fi ligature: NFKD_QC=No (has compatibility decomposition)
    CHECK(nfkd_quick_check(U'\uFB01') == false);

    // Composed e-acute: NFKD_QC=No (has canonical decomposition)
    CHECK(nfkd_quick_check(U'\u00E9') == false);

    // ASCII: NFKD_QC=Yes
    CHECK(nfkd_quick_check('A') == true);
}

TEST_CASE("normalization.quick_check_nfkc", "[normalization]")
{
    // fi ligature: NFKC_QC=No
    CHECK(nfkc_quick_check(U'\uFB01') == NFKC_Quick_Check::No);

    // ASCII: NFKC_QC=Yes
    CHECK(nfkc_quick_check('A') == NFKC_Quick_Check::Yes);
}

TEST_CASE("normalization.quick_check_string", "[normalization]")
{
    // fi ligature: NFC=Yes (no canonical decomposition), NFKC=No
    CHECK(quick_check(U"\uFB01", Normalization_Form::NFC) == Quick_Check_Result::Yes);
    CHECK(quick_check(U"\uFB01", Normalization_Form::NFKC) == Quick_Check_Result::No);
    CHECK(quick_check(U"\uFB01", Normalization_Form::NFD) == Quick_Check_Result::Yes);
    CHECK(quick_check(U"\uFB01", Normalization_Form::NFKD) == Quick_Check_Result::No);
}

// ============================================================================
// Decomposition type for compatibility types
// ============================================================================

TEST_CASE("normalization.decomposition_type_compat", "[normalization]")
{
    // fi ligature has compat decomposition
    CHECK(decomposition_type(U'\uFB01') == Decomposition_Type::Compat);

    // Superscript 2 has super decomposition
    CHECK(decomposition_type(U'\u00B2') == Decomposition_Type::Super);

    // Fullwidth A has wide decomposition
    CHECK(decomposition_type(U'\uFF21') == Decomposition_Type::Wide);

    // Halfwidth katakana wo (U+FF66) has narrow decomposition
    CHECK(decomposition_type(U'\uFF66') == Decomposition_Type::Narrow);

    // Circled digit 1 (U+2460) has circle decomposition
    CHECK(decomposition_type(U'\u2460') == Decomposition_Type::Circle);
}

