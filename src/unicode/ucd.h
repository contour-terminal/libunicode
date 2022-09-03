/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2020-2021 Christian Parpart <christian@parpart.family>
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

#include <unicode/ucd_enums.h>

#include <array>
#include <optional>
#include <string>
#include <utility>

namespace unicode
{

Plane plane(char32_t codepoint) noexcept;

bool contains(Core_Property prop, char32_t codepoint) noexcept;

bool contains(General_Category generalCategory, char32_t codepoint) noexcept;

// Disabling clang-format to avoid single-line folding implementations.
// clang-format off
namespace general_category
{
    General_Category get(char32_t value) noexcept;

    inline bool close_punctuation(char32_t codepoint) noexcept
    {
        return contains(General_Category::Close_Punctuation, codepoint);
    }

    inline bool connector_punctuation(char32_t codepoint) noexcept
    {
        return contains(General_Category::Connector_Punctuation, codepoint);
    }

    inline bool control(char32_t codepoint) noexcept
    {
        return contains(General_Category::Control, codepoint);
    }

    inline bool currency_symbol(char32_t codepoint) noexcept
    {
        return contains(General_Category::Currency_Symbol, codepoint);
    }

    inline bool dash_punctuation(char32_t codepoint) noexcept
    {
        return contains(General_Category::Dash_Punctuation, codepoint);
    }

    inline bool decimal_number(char32_t codepoint) noexcept
    {
        return contains(General_Category::Decimal_Number, codepoint);
    }

    inline bool enclosing_mark(char32_t codepoint) noexcept
    {
        return contains(General_Category::Enclosing_Mark, codepoint);
    }

    inline bool final_punctuation(char32_t codepoint) noexcept
    {
        return contains(General_Category::Final_Punctuation, codepoint);
    }

    inline bool format(char32_t codepoint) noexcept
    {
        return contains(General_Category::Format, codepoint);
    }

    inline bool initial_punctuation(char32_t codepoint) noexcept
    {
        return contains(General_Category::Initial_Punctuation, codepoint);
    }

    inline bool letter_number(char32_t codepoint) noexcept
    {
        return contains(General_Category::Letter_Number, codepoint);
    }

    inline bool line_separator(char32_t codepoint) noexcept
    {
        return contains(General_Category::Line_Separator, codepoint);
    }

    inline bool lowercase_letter(char32_t codepoint) noexcept
    {
        return contains(General_Category::Lowercase_Letter, codepoint);
    }

    inline bool math_symbol(char32_t codepoint) noexcept
    {
        return contains(General_Category::Math_Symbol, codepoint);
    }

    inline bool modifier_letter(char32_t codepoint) noexcept
    {
        return contains(General_Category::Modifier_Letter, codepoint);
    }

    inline bool modifier_symbol(char32_t codepoint) noexcept
    {
        return contains(General_Category::Modifier_Symbol, codepoint);
    }

    inline bool nonspacing_mark(char32_t codepoint) noexcept
    {
        return contains(General_Category::Nonspacing_Mark, codepoint);
    }

    inline bool open_punctuation(char32_t codepoint) noexcept
    {
        return contains(General_Category::Open_Punctuation, codepoint);
    }

    inline bool other_letter(char32_t codepoint) noexcept
    {
        return contains(General_Category::Other_Letter, codepoint);
    }

    inline bool other_number(char32_t codepoint) noexcept
    {
        return contains(General_Category::Other_Number, codepoint);
    }

    inline bool other_punctuation(char32_t codepoint) noexcept
    {
        return contains(General_Category::Other_Punctuation, codepoint);
    }

    inline bool other_symbol(char32_t codepoint) noexcept
    {
        return contains(General_Category::Other_Symbol, codepoint);
    }

    inline bool paragraph_separator(char32_t codepoint) noexcept
    {
        return contains(General_Category::Paragraph_Separator, codepoint);
    }

    inline bool private_use(char32_t codepoint) noexcept
    {
        return contains(General_Category::Private_Use, codepoint);
    }

    inline bool space_separator(char32_t codepoint) noexcept
    {
        return contains(General_Category::Space_Separator, codepoint);
    }

    inline bool spacing_mark(char32_t codepoint) noexcept
    {
        return contains(General_Category::Spacing_Mark, codepoint);
    }

    inline bool surrogate(char32_t codepoint) noexcept
    {
        return contains(General_Category::Surrogate, codepoint);
    }

    inline bool titlecase_letter(char32_t codepoint) noexcept
    {
        return contains(General_Category::Titlecase_Letter, codepoint);
    }

    inline bool unassigned(char32_t codepoint) noexcept
    {
        return contains(General_Category::Unassigned, codepoint);
    }

    inline bool uppercase_letter(char32_t codepoint) noexcept
    {
        return contains(General_Category::Uppercase_Letter, codepoint);
    }

} // namespace general_category
// clang-format on

Script script(char32_t codepoint) noexcept;

size_t script_extensions(char32_t codepoint, Script* result, size_t capacity) noexcept;

Block block(char32_t codepoint) noexcept;

namespace grapheme_cluster_break
{
    bool cr(char32_t codepoint) noexcept;
    bool control(char32_t codepoint) noexcept;
    bool extend(char32_t codepoint) noexcept;
    bool l(char32_t codepoint) noexcept;
    bool lf(char32_t codepoint) noexcept;
    bool lv(char32_t codepoint) noexcept;
    bool lvt(char32_t codepoint) noexcept;
    bool prepend(char32_t codepoint) noexcept;
    bool regional_indicator(char32_t codepoint) noexcept;
    bool spacingmark(char32_t codepoint) noexcept;
    bool t(char32_t codepoint) noexcept;
    bool v(char32_t codepoint) noexcept;
    bool zwj(char32_t codepoint) noexcept;
} // namespace grapheme_cluster_break

inline std::string to_string(EastAsianWidth value)
{
    switch (value)
    {
        case EastAsianWidth::Ambiguous: return "Ambiguous";
        case EastAsianWidth::FullWidth: return "FullWidth";
        case EastAsianWidth::HalfWidth: return "HalfWidth";
        case EastAsianWidth::Neutral: return "Neutral";
        case EastAsianWidth::Narrow: return "Narrow";
        case EastAsianWidth::Wide: return "Wide";
        case EastAsianWidth::Unspecified: return "Unspecified";
    }
    return "Unknown";
}

EastAsianWidth east_asian_width(char32_t codepoint) noexcept;

bool emoji(char32_t codepoint) noexcept;
bool emoji_component(char32_t codepoint) noexcept;
bool emoji_modifier(char32_t codepoint) noexcept;
bool emoji_modifier_base(char32_t codepoint) noexcept;
bool emoji_presentation(char32_t codepoint) noexcept;
bool extended_pictographic(char32_t codepoint) noexcept;

} // namespace unicode
