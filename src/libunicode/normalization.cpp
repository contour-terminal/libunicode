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
#include <libunicode/case_normalization_data.h>
#include <libunicode/convert.h>
#include <libunicode/normalization.h>

#include <algorithm>
#include <iterator>

namespace unicode
{

// ============================================================================
// Hangul constants (from Unicode Standard, Chapter 3)
// ============================================================================
namespace hangul
{
    constexpr char32_t SBase = 0xAC00;
    constexpr char32_t LBase = 0x1100;
    constexpr char32_t VBase = 0x1161;
    constexpr char32_t TBase = 0x11A7;
    constexpr char32_t LCount = 19;
    constexpr char32_t VCount = 21;
    constexpr char32_t TCount = 28;
    constexpr char32_t NCount = VCount * TCount; // 588
    constexpr char32_t SCount = LCount * NCount; // 11172
} // namespace hangul

// ============================================================================
// Helper functions
// ============================================================================

namespace
{
    // Binary search in sorted codepoint array
    template <size_t N>
    [[nodiscard]] bool contains_codepoint(std::array<char32_t, N> const& table, char32_t codepoint) noexcept
    {
        return std::binary_search(table.begin(), table.end(), codepoint);
    }

    // Binary search for CCC lookup
    [[nodiscard]] uint8_t lookup_ccc(char32_t codepoint) noexcept
    {
        auto const& table = detail::ccc_table;
        auto const it = std::lower_bound(
            table.begin(), table.end(), codepoint, [](auto const& pair, char32_t cp) { return pair.first < cp; });

        if (it != table.end() && it->first == codepoint)
            return it->second;

        return 0;
    }

    // Look up canonical decomposition
    [[nodiscard]] std::vector<char32_t> lookup_canonical_decomposition(char32_t codepoint)
    {
        auto const& table = detail::canonical_decomposition_table;
        auto const it = std::lower_bound(
            table.begin(), table.end(), codepoint, [](auto const& entry, char32_t cp) { return entry.source < cp; });

        if (it != table.end() && it->source == codepoint)
        {
            std::vector<char32_t> result;
            result.reserve(it->length);
            for (uint8_t i = 0; i < it->length; ++i)
                result.push_back(it->targets[i]);
            return result;
        }

        return {};
    }

    // Try to compose two codepoints
    [[nodiscard]] char32_t try_compose(char32_t first, char32_t second) noexcept
    {
        // Check Hangul composition first (L + V or LV + T)
        if (first >= hangul::LBase && first < hangul::LBase + hangul::LCount)
        {
            if (second >= hangul::VBase && second < hangul::VBase + hangul::VCount)
            {
                // L + V -> LV
                return hangul::SBase + ((first - hangul::LBase) * hangul::VCount + (second - hangul::VBase)) * hangul::TCount;
            }
        }
        else if (first >= hangul::SBase && first < hangul::SBase + hangul::SCount)
        {
            if ((first - hangul::SBase) % hangul::TCount == 0)
            {
                if (second >= hangul::TBase && second < hangul::TBase + hangul::TCount)
                {
                    // LV + T -> LVT
                    return first + (second - hangul::TBase);
                }
            }
        }

        // Look up in composition table (binary search)
        auto const& table = detail::composition_table;
        auto it = table.begin();
        auto end = table.end();

        while (it != end)
        {
            auto mid = it + (end - it) / 2;
            if (mid->first < first || (mid->first == first && mid->second < second))
                it = mid + 1;
            else if (mid->first > first || (mid->first == first && mid->second > second))
                end = mid;
            else
                return mid->composed;
        }

        return 0; // No composition found
    }

    // Recursively decompose a codepoint
    void decompose_recursive(char32_t cp, std::u32string& output, bool compatibility)
    {
        // Handle Hangul syllables algorithmically
        if (is_hangul_syllable(cp))
        {
            char32_t jamos[3];
            auto const count = hangul_decompose(cp, jamos);
            for (size_t i = 0; i < count; ++i)
                output.push_back(jamos[i]);
            return;
        }

        // Look up decomposition
        auto decomp = lookup_canonical_decomposition(cp);
        if (decomp.empty())
        {
            // TODO: For NFKC/NFKD, also check compatibility decompositions
            output.push_back(cp);
            return;
        }

        // Recursively decompose each component
        for (char32_t c: decomp)
            decompose_recursive(c, output, compatibility);
    }

    // Canonical ordering of combining marks
    void canonical_order(std::u32string& text)
    {
        if (text.size() < 2)
            return;

        // Bubble sort adjacent combining marks by CCC (stable)
        for (size_t i = 1; i < text.size(); ++i)
        {
            auto const ccc_i = canonical_combining_class(text[i]);
            if (ccc_i == 0)
                continue; // Starter, no reordering needed

            size_t j = i;
            while (j > 0)
            {
                auto const ccc_prev = canonical_combining_class(text[j - 1]);
                if (ccc_prev == 0 || ccc_prev <= ccc_i)
                    break;
                std::swap(text[j - 1], text[j]);
                --j;
            }
        }
    }

