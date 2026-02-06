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
#include <libunicode/case_normalization_data.h>
#include <libunicode/convert.h>
#include <libunicode/ucd.h>
#include <libunicode/word_segmenter.h>

#include <algorithm>
#include <iterator>

namespace unicode
{

namespace
{
    // Binary search helper for sorted pair arrays
    template <typename T, size_t N>
    [[nodiscard]] constexpr char32_t lookup_simple(std::array<std::pair<char32_t, T>, N> const& table,
                                                   char32_t codepoint) noexcept
    {
        auto const it = std::lower_bound(
            table.begin(), table.end(), codepoint, [](auto const& pair, char32_t cp) { return pair.first < cp; });

        if (it != table.end() && it->first == codepoint)
            return static_cast<char32_t>(it->second);

        return codepoint;
    }

    // Binary search helper for full case mapping arrays
    template <typename Entry, size_t N>
    [[nodiscard]] case_mapping_result lookup_full(std::array<Entry, N> const& table, char32_t codepoint) noexcept
    {
        auto const it = std::lower_bound(
            table.begin(), table.end(), codepoint, [](auto const& entry, char32_t cp) { return entry.source < cp; });

        if (it != table.end() && it->source == codepoint)
        {
            case_mapping_result result;
            result.length = it->length;
            for (uint8_t i = 0; i < it->length; ++i)
                result.codepoints[i] = it->targets[i];
            return result;
        }

        return {}; // Identity mapping
    }

} // namespace

// ============================================================================
// Simple case mappings
// ============================================================================

char32_t simple_uppercase(char32_t codepoint) noexcept
{
    // Fast path for ASCII
    if (codepoint >= 'a' && codepoint <= 'z')
        return codepoint - ('a' - 'A');

    return lookup_simple(detail::simple_uppercase_table, codepoint);
}

char32_t simple_lowercase(char32_t codepoint) noexcept
{
    // Fast path for ASCII
    if (codepoint >= 'A' && codepoint <= 'Z')
        return codepoint + ('a' - 'A');

    return lookup_simple(detail::simple_lowercase_table, codepoint);
}

char32_t simple_titlecase(char32_t codepoint) noexcept
{
    // Fast path for ASCII (titlecase == uppercase for ASCII)
    if (codepoint >= 'a' && codepoint <= 'z')
        return codepoint - ('a' - 'A');

    return lookup_simple(detail::simple_titlecase_table, codepoint);
}

char32_t simple_casefold(char32_t codepoint) noexcept
{
    // Fast path for ASCII (casefold == lowercase for ASCII)
    if (codepoint >= 'A' && codepoint <= 'Z')
        return codepoint + ('a' - 'A');

    return lookup_simple(detail::simple_casefold_table, codepoint);
}

// ============================================================================
// Full case mappings
// ============================================================================

case_mapping_result full_uppercase(char32_t codepoint) noexcept
{
    // Check full mapping table first
    auto result = lookup_full(detail::full_uppercase_table, codepoint);
    if (!result.is_identity())
        return result;

    // Fall back to simple mapping
    auto const simple = simple_uppercase(codepoint);
    if (simple != codepoint)
    {
        result.codepoints[0] = simple;
        result.length = 1;
        return result;
    }

    return {}; // Identity
}

case_mapping_result full_lowercase(char32_t codepoint) noexcept
{
    // Check full mapping table first
    auto result = lookup_full(detail::full_lowercase_table, codepoint);
    if (!result.is_identity())
        return result;

    // Fall back to simple mapping
    auto const simple = simple_lowercase(codepoint);
    if (simple != codepoint)
    {
        result.codepoints[0] = simple;
        result.length = 1;
        return result;
    }

    return {}; // Identity
}

case_mapping_result full_titlecase(char32_t codepoint) noexcept
{
    // Check full mapping table first
    auto result = lookup_full(detail::full_titlecase_table, codepoint);
    if (!result.is_identity())
        return result;

    // Fall back to simple mapping
    auto const simple = simple_titlecase(codepoint);
    if (simple != codepoint)
    {
        result.codepoints[0] = simple;
        result.length = 1;
        return result;
    }

    return {}; // Identity
}

case_mapping_result full_casefold(char32_t codepoint) noexcept
{
    // Check full mapping table first
    auto result = lookup_full(detail::full_casefold_table, codepoint);
    if (!result.is_identity())
        return result;

    // Fall back to simple mapping
    auto const simple = simple_casefold(codepoint);
    if (simple != codepoint)
    {
        result.codepoints[0] = simple;
        result.length = 1;
        return result;
    }

    return {}; // Identity
}

// ============================================================================
// String-level operations (UTF-32)
// ============================================================================

std::u32string to_uppercase(std::u32string_view text)
{
    std::u32string result;
    result.reserve(text.size());

    for (char32_t cp: text)
    {
        auto const mapping = full_uppercase(cp);
        if (mapping.is_identity())
            result.push_back(cp);
        else
            result.append(mapping.view());
    }

    return result;
}

std::u32string to_lowercase(std::u32string_view text)
{
    std::u32string result;
    result.reserve(text.size());

    for (char32_t cp: text)
    {
        auto const mapping = full_lowercase(cp);
        if (mapping.is_identity())
            result.push_back(cp);
        else
            result.append(mapping.view());
    }

    return result;
}

std::u32string to_titlecase(std::u32string_view text)
{
    std::u32string result;
    result.reserve(text.size());

    bool at_word_start = true;

    for (char32_t cp: text)
    {
        // Simple word boundary detection: after space/punctuation
        bool const is_letter = is_cased(cp);

        if (at_word_start && is_letter)
        {
            // Titlecase the first letter of a word
            auto const mapping = full_titlecase(cp);
            if (mapping.is_identity())
                result.push_back(cp);
            else
                result.append(mapping.view());
            at_word_start = false;
        }
        else if (is_letter)
        {
            // Lowercase the rest of the word
            auto const mapping = full_lowercase(cp);
            if (mapping.is_identity())
                result.push_back(cp);
            else
                result.append(mapping.view());
        }
        else
        {
            result.push_back(cp);
            // Check if this is a word boundary
            if (cp == ' ' || cp == '\t' || cp == '\n' || cp == '\r' || general_category::dash_punctuation(cp)
                || general_category::open_punctuation(cp) || general_category::close_punctuation(cp))
            {
                at_word_start = true;
            }
        }
    }

    return result;
}

std::u32string casefold(std::u32string_view text)
{
    std::u32string result;
    result.reserve(text.size());

    for (char32_t cp: text)
    {
        auto const mapping = full_casefold(cp);
        if (mapping.is_identity())
            result.push_back(cp);
        else
            result.append(mapping.view());
    }

    return result;
}

// ============================================================================
// String-level operations (UTF-8)
// ============================================================================

std::string to_uppercase(std::string_view text)
{
    auto const u32text = convert_to<char32_t>(text);
    auto const u32result = to_uppercase(std::u32string_view(u32text));
    return convert_to<char>(std::u32string_view(u32result));
}

std::string to_lowercase(std::string_view text)
{
    auto const u32text = convert_to<char32_t>(text);
    auto const u32result = to_lowercase(std::u32string_view(u32text));
    return convert_to<char>(std::u32string_view(u32result));
}

std::string to_titlecase(std::string_view text)
{
    auto const u32text = convert_to<char32_t>(text);
    auto const u32result = to_titlecase(std::u32string_view(u32text));
    return convert_to<char>(std::u32string_view(u32result));
}

std::string casefold(std::string_view text)
{
    auto const u32text = convert_to<char32_t>(text);
    auto const u32result = casefold(std::u32string_view(u32text));
    return convert_to<char>(std::u32string_view(u32result));
}

// ============================================================================
// Case-insensitive comparison
// ============================================================================

int casefold_compare(std::u32string_view a, std::u32string_view b)
{
    auto const folded_a = casefold(a);
    auto const folded_b = casefold(b);
    return folded_a.compare(folded_b);
}

int casefold_compare(std::string_view a, std::string_view b)
{
    auto const folded_a = casefold(a);
    auto const folded_b = casefold(b);
    return folded_a.compare(folded_b);
}

bool casefold_equals(std::u32string_view a, std::u32string_view b)
{
    return casefold_compare(a, b) == 0;
}

bool casefold_equals(std::string_view a, std::string_view b)
{
    return casefold_compare(a, b) == 0;
}

// ============================================================================
// Case property queries
// ============================================================================

bool is_cased(char32_t codepoint) noexcept
{
    auto const gc = general_category::get(codepoint);
    return gc == General_Category::Uppercase_Letter || gc == General_Category::Lowercase_Letter
           || gc == General_Category::Titlecase_Letter;
}

bool is_case_ignorable(char32_t codepoint) noexcept
{
    // Case_Ignorable includes:
    // - General_Category = Mn, Me, Cf, Lm, Sk
    // - Word_Break = MidLetter, MidNumLet, Single_Quote
    auto const gc = general_category::get(codepoint);
    return gc == General_Category::Nonspacing_Mark || gc == General_Category::Enclosing_Mark || gc == General_Category::Format
           || gc == General_Category::Modifier_Letter || gc == General_Category::Modifier_Symbol;
}

bool changes_when_uppercased(char32_t codepoint) noexcept
{
    return simple_uppercase(codepoint) != codepoint || !full_uppercase(codepoint).is_identity();
}

bool changes_when_lowercased(char32_t codepoint) noexcept
{
    return simple_lowercase(codepoint) != codepoint || !full_lowercase(codepoint).is_identity();
}

bool changes_when_titlecased(char32_t codepoint) noexcept
{
    return simple_titlecase(codepoint) != codepoint || !full_titlecase(codepoint).is_identity();
}

bool changes_when_casefolded(char32_t codepoint) noexcept
{
    return simple_casefold(codepoint) != codepoint || !full_casefold(codepoint).is_identity();
}

} // namespace unicode
