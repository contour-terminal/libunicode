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
#pragma once

#include <libunicode/ucd_enums.h>

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace unicode
{

/// Maximum number of codepoints a single codepoint can expand to during case mapping.
/// (e.g., German ß -> SS, or ligature ﬃ -> FFI)
inline constexpr size_t MaxCaseMappingLength = 3;

/// Result of a case mapping operation for a single codepoint.
struct case_mapping_result
{
    std::array<char32_t, MaxCaseMappingLength> codepoints {};
    uint8_t length = 0;

    /// Returns a view of the mapped codepoints.
    [[nodiscard]] constexpr std::u32string_view view() const noexcept { return std::u32string_view(codepoints.data(), length); }

    /// Returns true if this is an identity mapping (no change).
    [[nodiscard]] constexpr bool is_identity() const noexcept { return length == 0; }
};

// ============================================================================
// Simple case mappings (single codepoint -> single codepoint)
// These are used when string length must be preserved.
// ============================================================================

/// Returns the simple uppercase mapping for a codepoint.
/// If no mapping exists, returns the input codepoint.
[[nodiscard]] char32_t simple_uppercase(char32_t codepoint) noexcept;

/// Returns the simple lowercase mapping for a codepoint.
/// If no mapping exists, returns the input codepoint.
[[nodiscard]] char32_t simple_lowercase(char32_t codepoint) noexcept;

/// Returns the simple titlecase mapping for a codepoint.
/// If no mapping exists, returns the input codepoint.
[[nodiscard]] char32_t simple_titlecase(char32_t codepoint) noexcept;

/// Returns the simple case folding for a codepoint.
/// If no folding exists, returns the input codepoint.
[[nodiscard]] char32_t simple_casefold(char32_t codepoint) noexcept;

// ============================================================================
// Full case mappings (single codepoint -> possibly multiple codepoints)
// These are preferred for proper Unicode case conversion.
// ============================================================================

/// Returns the full uppercase mapping for a codepoint.
/// May return 1-3 codepoints (e.g., ß -> SS).
/// Returns length=0 if the codepoint maps to itself.
[[nodiscard]] case_mapping_result full_uppercase(char32_t codepoint) noexcept;

/// Returns the full lowercase mapping for a codepoint.
/// May return 1-3 codepoints (e.g., İ -> i + combining dot above).
/// Returns length=0 if the codepoint maps to itself.
[[nodiscard]] case_mapping_result full_lowercase(char32_t codepoint) noexcept;

/// Returns the full titlecase mapping for a codepoint.
/// May return 1-3 codepoints.
/// Returns length=0 if the codepoint maps to itself.
[[nodiscard]] case_mapping_result full_titlecase(char32_t codepoint) noexcept;

/// Returns the full case folding for a codepoint.
/// May return 1-3 codepoints (e.g., ß -> ss).
/// Returns length=0 if the codepoint folds to itself.
[[nodiscard]] case_mapping_result full_casefold(char32_t codepoint) noexcept;

// ============================================================================
// String-level case operations
// ============================================================================

/// Converts a UTF-32 string to uppercase.
[[nodiscard]] std::u32string to_uppercase(std::u32string_view text);

/// Converts a UTF-32 string to lowercase.
[[nodiscard]] std::u32string to_lowercase(std::u32string_view text);

/// Converts a UTF-32 string to titlecase.
/// Titlecase capitalizes the first letter of each word.
[[nodiscard]] std::u32string to_titlecase(std::u32string_view text);

/// Case-folds a UTF-32 string (for case-insensitive comparison).
[[nodiscard]] std::u32string casefold(std::u32string_view text);

/// Converts a UTF-8 string to uppercase.
[[nodiscard]] std::string to_uppercase(std::string_view text);

/// Converts a UTF-8 string to lowercase.
[[nodiscard]] std::string to_lowercase(std::string_view text);

/// Converts a UTF-8 string to titlecase.
[[nodiscard]] std::string to_titlecase(std::string_view text);

/// Case-folds a UTF-8 string.
[[nodiscard]] std::string casefold(std::string_view text);

// ============================================================================
// Case-insensitive comparison
// ============================================================================

/// Compares two UTF-32 strings case-insensitively using case folding.
/// @return negative if a < b, 0 if equal, positive if a > b
[[nodiscard]] int casefold_compare(std::u32string_view a, std::u32string_view b);

/// Compares two UTF-8 strings case-insensitively using case folding.
[[nodiscard]] int casefold_compare(std::string_view a, std::string_view b);

/// Checks if two strings are equal when case-folded.
[[nodiscard]] bool casefold_equals(std::u32string_view a, std::u32string_view b);

/// Checks if two UTF-8 strings are equal when case-folded.
[[nodiscard]] bool casefold_equals(std::string_view a, std::string_view b);

// ============================================================================
// Case property queries
// ============================================================================

/// Returns true if the codepoint is considered cased (Uppercase, Lowercase, or Titlecase letter).
[[nodiscard]] bool is_cased(char32_t codepoint) noexcept;

/// Returns true if the codepoint is case-ignorable.
[[nodiscard]] bool is_case_ignorable(char32_t codepoint) noexcept;

/// Returns true if the codepoint changes when uppercased.
[[nodiscard]] bool changes_when_uppercased(char32_t codepoint) noexcept;

/// Returns true if the codepoint changes when lowercased.
[[nodiscard]] bool changes_when_lowercased(char32_t codepoint) noexcept;

/// Returns true if the codepoint changes when titlecased.
[[nodiscard]] bool changes_when_titlecased(char32_t codepoint) noexcept;

/// Returns true if the codepoint changes when case-folded.
[[nodiscard]] bool changes_when_casefolded(char32_t codepoint) noexcept;

} // namespace unicode
