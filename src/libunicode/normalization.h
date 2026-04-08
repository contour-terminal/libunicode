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
#include <libunicode/utf8.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace unicode
{

/// Normalization forms as defined by Unicode Standard Annex #15.
enum class Normalization_Form : uint8_t
{
    NFC,  ///< Canonical Decomposition, followed by Canonical Composition
    NFD,  ///< Canonical Decomposition
    NFKC, ///< Compatibility Decomposition, followed by Canonical Composition
    NFKD  ///< Compatibility Decomposition
};

/// Result of quick check for normalization.
enum class Quick_Check_Result : uint8_t
{
    Yes,  ///< String is definitely in the specified normalization form
    No,   ///< String is definitely NOT in the specified normalization form
    Maybe ///< String may or may not be normalized (need full check)
};

// ============================================================================
// Normalization properties for individual codepoints
// ============================================================================

/// Returns the canonical combining class for a codepoint.
/// Used for canonical ordering of combining marks.
[[nodiscard]] uint8_t canonical_combining_class(char32_t codepoint) noexcept;

/// Returns the canonical decomposition for a codepoint.
/// Returns empty vector if the codepoint has no decomposition.
[[nodiscard]] std::vector<char32_t> canonical_decomposition(char32_t codepoint);

/// Returns the compatibility decomposition for a codepoint.
/// Includes canonical decompositions.
[[nodiscard]] std::vector<char32_t> compatibility_decomposition(char32_t codepoint);

/// Returns the decomposition type for a codepoint.
[[nodiscard]] Decomposition_Type decomposition_type(char32_t codepoint) noexcept;

/// Checks if a codepoint is excluded from composition.
[[nodiscard]] bool is_composition_exclusion(char32_t codepoint) noexcept;

/// Checks if a codepoint has the Full_Composition_Exclusion property.
[[nodiscard]] bool is_full_composition_exclusion(char32_t codepoint) noexcept;

// ============================================================================
// Quick Check properties (for fast normalization detection)
// ============================================================================

/// Quick check for NFC normalization.
[[nodiscard]] NFC_Quick_Check nfc_quick_check(char32_t codepoint) noexcept;

/// Quick check for NFD normalization.
/// Always returns Yes or No (never Maybe).
[[nodiscard]] bool nfd_quick_check(char32_t codepoint) noexcept;

/// Quick check for NFKC normalization.
[[nodiscard]] NFKC_Quick_Check nfkc_quick_check(char32_t codepoint) noexcept;

/// Quick check for NFKD normalization.
/// Always returns Yes or No (never Maybe).
[[nodiscard]] bool nfkd_quick_check(char32_t codepoint) noexcept;

// ============================================================================
// String normalization
// ============================================================================

/// Normalizes a UTF-32 string to the specified normalization form.
[[nodiscard]] std::u32string normalize(std::u32string_view text, Normalization_Form form);

/// Normalizes a UTF-8 string to the specified normalization form.
[[nodiscard]] std::string normalize(std::string_view text, Normalization_Form form);

/// Normalizes a UTF-32 string to NFC.
[[nodiscard]] inline std::u32string to_nfc(std::u32string_view text)
{
    return normalize(text, Normalization_Form::NFC);
}

/// Normalizes a UTF-32 string to NFD.
[[nodiscard]] inline std::u32string to_nfd(std::u32string_view text)
{
    return normalize(text, Normalization_Form::NFD);
}

/// Normalizes a UTF-32 string to NFKC.
[[nodiscard]] inline std::u32string to_nfkc(std::u32string_view text)
{
    return normalize(text, Normalization_Form::NFKC);
}

/// Normalizes a UTF-32 string to NFKD.
[[nodiscard]] inline std::u32string to_nfkd(std::u32string_view text)
{
    return normalize(text, Normalization_Form::NFKD);
}

/// Normalizes a UTF-8 string to NFC.
[[nodiscard]] inline std::string to_nfc(std::string_view text)
{
    return normalize(text, Normalization_Form::NFC);
}

/// Normalizes a UTF-8 string to NFD.
[[nodiscard]] inline std::string to_nfd(std::string_view text)
{
    return normalize(text, Normalization_Form::NFD);
}

/// Normalizes a UTF-8 string to NFKC.
[[nodiscard]] inline std::string to_nfkc(std::string_view text)
{
    return normalize(text, Normalization_Form::NFKC);
}

/// Normalizes a UTF-8 string to NFKD.
[[nodiscard]] inline std::string to_nfkd(std::string_view text)
{
    return normalize(text, Normalization_Form::NFKD);
}

// ============================================================================
// Normalization checks
// ============================================================================

/// Performs a quick check to see if a string is normalized.
[[nodiscard]] Quick_Check_Result quick_check(std::u32string_view text, Normalization_Form form);

/// Performs a quick check on a UTF-8 string.
[[nodiscard]] Quick_Check_Result quick_check(std::string_view text, Normalization_Form form);

/// Returns true if the string is in the specified normalization form.
/// This performs a full check when quick_check returns Maybe.
[[nodiscard]] bool is_normalized(std::u32string_view text, Normalization_Form form);

/// Returns true if the UTF-8 string is in the specified normalization form.
[[nodiscard]] bool is_normalized(std::string_view text, Normalization_Form form);

/// Convenience functions for checking specific normalization forms.
[[nodiscard]] inline bool is_nfc(std::u32string_view text)
{
    return is_normalized(text, Normalization_Form::NFC);
}
[[nodiscard]] inline bool is_nfd(std::u32string_view text)
{
    return is_normalized(text, Normalization_Form::NFD);
}
[[nodiscard]] inline bool is_nfkc(std::u32string_view text)
{
    return is_normalized(text, Normalization_Form::NFKC);
}
[[nodiscard]] inline bool is_nfkd(std::u32string_view text)
{
    return is_normalized(text, Normalization_Form::NFKD);
}

[[nodiscard]] inline bool is_nfc(std::string_view text)
{
    return is_normalized(text, Normalization_Form::NFC);
}
[[nodiscard]] inline bool is_nfd(std::string_view text)
{
    return is_normalized(text, Normalization_Form::NFD);
}
[[nodiscard]] inline bool is_nfkc(std::string_view text)
{
    return is_normalized(text, Normalization_Form::NFKC);
}
[[nodiscard]] inline bool is_nfkd(std::string_view text)
{
    return is_normalized(text, Normalization_Form::NFKD);
}

// ============================================================================
// Canonical equivalence
// ============================================================================

/// Returns true if two strings are canonically equivalent (same NFC or NFD form).
[[nodiscard]] bool is_canonically_equivalent(std::u32string_view a, std::u32string_view b);

/// Returns true if two UTF-8 strings are canonically equivalent.
[[nodiscard]] bool is_canonically_equivalent(std::string_view a, std::string_view b);

/// Returns true if two strings are compatibility equivalent (same NFKC or NFKD form).
[[nodiscard]] bool is_compatibility_equivalent(std::u32string_view a, std::u32string_view b);

/// Returns true if two UTF-8 strings are compatibility equivalent.
[[nodiscard]] bool is_compatibility_equivalent(std::string_view a, std::string_view b);

// ============================================================================
// Hangul algorithmic decomposition/composition
// ============================================================================

/// Returns true if the codepoint is a Hangul syllable.
[[nodiscard]] bool is_hangul_syllable(char32_t codepoint) noexcept;

/// Returns true if the codepoint is a Hangul leading jamo (L).
[[nodiscard]] bool is_hangul_l_jamo(char32_t codepoint) noexcept;

/// Returns true if the codepoint is a Hangul vowel jamo (V).
[[nodiscard]] bool is_hangul_v_jamo(char32_t codepoint) noexcept;

/// Returns true if the codepoint is a Hangul trailing jamo (T).
[[nodiscard]] bool is_hangul_t_jamo(char32_t codepoint) noexcept;

/// Decomposes a Hangul syllable into L, V, (T) jamos.
/// Returns the number of jamos written (2 or 3), or 0 if not a syllable.
size_t hangul_decompose(char32_t syllable, char32_t* output) noexcept;

/// Composes Hangul jamos into a syllable.
/// Returns the composed syllable, or 0 if composition not possible.
[[nodiscard]] char32_t hangul_compose(char32_t l, char32_t v, char32_t t = 0) noexcept;

// ============================================================================
// Streaming normalizer
// ============================================================================

/// Streaming Unicode normalizer for incremental input processing.
///
/// Buffers codepoints until a safe normalization boundary is found (UAX#15 Section 9),
/// then normalizes the buffered segment. This enables processing arbitrarily large
/// text without loading it entirely into memory.
///
/// The returned string_view from feed()/flush() points into an internal buffer
/// and is valid until the next call to feed(), flush(), or reset().
///
/// @code
///     normalizer norm(Normalization_Form::NFC);
///     for (char32_t cp : input)
///     {
///         auto segment = norm.feed(cp);
///         if (!segment.empty())
///             process(segment);
///     }
///     auto final = norm.flush();
///     if (!final.empty())
///         process(final);
/// @endcode
class normalizer
{
  public:
    /// @param form The normalization form to apply.
    explicit normalizer(Normalization_Form form) noexcept;

    /// Feeds a single codepoint into the normalizer.
    /// @param codepoint The codepoint to process.
    /// @return Normalized output segment, or empty if still buffering.
    [[nodiscard]] std::u32string_view feed(char32_t codepoint);

    /// Flushes remaining buffered codepoints.
    /// Must be called when input is complete.
    /// @return Final normalized output segment, or empty if nothing was buffered.
    [[nodiscard]] std::u32string_view flush();

    /// Resets to initial state, discarding any buffered data.
    void reset() noexcept;

  private:
    [[nodiscard]] bool is_boundary(char32_t codepoint) const noexcept;
    std::u32string_view emit_pending();

    Normalization_Form _form;
    std::u32string _pending; ///< Current combining character sequence being buffered
    std::u32string _output;  ///< Last emitted normalized segment
};

/// Streaming UTF-8 normalizer.
///
/// Wraps a normalizer with incremental UTF-8 decoding, allowing normalization
/// of UTF-8 byte streams without requiring full decoding upfront.
///
/// The returned string_view from feed()/flush() points into an internal buffer
/// and is valid until the next call to feed(), flush(), or reset().
class utf8_normalizer
{
  public:
    /// @param form The normalization form to apply.
    explicit utf8_normalizer(Normalization_Form form) noexcept;

    /// Feeds a chunk of UTF-8 data into the normalizer.
    /// @param utf8Data UTF-8 encoded input bytes.
    /// @return Normalized UTF-8 output, or empty if still buffering.
    [[nodiscard]] std::string_view feed(std::string_view utf8Data);

    /// Flushes remaining buffered data.
    /// @return Final normalized UTF-8 output.
    [[nodiscard]] std::string_view flush();

    /// Resets to initial state.
    void reset() noexcept;

  private:
    normalizer _inner;
    utf8_decoder_state _utf8State {};
    std::string _utf8Output;
};

} // namespace unicode