    // Compose a decomposed string
    std::u32string compose(std::u32string const& decomposed)
    {
        if (decomposed.empty())
            return {};

        std::u32string result;
        result.reserve(decomposed.size());

        size_t i = 0;

        // Find first starter
        while (i < decomposed.size() && canonical_combining_class(decomposed[i]) != 0)
        {
            result.push_back(decomposed[i]);
            ++i;
        }

        if (i >= decomposed.size())
            return result;

        char32_t starter = decomposed[i++];
        uint8_t last_ccc = 0;

        while (i < decomposed.size())
        {
            char32_t cp = decomposed[i];
            uint8_t ccc = canonical_combining_class(cp);

            // Check if we can compose with the starter
            char32_t composed = 0;
            if (last_ccc < ccc || last_ccc == 0)
            {
                composed = try_compose(starter, cp);
            }

            if (composed != 0 && !is_composition_exclusion(composed))
            {
                // Composition succeeded
                starter = composed;
            }
            else if (ccc == 0)
            {
                // New starter
                result.push_back(starter);
                starter = cp;
                last_ccc = 0;
            }
            else
            {
                // Can't compose, keep the character
                result.push_back(cp);
                last_ccc = ccc;
            }

            ++i;
        }

        result.push_back(starter);
        return result;
    }

} // anonymous namespace

// ============================================================================
// Normalization properties
// ============================================================================

uint8_t canonical_combining_class(char32_t codepoint) noexcept
{
    return lookup_ccc(codepoint);
}

std::vector<char32_t> canonical_decomposition(char32_t codepoint)
{
    // Handle Hangul syllables algorithmically
    if (is_hangul_syllable(codepoint))
    {
        std::vector<char32_t> result;
        result.resize(3);
        auto const count = hangul_decompose(codepoint, result.data());
        result.resize(count);
        return result;
    }

    return lookup_canonical_decomposition(codepoint);
}

std::vector<char32_t> compatibility_decomposition(char32_t codepoint)
{
    // For now, return canonical decomposition
    // TODO: Implement full compatibility decomposition lookup
    return canonical_decomposition(codepoint);
}

Decomposition_Type decomposition_type(char32_t codepoint) noexcept
{
    // Check if there's any decomposition
    if (is_hangul_syllable(codepoint))
        return Decomposition_Type::Canonical;

    auto decomp = lookup_canonical_decomposition(codepoint);
    if (!decomp.empty())
        return Decomposition_Type::Canonical;

    return Decomposition_Type::None;
}

bool is_composition_exclusion(char32_t codepoint) noexcept
{
    return contains_codepoint(detail::composition_exclusions_table, codepoint);
}

bool is_full_composition_exclusion(char32_t codepoint) noexcept
{
    return is_composition_exclusion(codepoint);
}

// ============================================================================
// Quick Check properties
// ============================================================================

NFC_Quick_Check nfc_quick_check(char32_t codepoint) noexcept
{
    if (contains_codepoint(detail::nfc_qc_no_table, codepoint))
        return NFC_Quick_Check::No;
    if (contains_codepoint(detail::nfc_qc_maybe_table, codepoint))
        return NFC_Quick_Check::Maybe;
    return NFC_Quick_Check::Yes;
}

bool nfd_quick_check(char32_t codepoint) noexcept
{
    return !contains_codepoint(detail::nfd_qc_no_table, codepoint);
}

NFKC_Quick_Check nfkc_quick_check(char32_t codepoint) noexcept
{
    if (contains_codepoint(detail::nfkc_qc_no_table, codepoint))
        return NFKC_Quick_Check::No;
    if (contains_codepoint(detail::nfkc_qc_maybe_table, codepoint))
        return NFKC_Quick_Check::Maybe;
    return NFKC_Quick_Check::Yes;
}

bool nfkd_quick_check(char32_t codepoint) noexcept
{
    return !contains_codepoint(detail::nfkd_qc_no_table, codepoint);
}

// ============================================================================
// String normalization
// ============================================================================

std::u32string normalize(std::u32string_view text, Normalization_Form form)
{
    if (text.empty())
        return {};

    bool const use_compatibility = (form == Normalization_Form::NFKC || form == Normalization_Form::NFKD);

    // Step 1: Decompose
    std::u32string decomposed;
    decomposed.reserve(text.size() * 2);

    for (char32_t cp: text)
        decompose_recursive(cp, decomposed, use_compatibility);

    // Step 2: Canonical ordering
    canonical_order(decomposed);

    // Step 3: Compose (for NFC/NFKC only)
    if (form == Normalization_Form::NFC || form == Normalization_Form::NFKC)
        return compose(decomposed);

    return decomposed;
}

std::string normalize(std::string_view text, Normalization_Form form)
{
    auto const u32text = convert_to<char32_t>(text);
    auto const u32result = normalize(std::u32string_view(u32text), form);
    return convert_to<char>(std::u32string_view(u32result));
}

// ============================================================================
// Normalization checks
// ============================================================================

Quick_Check_Result quick_check(std::u32string_view text, Normalization_Form form)
{
    Quick_Check_Result result = Quick_Check_Result::Yes;
    uint8_t last_ccc = 0;

    for (char32_t cp: text)
    {
        uint8_t ccc = canonical_combining_class(cp);

        // Check canonical ordering
        if (ccc != 0 && last_ccc > ccc)
            return Quick_Check_Result::No;

        last_ccc = ccc;

        // Check quick check property
        switch (form)
        {
            case Normalization_Form::NFC: {
                auto qc = nfc_quick_check(cp);
                if (qc == NFC_Quick_Check::No)
                    return Quick_Check_Result::No;
                if (qc == NFC_Quick_Check::Maybe)
                    result = Quick_Check_Result::Maybe;
                break;
            }
            case Normalization_Form::NFD:
                if (!nfd_quick_check(cp))
                    return Quick_Check_Result::No;
                break;
            case Normalization_Form::NFKC: {
                auto qc = nfkc_quick_check(cp);
                if (qc == NFKC_Quick_Check::No)
                    return Quick_Check_Result::No;
                if (qc == NFKC_Quick_Check::Maybe)
                    result = Quick_Check_Result::Maybe;
                break;
            }
            case Normalization_Form::NFKD:
                if (!nfkd_quick_check(cp))
                    return Quick_Check_Result::No;
                break;
        }
    }

    return result;
}

Quick_Check_Result quick_check(std::string_view text, Normalization_Form form)
{
    auto const u32text = convert_to<char32_t>(text);
    return quick_check(std::u32string_view(u32text), form);
}

bool is_normalized(std::u32string_view text, Normalization_Form form)
{
    auto qc = quick_check(text, form);
    if (qc == Quick_Check_Result::Yes)
        return true;
    if (qc == Quick_Check_Result::No)
        return false;

    // Quick check returned Maybe, do full check
    auto const normalized = normalize(text, form);
    return normalized == text;
}

bool is_normalized(std::string_view text, Normalization_Form form)
{
    auto const u32text = convert_to<char32_t>(text);
    return is_normalized(std::u32string_view(u32text), form);
}

// ============================================================================
// Canonical equivalence
// ============================================================================

bool canonically_equivalent(std::u32string_view a, std::u32string_view b)
{
    return to_nfd(a) == to_nfd(b);
}

bool canonically_equivalent(std::string_view a, std::string_view b)
{
    return to_nfd(a) == to_nfd(b);
}

bool compatibility_equivalent(std::u32string_view a, std::u32string_view b)
{
    return to_nfkd(a) == to_nfkd(b);
}

bool compatibility_equivalent(std::string_view a, std::string_view b)
{
    return to_nfkd(a) == to_nfkd(b);
}

// ============================================================================
// Hangul algorithmic decomposition/composition
// ============================================================================

bool is_hangul_syllable(char32_t codepoint) noexcept
{
    return codepoint >= hangul::SBase && codepoint < hangul::SBase + hangul::SCount;
}

bool is_hangul_l_jamo(char32_t codepoint) noexcept
{
    return codepoint >= hangul::LBase && codepoint < hangul::LBase + hangul::LCount;
}

bool is_hangul_v_jamo(char32_t codepoint) noexcept
{
    return codepoint >= hangul::VBase && codepoint < hangul::VBase + hangul::VCount;
}

bool is_hangul_t_jamo(char32_t codepoint) noexcept
{
    return codepoint > hangul::TBase && codepoint < hangul::TBase + hangul::TCount;
}

size_t hangul_decompose(char32_t syllable, char32_t* output) noexcept
{
    if (!is_hangul_syllable(syllable))
        return 0;

    auto const sIndex = syllable - hangul::SBase;
    auto const lIndex = sIndex / hangul::NCount;
    auto const vIndex = (sIndex % hangul::NCount) / hangul::TCount;
    auto const tIndex = sIndex % hangul::TCount;

    output[0] = hangul::LBase + lIndex;
    output[1] = hangul::VBase + vIndex;

    if (tIndex > 0)
    {
        output[2] = hangul::TBase + tIndex;
        return 3;
    }

    return 2;
}

char32_t hangul_compose(char32_t l, char32_t v, char32_t t) noexcept
{
    if (!is_hangul_l_jamo(l) || !is_hangul_v_jamo(v))
        return 0;

    auto const lIndex = l - hangul::LBase;
    auto const vIndex = v - hangul::VBase;
    auto const lvIndex = lIndex * hangul::NCount + vIndex * hangul::TCount;

    if (t == 0)
        return hangul::SBase + lvIndex;

    if (!is_hangul_t_jamo(t))
        return 0;

    auto const tIndex = t - hangul::TBase;
    return hangul::SBase + lvIndex + tIndex;
}

} // namespace unicode
