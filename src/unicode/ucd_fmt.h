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

#include <fmt/format.h>

namespace fmt
{

template <>
struct formatter<unicode::Plane>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Plane _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Plane::Basic_Multilingual_Plane: return format_to(ctx.out(), "Basic_Multilingual_Plane");
        case unicode::Plane::Supplementary_Ideographic_Plane: return format_to(ctx.out(), "Supplementary_Ideographic_Plane");
        case unicode::Plane::Supplementary_Multilingual_Plane: return format_to(ctx.out(), "Supplementary_Multilingual_Plane");
        case unicode::Plane::Supplementary_Private_Use_Area_Plane: return format_to(ctx.out(), "Supplementary_Private_Use_Area_Plane");
        case unicode::Plane::Supplementary_Special_purpose_Plane: return format_to(ctx.out(), "Supplementary_Special_purpose_Plane");
        case unicode::Plane::Tertiary_Ideographic_Plane: return format_to(ctx.out(), "Tertiary_Ideographic_Plane");
        case unicode::Plane::Unassigned: return format_to(ctx.out(), "Unassigned");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Age>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Age _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Age::Unassigned: return format_to(ctx.out(), "Unassigned");
        case unicode::Age::V10_0: return format_to(ctx.out(), "V10_0");
        case unicode::Age::V11_0: return format_to(ctx.out(), "V11_0");
        case unicode::Age::V12_0: return format_to(ctx.out(), "V12_0");
        case unicode::Age::V12_1: return format_to(ctx.out(), "V12_1");
        case unicode::Age::V13_0: return format_to(ctx.out(), "V13_0");
        case unicode::Age::V1_1: return format_to(ctx.out(), "V1_1");
        case unicode::Age::V2_0: return format_to(ctx.out(), "V2_0");
        case unicode::Age::V2_1: return format_to(ctx.out(), "V2_1");
        case unicode::Age::V3_0: return format_to(ctx.out(), "V3_0");
        case unicode::Age::V3_1: return format_to(ctx.out(), "V3_1");
        case unicode::Age::V3_2: return format_to(ctx.out(), "V3_2");
        case unicode::Age::V4_0: return format_to(ctx.out(), "V4_0");
        case unicode::Age::V4_1: return format_to(ctx.out(), "V4_1");
        case unicode::Age::V5_0: return format_to(ctx.out(), "V5_0");
        case unicode::Age::V5_1: return format_to(ctx.out(), "V5_1");
        case unicode::Age::V5_2: return format_to(ctx.out(), "V5_2");
        case unicode::Age::V6_0: return format_to(ctx.out(), "V6_0");
        case unicode::Age::V6_1: return format_to(ctx.out(), "V6_1");
        case unicode::Age::V6_2: return format_to(ctx.out(), "V6_2");
        case unicode::Age::V6_3: return format_to(ctx.out(), "V6_3");
        case unicode::Age::V7_0: return format_to(ctx.out(), "V7_0");
        case unicode::Age::V8_0: return format_to(ctx.out(), "V8_0");
        case unicode::Age::V9_0: return format_to(ctx.out(), "V9_0");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Bidi_Class>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Bidi_Class _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Bidi_Class::Arabic_Letter: return format_to(ctx.out(), "Arabic_Letter");
        case unicode::Bidi_Class::Arabic_Number: return format_to(ctx.out(), "Arabic_Number");
        case unicode::Bidi_Class::Boundary_Neutral: return format_to(ctx.out(), "Boundary_Neutral");
        case unicode::Bidi_Class::Common_Separator: return format_to(ctx.out(), "Common_Separator");
        case unicode::Bidi_Class::European_Number: return format_to(ctx.out(), "European_Number");
        case unicode::Bidi_Class::European_Separator: return format_to(ctx.out(), "European_Separator");
        case unicode::Bidi_Class::European_Terminator: return format_to(ctx.out(), "European_Terminator");
        case unicode::Bidi_Class::First_Strong_Isolate: return format_to(ctx.out(), "First_Strong_Isolate");
        case unicode::Bidi_Class::Left_To_Right: return format_to(ctx.out(), "Left_To_Right");
        case unicode::Bidi_Class::Left_To_Right_Embedding: return format_to(ctx.out(), "Left_To_Right_Embedding");
        case unicode::Bidi_Class::Left_To_Right_Isolate: return format_to(ctx.out(), "Left_To_Right_Isolate");
        case unicode::Bidi_Class::Left_To_Right_Override: return format_to(ctx.out(), "Left_To_Right_Override");
        case unicode::Bidi_Class::Nonspacing_Mark: return format_to(ctx.out(), "Nonspacing_Mark");
        case unicode::Bidi_Class::Other_Neutral: return format_to(ctx.out(), "Other_Neutral");
        case unicode::Bidi_Class::Paragraph_Separator: return format_to(ctx.out(), "Paragraph_Separator");
        case unicode::Bidi_Class::Pop_Directional_Format: return format_to(ctx.out(), "Pop_Directional_Format");
        case unicode::Bidi_Class::Pop_Directional_Isolate: return format_to(ctx.out(), "Pop_Directional_Isolate");
        case unicode::Bidi_Class::Right_To_Left: return format_to(ctx.out(), "Right_To_Left");
        case unicode::Bidi_Class::Right_To_Left_Embedding: return format_to(ctx.out(), "Right_To_Left_Embedding");
        case unicode::Bidi_Class::Right_To_Left_Isolate: return format_to(ctx.out(), "Right_To_Left_Isolate");
        case unicode::Bidi_Class::Right_To_Left_Override: return format_to(ctx.out(), "Right_To_Left_Override");
        case unicode::Bidi_Class::Segment_Separator: return format_to(ctx.out(), "Segment_Separator");
        case unicode::Bidi_Class::White_Space: return format_to(ctx.out(), "White_Space");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Bidi_Paired_Bracket_Type>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Bidi_Paired_Bracket_Type _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Bidi_Paired_Bracket_Type::Close: return format_to(ctx.out(), "Close");
        case unicode::Bidi_Paired_Bracket_Type::None: return format_to(ctx.out(), "None");
        case unicode::Bidi_Paired_Bracket_Type::Open: return format_to(ctx.out(), "Open");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Canonical_Combining_Class>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Canonical_Combining_Class _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Canonical_Combining_Class::A: return format_to(ctx.out(), "A");
        case unicode::Canonical_Combining_Class::AL: return format_to(ctx.out(), "AL");
        case unicode::Canonical_Combining_Class::AR: return format_to(ctx.out(), "AR");
        case unicode::Canonical_Combining_Class::ATA: return format_to(ctx.out(), "ATA");
        case unicode::Canonical_Combining_Class::ATAR: return format_to(ctx.out(), "ATAR");
        case unicode::Canonical_Combining_Class::ATB: return format_to(ctx.out(), "ATB");
        case unicode::Canonical_Combining_Class::ATBL: return format_to(ctx.out(), "ATBL");
        case unicode::Canonical_Combining_Class::B: return format_to(ctx.out(), "B");
        case unicode::Canonical_Combining_Class::BL: return format_to(ctx.out(), "BL");
        case unicode::Canonical_Combining_Class::BR: return format_to(ctx.out(), "BR");
        case unicode::Canonical_Combining_Class::CCC10: return format_to(ctx.out(), "CCC10");
        case unicode::Canonical_Combining_Class::CCC103: return format_to(ctx.out(), "CCC103");
        case unicode::Canonical_Combining_Class::CCC107: return format_to(ctx.out(), "CCC107");
        case unicode::Canonical_Combining_Class::CCC11: return format_to(ctx.out(), "CCC11");
        case unicode::Canonical_Combining_Class::CCC118: return format_to(ctx.out(), "CCC118");
        case unicode::Canonical_Combining_Class::CCC12: return format_to(ctx.out(), "CCC12");
        case unicode::Canonical_Combining_Class::CCC122: return format_to(ctx.out(), "CCC122");
        case unicode::Canonical_Combining_Class::CCC129: return format_to(ctx.out(), "CCC129");
        case unicode::Canonical_Combining_Class::CCC13: return format_to(ctx.out(), "CCC13");
        case unicode::Canonical_Combining_Class::CCC130: return format_to(ctx.out(), "CCC130");
        case unicode::Canonical_Combining_Class::CCC132: return format_to(ctx.out(), "CCC132");
        case unicode::Canonical_Combining_Class::CCC133: return format_to(ctx.out(), "CCC133");
        case unicode::Canonical_Combining_Class::CCC14: return format_to(ctx.out(), "CCC14");
        case unicode::Canonical_Combining_Class::CCC15: return format_to(ctx.out(), "CCC15");
        case unicode::Canonical_Combining_Class::CCC16: return format_to(ctx.out(), "CCC16");
        case unicode::Canonical_Combining_Class::CCC17: return format_to(ctx.out(), "CCC17");
        case unicode::Canonical_Combining_Class::CCC18: return format_to(ctx.out(), "CCC18");
        case unicode::Canonical_Combining_Class::CCC19: return format_to(ctx.out(), "CCC19");
        case unicode::Canonical_Combining_Class::CCC20: return format_to(ctx.out(), "CCC20");
        case unicode::Canonical_Combining_Class::CCC21: return format_to(ctx.out(), "CCC21");
        case unicode::Canonical_Combining_Class::CCC22: return format_to(ctx.out(), "CCC22");
        case unicode::Canonical_Combining_Class::CCC23: return format_to(ctx.out(), "CCC23");
        case unicode::Canonical_Combining_Class::CCC24: return format_to(ctx.out(), "CCC24");
        case unicode::Canonical_Combining_Class::CCC25: return format_to(ctx.out(), "CCC25");
        case unicode::Canonical_Combining_Class::CCC26: return format_to(ctx.out(), "CCC26");
        case unicode::Canonical_Combining_Class::CCC27: return format_to(ctx.out(), "CCC27");
        case unicode::Canonical_Combining_Class::CCC28: return format_to(ctx.out(), "CCC28");
        case unicode::Canonical_Combining_Class::CCC29: return format_to(ctx.out(), "CCC29");
        case unicode::Canonical_Combining_Class::CCC30: return format_to(ctx.out(), "CCC30");
        case unicode::Canonical_Combining_Class::CCC31: return format_to(ctx.out(), "CCC31");
        case unicode::Canonical_Combining_Class::CCC32: return format_to(ctx.out(), "CCC32");
        case unicode::Canonical_Combining_Class::CCC33: return format_to(ctx.out(), "CCC33");
        case unicode::Canonical_Combining_Class::CCC34: return format_to(ctx.out(), "CCC34");
        case unicode::Canonical_Combining_Class::CCC35: return format_to(ctx.out(), "CCC35");
        case unicode::Canonical_Combining_Class::CCC36: return format_to(ctx.out(), "CCC36");
        case unicode::Canonical_Combining_Class::CCC84: return format_to(ctx.out(), "CCC84");
        case unicode::Canonical_Combining_Class::CCC91: return format_to(ctx.out(), "CCC91");
        case unicode::Canonical_Combining_Class::DA: return format_to(ctx.out(), "DA");
        case unicode::Canonical_Combining_Class::DB: return format_to(ctx.out(), "DB");
        case unicode::Canonical_Combining_Class::HANR: return format_to(ctx.out(), "HANR");
        case unicode::Canonical_Combining_Class::IS: return format_to(ctx.out(), "IS");
        case unicode::Canonical_Combining_Class::KV: return format_to(ctx.out(), "KV");
        case unicode::Canonical_Combining_Class::L: return format_to(ctx.out(), "L");
        case unicode::Canonical_Combining_Class::NK: return format_to(ctx.out(), "NK");
        case unicode::Canonical_Combining_Class::NR: return format_to(ctx.out(), "NR");
        case unicode::Canonical_Combining_Class::OV: return format_to(ctx.out(), "OV");
        case unicode::Canonical_Combining_Class::R: return format_to(ctx.out(), "R");
        case unicode::Canonical_Combining_Class::VR: return format_to(ctx.out(), "VR");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Decomposition_Type>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Decomposition_Type _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Decomposition_Type::Canonical: return format_to(ctx.out(), "Canonical");
        case unicode::Decomposition_Type::Circle: return format_to(ctx.out(), "Circle");
        case unicode::Decomposition_Type::Compat: return format_to(ctx.out(), "Compat");
        case unicode::Decomposition_Type::Final: return format_to(ctx.out(), "Final");
        case unicode::Decomposition_Type::Font: return format_to(ctx.out(), "Font");
        case unicode::Decomposition_Type::Fraction: return format_to(ctx.out(), "Fraction");
        case unicode::Decomposition_Type::Initial: return format_to(ctx.out(), "Initial");
        case unicode::Decomposition_Type::Isolated: return format_to(ctx.out(), "Isolated");
        case unicode::Decomposition_Type::Medial: return format_to(ctx.out(), "Medial");
        case unicode::Decomposition_Type::Narrow: return format_to(ctx.out(), "Narrow");
        case unicode::Decomposition_Type::Nobreak: return format_to(ctx.out(), "Nobreak");
        case unicode::Decomposition_Type::None: return format_to(ctx.out(), "None");
        case unicode::Decomposition_Type::Small: return format_to(ctx.out(), "Small");
        case unicode::Decomposition_Type::Square: return format_to(ctx.out(), "Square");
        case unicode::Decomposition_Type::Sub: return format_to(ctx.out(), "Sub");
        case unicode::Decomposition_Type::Super: return format_to(ctx.out(), "Super");
        case unicode::Decomposition_Type::Vertical: return format_to(ctx.out(), "Vertical");
        case unicode::Decomposition_Type::Wide: return format_to(ctx.out(), "Wide");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::East_Asian_Width>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::East_Asian_Width _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::East_Asian_Width::Ambiguous: return format_to(ctx.out(), "Ambiguous");
        case unicode::East_Asian_Width::Fullwidth: return format_to(ctx.out(), "Fullwidth");
        case unicode::East_Asian_Width::Halfwidth: return format_to(ctx.out(), "Halfwidth");
        case unicode::East_Asian_Width::Narrow: return format_to(ctx.out(), "Narrow");
        case unicode::East_Asian_Width::Neutral: return format_to(ctx.out(), "Neutral");
        case unicode::East_Asian_Width::Wide: return format_to(ctx.out(), "Wide");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Hangul_Syllable_Type>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Hangul_Syllable_Type _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Hangul_Syllable_Type::LVT_Syllable: return format_to(ctx.out(), "LVT_Syllable");
        case unicode::Hangul_Syllable_Type::LV_Syllable: return format_to(ctx.out(), "LV_Syllable");
        case unicode::Hangul_Syllable_Type::Leading_Jamo: return format_to(ctx.out(), "Leading_Jamo");
        case unicode::Hangul_Syllable_Type::Not_Applicable: return format_to(ctx.out(), "Not_Applicable");
        case unicode::Hangul_Syllable_Type::Trailing_Jamo: return format_to(ctx.out(), "Trailing_Jamo");
        case unicode::Hangul_Syllable_Type::Vowel_Jamo: return format_to(ctx.out(), "Vowel_Jamo");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Indic_Positional_Category>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Indic_Positional_Category _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Indic_Positional_Category::Bottom: return format_to(ctx.out(), "Bottom");
        case unicode::Indic_Positional_Category::Bottom_And_Left: return format_to(ctx.out(), "Bottom_And_Left");
        case unicode::Indic_Positional_Category::Bottom_And_Right: return format_to(ctx.out(), "Bottom_And_Right");
        case unicode::Indic_Positional_Category::Left: return format_to(ctx.out(), "Left");
        case unicode::Indic_Positional_Category::Left_And_Right: return format_to(ctx.out(), "Left_And_Right");
        case unicode::Indic_Positional_Category::NA: return format_to(ctx.out(), "NA");
        case unicode::Indic_Positional_Category::Overstruck: return format_to(ctx.out(), "Overstruck");
        case unicode::Indic_Positional_Category::Right: return format_to(ctx.out(), "Right");
        case unicode::Indic_Positional_Category::Top: return format_to(ctx.out(), "Top");
        case unicode::Indic_Positional_Category::Top_And_Bottom: return format_to(ctx.out(), "Top_And_Bottom");
        case unicode::Indic_Positional_Category::Top_And_Bottom_And_Left: return format_to(ctx.out(), "Top_And_Bottom_And_Left");
        case unicode::Indic_Positional_Category::Top_And_Bottom_And_Right: return format_to(ctx.out(), "Top_And_Bottom_And_Right");
        case unicode::Indic_Positional_Category::Top_And_Left: return format_to(ctx.out(), "Top_And_Left");
        case unicode::Indic_Positional_Category::Top_And_Left_And_Right: return format_to(ctx.out(), "Top_And_Left_And_Right");
        case unicode::Indic_Positional_Category::Top_And_Right: return format_to(ctx.out(), "Top_And_Right");
        case unicode::Indic_Positional_Category::Visual_Order_Left: return format_to(ctx.out(), "Visual_Order_Left");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Indic_Syllabic_Category>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Indic_Syllabic_Category _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Indic_Syllabic_Category::Avagraha: return format_to(ctx.out(), "Avagraha");
        case unicode::Indic_Syllabic_Category::Bindu: return format_to(ctx.out(), "Bindu");
        case unicode::Indic_Syllabic_Category::Brahmi_Joining_Number: return format_to(ctx.out(), "Brahmi_Joining_Number");
        case unicode::Indic_Syllabic_Category::Cantillation_Mark: return format_to(ctx.out(), "Cantillation_Mark");
        case unicode::Indic_Syllabic_Category::Consonant: return format_to(ctx.out(), "Consonant");
        case unicode::Indic_Syllabic_Category::Consonant_Dead: return format_to(ctx.out(), "Consonant_Dead");
        case unicode::Indic_Syllabic_Category::Consonant_Final: return format_to(ctx.out(), "Consonant_Final");
        case unicode::Indic_Syllabic_Category::Consonant_Head_Letter: return format_to(ctx.out(), "Consonant_Head_Letter");
        case unicode::Indic_Syllabic_Category::Consonant_Initial_Postfixed: return format_to(ctx.out(), "Consonant_Initial_Postfixed");
        case unicode::Indic_Syllabic_Category::Consonant_Killer: return format_to(ctx.out(), "Consonant_Killer");
        case unicode::Indic_Syllabic_Category::Consonant_Medial: return format_to(ctx.out(), "Consonant_Medial");
        case unicode::Indic_Syllabic_Category::Consonant_Placeholder: return format_to(ctx.out(), "Consonant_Placeholder");
        case unicode::Indic_Syllabic_Category::Consonant_Preceding_Repha: return format_to(ctx.out(), "Consonant_Preceding_Repha");
        case unicode::Indic_Syllabic_Category::Consonant_Prefixed: return format_to(ctx.out(), "Consonant_Prefixed");
        case unicode::Indic_Syllabic_Category::Consonant_Subjoined: return format_to(ctx.out(), "Consonant_Subjoined");
        case unicode::Indic_Syllabic_Category::Consonant_Succeeding_Repha: return format_to(ctx.out(), "Consonant_Succeeding_Repha");
        case unicode::Indic_Syllabic_Category::Consonant_With_Stacker: return format_to(ctx.out(), "Consonant_With_Stacker");
        case unicode::Indic_Syllabic_Category::Gemination_Mark: return format_to(ctx.out(), "Gemination_Mark");
        case unicode::Indic_Syllabic_Category::Invisible_Stacker: return format_to(ctx.out(), "Invisible_Stacker");
        case unicode::Indic_Syllabic_Category::Joiner: return format_to(ctx.out(), "Joiner");
        case unicode::Indic_Syllabic_Category::Modifying_Letter: return format_to(ctx.out(), "Modifying_Letter");
        case unicode::Indic_Syllabic_Category::Non_Joiner: return format_to(ctx.out(), "Non_Joiner");
        case unicode::Indic_Syllabic_Category::Nukta: return format_to(ctx.out(), "Nukta");
        case unicode::Indic_Syllabic_Category::Number: return format_to(ctx.out(), "Number");
        case unicode::Indic_Syllabic_Category::Number_Joiner: return format_to(ctx.out(), "Number_Joiner");
        case unicode::Indic_Syllabic_Category::Other: return format_to(ctx.out(), "Other");
        case unicode::Indic_Syllabic_Category::Pure_Killer: return format_to(ctx.out(), "Pure_Killer");
        case unicode::Indic_Syllabic_Category::Register_Shifter: return format_to(ctx.out(), "Register_Shifter");
        case unicode::Indic_Syllabic_Category::Syllable_Modifier: return format_to(ctx.out(), "Syllable_Modifier");
        case unicode::Indic_Syllabic_Category::Tone_Letter: return format_to(ctx.out(), "Tone_Letter");
        case unicode::Indic_Syllabic_Category::Tone_Mark: return format_to(ctx.out(), "Tone_Mark");
        case unicode::Indic_Syllabic_Category::Virama: return format_to(ctx.out(), "Virama");
        case unicode::Indic_Syllabic_Category::Visarga: return format_to(ctx.out(), "Visarga");
        case unicode::Indic_Syllabic_Category::Vowel: return format_to(ctx.out(), "Vowel");
        case unicode::Indic_Syllabic_Category::Vowel_Dependent: return format_to(ctx.out(), "Vowel_Dependent");
        case unicode::Indic_Syllabic_Category::Vowel_Independent: return format_to(ctx.out(), "Vowel_Independent");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Jamo_Short_Name>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Jamo_Short_Name _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Jamo_Short_Name::A: return format_to(ctx.out(), "A");
        case unicode::Jamo_Short_Name::AE: return format_to(ctx.out(), "AE");
        case unicode::Jamo_Short_Name::B: return format_to(ctx.out(), "B");
        case unicode::Jamo_Short_Name::BB: return format_to(ctx.out(), "BB");
        case unicode::Jamo_Short_Name::BS: return format_to(ctx.out(), "BS");
        case unicode::Jamo_Short_Name::C: return format_to(ctx.out(), "C");
        case unicode::Jamo_Short_Name::D: return format_to(ctx.out(), "D");
        case unicode::Jamo_Short_Name::DD: return format_to(ctx.out(), "DD");
        case unicode::Jamo_Short_Name::E: return format_to(ctx.out(), "E");
        case unicode::Jamo_Short_Name::EO: return format_to(ctx.out(), "EO");
        case unicode::Jamo_Short_Name::EU: return format_to(ctx.out(), "EU");
        case unicode::Jamo_Short_Name::G: return format_to(ctx.out(), "G");
        case unicode::Jamo_Short_Name::GG: return format_to(ctx.out(), "GG");
        case unicode::Jamo_Short_Name::GS: return format_to(ctx.out(), "GS");
        case unicode::Jamo_Short_Name::H: return format_to(ctx.out(), "H");
        case unicode::Jamo_Short_Name::I: return format_to(ctx.out(), "I");
        case unicode::Jamo_Short_Name::J: return format_to(ctx.out(), "J");
        case unicode::Jamo_Short_Name::JJ: return format_to(ctx.out(), "JJ");
        case unicode::Jamo_Short_Name::K: return format_to(ctx.out(), "K");
        case unicode::Jamo_Short_Name::L: return format_to(ctx.out(), "L");
        case unicode::Jamo_Short_Name::LB: return format_to(ctx.out(), "LB");
        case unicode::Jamo_Short_Name::LG: return format_to(ctx.out(), "LG");
        case unicode::Jamo_Short_Name::LH: return format_to(ctx.out(), "LH");
        case unicode::Jamo_Short_Name::LM: return format_to(ctx.out(), "LM");
        case unicode::Jamo_Short_Name::LP: return format_to(ctx.out(), "LP");
        case unicode::Jamo_Short_Name::LS: return format_to(ctx.out(), "LS");
        case unicode::Jamo_Short_Name::LT: return format_to(ctx.out(), "LT");
        case unicode::Jamo_Short_Name::M: return format_to(ctx.out(), "M");
        case unicode::Jamo_Short_Name::N: return format_to(ctx.out(), "N");
        case unicode::Jamo_Short_Name::NG: return format_to(ctx.out(), "NG");
        case unicode::Jamo_Short_Name::NH: return format_to(ctx.out(), "NH");
        case unicode::Jamo_Short_Name::NJ: return format_to(ctx.out(), "NJ");
        case unicode::Jamo_Short_Name::O: return format_to(ctx.out(), "O");
        case unicode::Jamo_Short_Name::OE: return format_to(ctx.out(), "OE");
        case unicode::Jamo_Short_Name::P: return format_to(ctx.out(), "P");
        case unicode::Jamo_Short_Name::R: return format_to(ctx.out(), "R");
        case unicode::Jamo_Short_Name::S: return format_to(ctx.out(), "S");
        case unicode::Jamo_Short_Name::SS: return format_to(ctx.out(), "SS");
        case unicode::Jamo_Short_Name::T: return format_to(ctx.out(), "T");
        case unicode::Jamo_Short_Name::U: return format_to(ctx.out(), "U");
        case unicode::Jamo_Short_Name::WA: return format_to(ctx.out(), "WA");
        case unicode::Jamo_Short_Name::WAE: return format_to(ctx.out(), "WAE");
        case unicode::Jamo_Short_Name::WE: return format_to(ctx.out(), "WE");
        case unicode::Jamo_Short_Name::WEO: return format_to(ctx.out(), "WEO");
        case unicode::Jamo_Short_Name::WI: return format_to(ctx.out(), "WI");
        case unicode::Jamo_Short_Name::YA: return format_to(ctx.out(), "YA");
        case unicode::Jamo_Short_Name::YAE: return format_to(ctx.out(), "YAE");
        case unicode::Jamo_Short_Name::YE: return format_to(ctx.out(), "YE");
        case unicode::Jamo_Short_Name::YEO: return format_to(ctx.out(), "YEO");
        case unicode::Jamo_Short_Name::YI: return format_to(ctx.out(), "YI");
        case unicode::Jamo_Short_Name::YO: return format_to(ctx.out(), "YO");
        case unicode::Jamo_Short_Name::YU: return format_to(ctx.out(), "YU");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Joining_Group>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Joining_Group _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Joining_Group::African_Feh: return format_to(ctx.out(), "African_Feh");
        case unicode::Joining_Group::African_Noon: return format_to(ctx.out(), "African_Noon");
        case unicode::Joining_Group::African_Qaf: return format_to(ctx.out(), "African_Qaf");
        case unicode::Joining_Group::Ain: return format_to(ctx.out(), "Ain");
        case unicode::Joining_Group::Alaph: return format_to(ctx.out(), "Alaph");
        case unicode::Joining_Group::Alef: return format_to(ctx.out(), "Alef");
        case unicode::Joining_Group::Beh: return format_to(ctx.out(), "Beh");
        case unicode::Joining_Group::Beth: return format_to(ctx.out(), "Beth");
        case unicode::Joining_Group::Burushaski_Yeh_Barree: return format_to(ctx.out(), "Burushaski_Yeh_Barree");
        case unicode::Joining_Group::Dal: return format_to(ctx.out(), "Dal");
        case unicode::Joining_Group::Dalath_Rish: return format_to(ctx.out(), "Dalath_Rish");
        case unicode::Joining_Group::E: return format_to(ctx.out(), "E");
        case unicode::Joining_Group::Farsi_Yeh: return format_to(ctx.out(), "Farsi_Yeh");
        case unicode::Joining_Group::Fe: return format_to(ctx.out(), "Fe");
        case unicode::Joining_Group::Feh: return format_to(ctx.out(), "Feh");
        case unicode::Joining_Group::Final_Semkath: return format_to(ctx.out(), "Final_Semkath");
        case unicode::Joining_Group::Gaf: return format_to(ctx.out(), "Gaf");
        case unicode::Joining_Group::Gamal: return format_to(ctx.out(), "Gamal");
        case unicode::Joining_Group::Hah: return format_to(ctx.out(), "Hah");
        case unicode::Joining_Group::Hamza_On_Heh_Goal: return format_to(ctx.out(), "Hamza_On_Heh_Goal");
        case unicode::Joining_Group::Hanifi_Rohingya_Kinna_Ya: return format_to(ctx.out(), "Hanifi_Rohingya_Kinna_Ya");
        case unicode::Joining_Group::Hanifi_Rohingya_Pa: return format_to(ctx.out(), "Hanifi_Rohingya_Pa");
        case unicode::Joining_Group::He: return format_to(ctx.out(), "He");
        case unicode::Joining_Group::Heh: return format_to(ctx.out(), "Heh");
        case unicode::Joining_Group::Heh_Goal: return format_to(ctx.out(), "Heh_Goal");
        case unicode::Joining_Group::Heth: return format_to(ctx.out(), "Heth");
        case unicode::Joining_Group::Kaf: return format_to(ctx.out(), "Kaf");
        case unicode::Joining_Group::Kaph: return format_to(ctx.out(), "Kaph");
        case unicode::Joining_Group::Khaph: return format_to(ctx.out(), "Khaph");
        case unicode::Joining_Group::Knotted_Heh: return format_to(ctx.out(), "Knotted_Heh");
        case unicode::Joining_Group::Lam: return format_to(ctx.out(), "Lam");
        case unicode::Joining_Group::Lamadh: return format_to(ctx.out(), "Lamadh");
        case unicode::Joining_Group::Malayalam_Bha: return format_to(ctx.out(), "Malayalam_Bha");
        case unicode::Joining_Group::Malayalam_Ja: return format_to(ctx.out(), "Malayalam_Ja");
        case unicode::Joining_Group::Malayalam_Lla: return format_to(ctx.out(), "Malayalam_Lla");
        case unicode::Joining_Group::Malayalam_Llla: return format_to(ctx.out(), "Malayalam_Llla");
        case unicode::Joining_Group::Malayalam_Nga: return format_to(ctx.out(), "Malayalam_Nga");
        case unicode::Joining_Group::Malayalam_Nna: return format_to(ctx.out(), "Malayalam_Nna");
        case unicode::Joining_Group::Malayalam_Nnna: return format_to(ctx.out(), "Malayalam_Nnna");
        case unicode::Joining_Group::Malayalam_Nya: return format_to(ctx.out(), "Malayalam_Nya");
        case unicode::Joining_Group::Malayalam_Ra: return format_to(ctx.out(), "Malayalam_Ra");
        case unicode::Joining_Group::Malayalam_Ssa: return format_to(ctx.out(), "Malayalam_Ssa");
        case unicode::Joining_Group::Malayalam_Tta: return format_to(ctx.out(), "Malayalam_Tta");
        case unicode::Joining_Group::Manichaean_Aleph: return format_to(ctx.out(), "Manichaean_Aleph");
        case unicode::Joining_Group::Manichaean_Ayin: return format_to(ctx.out(), "Manichaean_Ayin");
        case unicode::Joining_Group::Manichaean_Beth: return format_to(ctx.out(), "Manichaean_Beth");
        case unicode::Joining_Group::Manichaean_Daleth: return format_to(ctx.out(), "Manichaean_Daleth");
        case unicode::Joining_Group::Manichaean_Dhamedh: return format_to(ctx.out(), "Manichaean_Dhamedh");
        case unicode::Joining_Group::Manichaean_Five: return format_to(ctx.out(), "Manichaean_Five");
        case unicode::Joining_Group::Manichaean_Gimel: return format_to(ctx.out(), "Manichaean_Gimel");
        case unicode::Joining_Group::Manichaean_Heth: return format_to(ctx.out(), "Manichaean_Heth");
        case unicode::Joining_Group::Manichaean_Hundred: return format_to(ctx.out(), "Manichaean_Hundred");
        case unicode::Joining_Group::Manichaean_Kaph: return format_to(ctx.out(), "Manichaean_Kaph");
        case unicode::Joining_Group::Manichaean_Lamedh: return format_to(ctx.out(), "Manichaean_Lamedh");
        case unicode::Joining_Group::Manichaean_Mem: return format_to(ctx.out(), "Manichaean_Mem");
        case unicode::Joining_Group::Manichaean_Nun: return format_to(ctx.out(), "Manichaean_Nun");
        case unicode::Joining_Group::Manichaean_One: return format_to(ctx.out(), "Manichaean_One");
        case unicode::Joining_Group::Manichaean_Pe: return format_to(ctx.out(), "Manichaean_Pe");
        case unicode::Joining_Group::Manichaean_Qoph: return format_to(ctx.out(), "Manichaean_Qoph");
        case unicode::Joining_Group::Manichaean_Resh: return format_to(ctx.out(), "Manichaean_Resh");
        case unicode::Joining_Group::Manichaean_Sadhe: return format_to(ctx.out(), "Manichaean_Sadhe");
        case unicode::Joining_Group::Manichaean_Samekh: return format_to(ctx.out(), "Manichaean_Samekh");
        case unicode::Joining_Group::Manichaean_Taw: return format_to(ctx.out(), "Manichaean_Taw");
        case unicode::Joining_Group::Manichaean_Ten: return format_to(ctx.out(), "Manichaean_Ten");
        case unicode::Joining_Group::Manichaean_Teth: return format_to(ctx.out(), "Manichaean_Teth");
        case unicode::Joining_Group::Manichaean_Thamedh: return format_to(ctx.out(), "Manichaean_Thamedh");
        case unicode::Joining_Group::Manichaean_Twenty: return format_to(ctx.out(), "Manichaean_Twenty");
        case unicode::Joining_Group::Manichaean_Waw: return format_to(ctx.out(), "Manichaean_Waw");
        case unicode::Joining_Group::Manichaean_Yodh: return format_to(ctx.out(), "Manichaean_Yodh");
        case unicode::Joining_Group::Manichaean_Zayin: return format_to(ctx.out(), "Manichaean_Zayin");
        case unicode::Joining_Group::Meem: return format_to(ctx.out(), "Meem");
        case unicode::Joining_Group::Mim: return format_to(ctx.out(), "Mim");
        case unicode::Joining_Group::No_Joining_Group: return format_to(ctx.out(), "No_Joining_Group");
        case unicode::Joining_Group::Noon: return format_to(ctx.out(), "Noon");
        case unicode::Joining_Group::Nun: return format_to(ctx.out(), "Nun");
        case unicode::Joining_Group::Nya: return format_to(ctx.out(), "Nya");
        case unicode::Joining_Group::Pe: return format_to(ctx.out(), "Pe");
        case unicode::Joining_Group::Qaf: return format_to(ctx.out(), "Qaf");
        case unicode::Joining_Group::Qaph: return format_to(ctx.out(), "Qaph");
        case unicode::Joining_Group::Reh: return format_to(ctx.out(), "Reh");
        case unicode::Joining_Group::Reversed_Pe: return format_to(ctx.out(), "Reversed_Pe");
        case unicode::Joining_Group::Rohingya_Yeh: return format_to(ctx.out(), "Rohingya_Yeh");
        case unicode::Joining_Group::Sad: return format_to(ctx.out(), "Sad");
        case unicode::Joining_Group::Sadhe: return format_to(ctx.out(), "Sadhe");
        case unicode::Joining_Group::Seen: return format_to(ctx.out(), "Seen");
        case unicode::Joining_Group::Semkath: return format_to(ctx.out(), "Semkath");
        case unicode::Joining_Group::Shin: return format_to(ctx.out(), "Shin");
        case unicode::Joining_Group::Straight_Waw: return format_to(ctx.out(), "Straight_Waw");
        case unicode::Joining_Group::Swash_Kaf: return format_to(ctx.out(), "Swash_Kaf");
        case unicode::Joining_Group::Syriac_Waw: return format_to(ctx.out(), "Syriac_Waw");
        case unicode::Joining_Group::Tah: return format_to(ctx.out(), "Tah");
        case unicode::Joining_Group::Taw: return format_to(ctx.out(), "Taw");
        case unicode::Joining_Group::Teh_Marbuta: return format_to(ctx.out(), "Teh_Marbuta");
        case unicode::Joining_Group::Teth: return format_to(ctx.out(), "Teth");
        case unicode::Joining_Group::Waw: return format_to(ctx.out(), "Waw");
        case unicode::Joining_Group::Yeh: return format_to(ctx.out(), "Yeh");
        case unicode::Joining_Group::Yeh_Barree: return format_to(ctx.out(), "Yeh_Barree");
        case unicode::Joining_Group::Yeh_With_Tail: return format_to(ctx.out(), "Yeh_With_Tail");
        case unicode::Joining_Group::Yudh: return format_to(ctx.out(), "Yudh");
        case unicode::Joining_Group::Yudh_He: return format_to(ctx.out(), "Yudh_He");
        case unicode::Joining_Group::Zain: return format_to(ctx.out(), "Zain");
        case unicode::Joining_Group::Zhain: return format_to(ctx.out(), "Zhain");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Joining_Type>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Joining_Type _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Joining_Type::Dual_Joining: return format_to(ctx.out(), "Dual_Joining");
        case unicode::Joining_Type::Join_Causing: return format_to(ctx.out(), "Join_Causing");
        case unicode::Joining_Type::Left_Joining: return format_to(ctx.out(), "Left_Joining");
        case unicode::Joining_Type::Non_Joining: return format_to(ctx.out(), "Non_Joining");
        case unicode::Joining_Type::Right_Joining: return format_to(ctx.out(), "Right_Joining");
        case unicode::Joining_Type::Transparent: return format_to(ctx.out(), "Transparent");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Line_Break>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Line_Break _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Line_Break::Alphabetic: return format_to(ctx.out(), "Alphabetic");
        case unicode::Line_Break::Ambiguous: return format_to(ctx.out(), "Ambiguous");
        case unicode::Line_Break::Break_After: return format_to(ctx.out(), "Break_After");
        case unicode::Line_Break::Break_Before: return format_to(ctx.out(), "Break_Before");
        case unicode::Line_Break::Break_Both: return format_to(ctx.out(), "Break_Both");
        case unicode::Line_Break::Break_Symbols: return format_to(ctx.out(), "Break_Symbols");
        case unicode::Line_Break::Carriage_Return: return format_to(ctx.out(), "Carriage_Return");
        case unicode::Line_Break::Close_Parenthesis: return format_to(ctx.out(), "Close_Parenthesis");
        case unicode::Line_Break::Close_Punctuation: return format_to(ctx.out(), "Close_Punctuation");
        case unicode::Line_Break::Combining_Mark: return format_to(ctx.out(), "Combining_Mark");
        case unicode::Line_Break::Complex_Context: return format_to(ctx.out(), "Complex_Context");
        case unicode::Line_Break::Conditional_Japanese_Starter: return format_to(ctx.out(), "Conditional_Japanese_Starter");
        case unicode::Line_Break::Contingent_Break: return format_to(ctx.out(), "Contingent_Break");
        case unicode::Line_Break::E_Base: return format_to(ctx.out(), "E_Base");
        case unicode::Line_Break::E_Modifier: return format_to(ctx.out(), "E_Modifier");
        case unicode::Line_Break::Exclamation: return format_to(ctx.out(), "Exclamation");
        case unicode::Line_Break::Glue: return format_to(ctx.out(), "Glue");
        case unicode::Line_Break::H2: return format_to(ctx.out(), "H2");
        case unicode::Line_Break::H3: return format_to(ctx.out(), "H3");
        case unicode::Line_Break::Hebrew_Letter: return format_to(ctx.out(), "Hebrew_Letter");
        case unicode::Line_Break::Hyphen: return format_to(ctx.out(), "Hyphen");
        case unicode::Line_Break::Ideographic: return format_to(ctx.out(), "Ideographic");
        case unicode::Line_Break::Infix_Numeric: return format_to(ctx.out(), "Infix_Numeric");
        case unicode::Line_Break::Inseparable: return format_to(ctx.out(), "Inseparable");
        case unicode::Line_Break::JL: return format_to(ctx.out(), "JL");
        case unicode::Line_Break::JT: return format_to(ctx.out(), "JT");
        case unicode::Line_Break::JV: return format_to(ctx.out(), "JV");
        case unicode::Line_Break::Line_Feed: return format_to(ctx.out(), "Line_Feed");
        case unicode::Line_Break::Mandatory_Break: return format_to(ctx.out(), "Mandatory_Break");
        case unicode::Line_Break::Next_Line: return format_to(ctx.out(), "Next_Line");
        case unicode::Line_Break::Nonstarter: return format_to(ctx.out(), "Nonstarter");
        case unicode::Line_Break::Numeric: return format_to(ctx.out(), "Numeric");
        case unicode::Line_Break::Open_Punctuation: return format_to(ctx.out(), "Open_Punctuation");
        case unicode::Line_Break::Postfix_Numeric: return format_to(ctx.out(), "Postfix_Numeric");
        case unicode::Line_Break::Prefix_Numeric: return format_to(ctx.out(), "Prefix_Numeric");
        case unicode::Line_Break::Quotation: return format_to(ctx.out(), "Quotation");
        case unicode::Line_Break::Regional_Indicator: return format_to(ctx.out(), "Regional_Indicator");
        case unicode::Line_Break::Space: return format_to(ctx.out(), "Space");
        case unicode::Line_Break::Surrogate: return format_to(ctx.out(), "Surrogate");
        case unicode::Line_Break::Unknown: return format_to(ctx.out(), "Unknown");
        case unicode::Line_Break::Word_Joiner: return format_to(ctx.out(), "Word_Joiner");
        case unicode::Line_Break::ZWJ: return format_to(ctx.out(), "ZWJ");
        case unicode::Line_Break::ZWSpace: return format_to(ctx.out(), "ZWSpace");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::NFC_Quick_Check>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::NFC_Quick_Check _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::NFC_Quick_Check::Maybe: return format_to(ctx.out(), "Maybe");
        case unicode::NFC_Quick_Check::No: return format_to(ctx.out(), "No");
        case unicode::NFC_Quick_Check::Yes: return format_to(ctx.out(), "Yes");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::NFKC_Quick_Check>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::NFKC_Quick_Check _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::NFKC_Quick_Check::Maybe: return format_to(ctx.out(), "Maybe");
        case unicode::NFKC_Quick_Check::No: return format_to(ctx.out(), "No");
        case unicode::NFKC_Quick_Check::Yes: return format_to(ctx.out(), "Yes");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Numeric_Type>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Numeric_Type _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Numeric_Type::Decimal: return format_to(ctx.out(), "Decimal");
        case unicode::Numeric_Type::Digit: return format_to(ctx.out(), "Digit");
        case unicode::Numeric_Type::None: return format_to(ctx.out(), "None");
        case unicode::Numeric_Type::Numeric: return format_to(ctx.out(), "Numeric");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Sentence_Break>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Sentence_Break _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Sentence_Break::ATerm: return format_to(ctx.out(), "ATerm");
        case unicode::Sentence_Break::CR: return format_to(ctx.out(), "CR");
        case unicode::Sentence_Break::Close: return format_to(ctx.out(), "Close");
        case unicode::Sentence_Break::Extend: return format_to(ctx.out(), "Extend");
        case unicode::Sentence_Break::Format: return format_to(ctx.out(), "Format");
        case unicode::Sentence_Break::LF: return format_to(ctx.out(), "LF");
        case unicode::Sentence_Break::Lower: return format_to(ctx.out(), "Lower");
        case unicode::Sentence_Break::Numeric: return format_to(ctx.out(), "Numeric");
        case unicode::Sentence_Break::OLetter: return format_to(ctx.out(), "OLetter");
        case unicode::Sentence_Break::Other: return format_to(ctx.out(), "Other");
        case unicode::Sentence_Break::SContinue: return format_to(ctx.out(), "SContinue");
        case unicode::Sentence_Break::STerm: return format_to(ctx.out(), "STerm");
        case unicode::Sentence_Break::Sep: return format_to(ctx.out(), "Sep");
        case unicode::Sentence_Break::Sp: return format_to(ctx.out(), "Sp");
        case unicode::Sentence_Break::Upper: return format_to(ctx.out(), "Upper");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Vertical_Orientation>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Vertical_Orientation _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Vertical_Orientation::Rotated: return format_to(ctx.out(), "Rotated");
        case unicode::Vertical_Orientation::Transformed_Rotated: return format_to(ctx.out(), "Transformed_Rotated");
        case unicode::Vertical_Orientation::Transformed_Upright: return format_to(ctx.out(), "Transformed_Upright");
        case unicode::Vertical_Orientation::Upright: return format_to(ctx.out(), "Upright");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Word_Break>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Word_Break _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Word_Break::ALetter: return format_to(ctx.out(), "ALetter");
        case unicode::Word_Break::CR: return format_to(ctx.out(), "CR");
        case unicode::Word_Break::Double_Quote: return format_to(ctx.out(), "Double_Quote");
        case unicode::Word_Break::E_Base: return format_to(ctx.out(), "E_Base");
        case unicode::Word_Break::E_Base_GAZ: return format_to(ctx.out(), "E_Base_GAZ");
        case unicode::Word_Break::E_Modifier: return format_to(ctx.out(), "E_Modifier");
        case unicode::Word_Break::Extend: return format_to(ctx.out(), "Extend");
        case unicode::Word_Break::ExtendNumLet: return format_to(ctx.out(), "ExtendNumLet");
        case unicode::Word_Break::Format: return format_to(ctx.out(), "Format");
        case unicode::Word_Break::Glue_After_Zwj: return format_to(ctx.out(), "Glue_After_Zwj");
        case unicode::Word_Break::Hebrew_Letter: return format_to(ctx.out(), "Hebrew_Letter");
        case unicode::Word_Break::Katakana: return format_to(ctx.out(), "Katakana");
        case unicode::Word_Break::LF: return format_to(ctx.out(), "LF");
        case unicode::Word_Break::MidLetter: return format_to(ctx.out(), "MidLetter");
        case unicode::Word_Break::MidNum: return format_to(ctx.out(), "MidNum");
        case unicode::Word_Break::MidNumLet: return format_to(ctx.out(), "MidNumLet");
        case unicode::Word_Break::Newline: return format_to(ctx.out(), "Newline");
        case unicode::Word_Break::Numeric: return format_to(ctx.out(), "Numeric");
        case unicode::Word_Break::Other: return format_to(ctx.out(), "Other");
        case unicode::Word_Break::Regional_Indicator: return format_to(ctx.out(), "Regional_Indicator");
        case unicode::Word_Break::Single_Quote: return format_to(ctx.out(), "Single_Quote");
        case unicode::Word_Break::WSegSpace: return format_to(ctx.out(), "WSegSpace");
        case unicode::Word_Break::ZWJ: return format_to(ctx.out(), "ZWJ");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Core_Property>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Core_Property _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Core_Property::Alphabetic: return format_to(ctx.out(), "Alphabetic");
        case unicode::Core_Property::Case_Ignorable: return format_to(ctx.out(), "Case_Ignorable");
        case unicode::Core_Property::Cased: return format_to(ctx.out(), "Cased");
        case unicode::Core_Property::Changes_When_Casefolded: return format_to(ctx.out(), "Changes_When_Casefolded");
        case unicode::Core_Property::Changes_When_Casemapped: return format_to(ctx.out(), "Changes_When_Casemapped");
        case unicode::Core_Property::Changes_When_Lowercased: return format_to(ctx.out(), "Changes_When_Lowercased");
        case unicode::Core_Property::Changes_When_Titlecased: return format_to(ctx.out(), "Changes_When_Titlecased");
        case unicode::Core_Property::Changes_When_Uppercased: return format_to(ctx.out(), "Changes_When_Uppercased");
        case unicode::Core_Property::Default_Ignorable_Code_Point: return format_to(ctx.out(), "Default_Ignorable_Code_Point");
        case unicode::Core_Property::Grapheme_Base: return format_to(ctx.out(), "Grapheme_Base");
        case unicode::Core_Property::Grapheme_Extend: return format_to(ctx.out(), "Grapheme_Extend");
        case unicode::Core_Property::Grapheme_Link: return format_to(ctx.out(), "Grapheme_Link");
        case unicode::Core_Property::ID_Continue: return format_to(ctx.out(), "ID_Continue");
        case unicode::Core_Property::ID_Start: return format_to(ctx.out(), "ID_Start");
        case unicode::Core_Property::Lowercase: return format_to(ctx.out(), "Lowercase");
        case unicode::Core_Property::Math: return format_to(ctx.out(), "Math");
        case unicode::Core_Property::Uppercase: return format_to(ctx.out(), "Uppercase");
        case unicode::Core_Property::XID_Continue: return format_to(ctx.out(), "XID_Continue");
        case unicode::Core_Property::XID_Start: return format_to(ctx.out(), "XID_Start");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::General_Category>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::General_Category _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::General_Category::Unspecified: return format_to(ctx.out(), "Unspecified");
        case unicode::General_Category::Close_Punctuation: return format_to(ctx.out(), "Close_Punctuation");
        case unicode::General_Category::Connector_Punctuation: return format_to(ctx.out(), "Connector_Punctuation");
        case unicode::General_Category::Control: return format_to(ctx.out(), "Control");
        case unicode::General_Category::Currency_Symbol: return format_to(ctx.out(), "Currency_Symbol");
        case unicode::General_Category::Dash_Punctuation: return format_to(ctx.out(), "Dash_Punctuation");
        case unicode::General_Category::Decimal_Number: return format_to(ctx.out(), "Decimal_Number");
        case unicode::General_Category::Enclosing_Mark: return format_to(ctx.out(), "Enclosing_Mark");
        case unicode::General_Category::Final_Punctuation: return format_to(ctx.out(), "Final_Punctuation");
        case unicode::General_Category::Format: return format_to(ctx.out(), "Format");
        case unicode::General_Category::Initial_Punctuation: return format_to(ctx.out(), "Initial_Punctuation");
        case unicode::General_Category::Letter_Number: return format_to(ctx.out(), "Letter_Number");
        case unicode::General_Category::Line_Separator: return format_to(ctx.out(), "Line_Separator");
        case unicode::General_Category::Lowercase_Letter: return format_to(ctx.out(), "Lowercase_Letter");
        case unicode::General_Category::Math_Symbol: return format_to(ctx.out(), "Math_Symbol");
        case unicode::General_Category::Modifier_Letter: return format_to(ctx.out(), "Modifier_Letter");
        case unicode::General_Category::Modifier_Symbol: return format_to(ctx.out(), "Modifier_Symbol");
        case unicode::General_Category::Nonspacing_Mark: return format_to(ctx.out(), "Nonspacing_Mark");
        case unicode::General_Category::Open_Punctuation: return format_to(ctx.out(), "Open_Punctuation");
        case unicode::General_Category::Other_Letter: return format_to(ctx.out(), "Other_Letter");
        case unicode::General_Category::Other_Number: return format_to(ctx.out(), "Other_Number");
        case unicode::General_Category::Other_Punctuation: return format_to(ctx.out(), "Other_Punctuation");
        case unicode::General_Category::Other_Symbol: return format_to(ctx.out(), "Other_Symbol");
        case unicode::General_Category::Paragraph_Separator: return format_to(ctx.out(), "Paragraph_Separator");
        case unicode::General_Category::Private_Use: return format_to(ctx.out(), "Private_Use");
        case unicode::General_Category::Space_Separator: return format_to(ctx.out(), "Space_Separator");
        case unicode::General_Category::Spacing_Mark: return format_to(ctx.out(), "Spacing_Mark");
        case unicode::General_Category::Surrogate: return format_to(ctx.out(), "Surrogate");
        case unicode::General_Category::Titlecase_Letter: return format_to(ctx.out(), "Titlecase_Letter");
        case unicode::General_Category::Unassigned: return format_to(ctx.out(), "Unassigned");
        case unicode::General_Category::Uppercase_Letter: return format_to(ctx.out(), "Uppercase_Letter");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Script>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Script _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Script::Invalid: return format_to(ctx.out(), "Invalid");
        case unicode::Script::Unknown: return format_to(ctx.out(), "Unknown");
        case unicode::Script::Common: return format_to(ctx.out(), "Common");
        case unicode::Script::Adlam: return format_to(ctx.out(), "Adlam");
        case unicode::Script::Ahom: return format_to(ctx.out(), "Ahom");
        case unicode::Script::Anatolian_Hieroglyphs: return format_to(ctx.out(), "Anatolian_Hieroglyphs");
        case unicode::Script::Arabic: return format_to(ctx.out(), "Arabic");
        case unicode::Script::Armenian: return format_to(ctx.out(), "Armenian");
        case unicode::Script::Avestan: return format_to(ctx.out(), "Avestan");
        case unicode::Script::Balinese: return format_to(ctx.out(), "Balinese");
        case unicode::Script::Bamum: return format_to(ctx.out(), "Bamum");
        case unicode::Script::Bassa_Vah: return format_to(ctx.out(), "Bassa_Vah");
        case unicode::Script::Batak: return format_to(ctx.out(), "Batak");
        case unicode::Script::Bengali: return format_to(ctx.out(), "Bengali");
        case unicode::Script::Bhaiksuki: return format_to(ctx.out(), "Bhaiksuki");
        case unicode::Script::Bopomofo: return format_to(ctx.out(), "Bopomofo");
        case unicode::Script::Brahmi: return format_to(ctx.out(), "Brahmi");
        case unicode::Script::Braille: return format_to(ctx.out(), "Braille");
        case unicode::Script::Buginese: return format_to(ctx.out(), "Buginese");
        case unicode::Script::Buhid: return format_to(ctx.out(), "Buhid");
        case unicode::Script::Canadian_Aboriginal: return format_to(ctx.out(), "Canadian_Aboriginal");
        case unicode::Script::Carian: return format_to(ctx.out(), "Carian");
        case unicode::Script::Caucasian_Albanian: return format_to(ctx.out(), "Caucasian_Albanian");
        case unicode::Script::Chakma: return format_to(ctx.out(), "Chakma");
        case unicode::Script::Cham: return format_to(ctx.out(), "Cham");
        case unicode::Script::Cherokee: return format_to(ctx.out(), "Cherokee");
        case unicode::Script::Chorasmian: return format_to(ctx.out(), "Chorasmian");
        case unicode::Script::Coptic: return format_to(ctx.out(), "Coptic");
        case unicode::Script::Cuneiform: return format_to(ctx.out(), "Cuneiform");
        case unicode::Script::Cypriot: return format_to(ctx.out(), "Cypriot");
        case unicode::Script::Cyrillic: return format_to(ctx.out(), "Cyrillic");
        case unicode::Script::Deseret: return format_to(ctx.out(), "Deseret");
        case unicode::Script::Devanagari: return format_to(ctx.out(), "Devanagari");
        case unicode::Script::Dives_Akuru: return format_to(ctx.out(), "Dives_Akuru");
        case unicode::Script::Dogra: return format_to(ctx.out(), "Dogra");
        case unicode::Script::Duployan: return format_to(ctx.out(), "Duployan");
        case unicode::Script::Egyptian_Hieroglyphs: return format_to(ctx.out(), "Egyptian_Hieroglyphs");
        case unicode::Script::Elbasan: return format_to(ctx.out(), "Elbasan");
        case unicode::Script::Elymaic: return format_to(ctx.out(), "Elymaic");
        case unicode::Script::Ethiopic: return format_to(ctx.out(), "Ethiopic");
        case unicode::Script::Georgian: return format_to(ctx.out(), "Georgian");
        case unicode::Script::Glagolitic: return format_to(ctx.out(), "Glagolitic");
        case unicode::Script::Gothic: return format_to(ctx.out(), "Gothic");
        case unicode::Script::Grantha: return format_to(ctx.out(), "Grantha");
        case unicode::Script::Greek: return format_to(ctx.out(), "Greek");
        case unicode::Script::Gujarati: return format_to(ctx.out(), "Gujarati");
        case unicode::Script::Gunjala_Gondi: return format_to(ctx.out(), "Gunjala_Gondi");
        case unicode::Script::Gurmukhi: return format_to(ctx.out(), "Gurmukhi");
        case unicode::Script::Han: return format_to(ctx.out(), "Han");
        case unicode::Script::Hangul: return format_to(ctx.out(), "Hangul");
        case unicode::Script::Hanifi_Rohingya: return format_to(ctx.out(), "Hanifi_Rohingya");
        case unicode::Script::Hanunoo: return format_to(ctx.out(), "Hanunoo");
        case unicode::Script::Hatran: return format_to(ctx.out(), "Hatran");
        case unicode::Script::Hebrew: return format_to(ctx.out(), "Hebrew");
        case unicode::Script::Hiragana: return format_to(ctx.out(), "Hiragana");
        case unicode::Script::Imperial_Aramaic: return format_to(ctx.out(), "Imperial_Aramaic");
        case unicode::Script::Inherited: return format_to(ctx.out(), "Inherited");
        case unicode::Script::Inscriptional_Pahlavi: return format_to(ctx.out(), "Inscriptional_Pahlavi");
        case unicode::Script::Inscriptional_Parthian: return format_to(ctx.out(), "Inscriptional_Parthian");
        case unicode::Script::Javanese: return format_to(ctx.out(), "Javanese");
        case unicode::Script::Kaithi: return format_to(ctx.out(), "Kaithi");
        case unicode::Script::Kannada: return format_to(ctx.out(), "Kannada");
        case unicode::Script::Katakana: return format_to(ctx.out(), "Katakana");
        case unicode::Script::Kayah_Li: return format_to(ctx.out(), "Kayah_Li");
        case unicode::Script::Kharoshthi: return format_to(ctx.out(), "Kharoshthi");
        case unicode::Script::Khitan_Small_Script: return format_to(ctx.out(), "Khitan_Small_Script");
        case unicode::Script::Khmer: return format_to(ctx.out(), "Khmer");
        case unicode::Script::Khojki: return format_to(ctx.out(), "Khojki");
        case unicode::Script::Khudawadi: return format_to(ctx.out(), "Khudawadi");
        case unicode::Script::Lao: return format_to(ctx.out(), "Lao");
        case unicode::Script::Latin: return format_to(ctx.out(), "Latin");
        case unicode::Script::Lepcha: return format_to(ctx.out(), "Lepcha");
        case unicode::Script::Limbu: return format_to(ctx.out(), "Limbu");
        case unicode::Script::Linear_A: return format_to(ctx.out(), "Linear_A");
        case unicode::Script::Linear_B: return format_to(ctx.out(), "Linear_B");
        case unicode::Script::Lisu: return format_to(ctx.out(), "Lisu");
        case unicode::Script::Lycian: return format_to(ctx.out(), "Lycian");
        case unicode::Script::Lydian: return format_to(ctx.out(), "Lydian");
        case unicode::Script::Mahajani: return format_to(ctx.out(), "Mahajani");
        case unicode::Script::Makasar: return format_to(ctx.out(), "Makasar");
        case unicode::Script::Malayalam: return format_to(ctx.out(), "Malayalam");
        case unicode::Script::Mandaic: return format_to(ctx.out(), "Mandaic");
        case unicode::Script::Manichaean: return format_to(ctx.out(), "Manichaean");
        case unicode::Script::Marchen: return format_to(ctx.out(), "Marchen");
        case unicode::Script::Masaram_Gondi: return format_to(ctx.out(), "Masaram_Gondi");
        case unicode::Script::Medefaidrin: return format_to(ctx.out(), "Medefaidrin");
        case unicode::Script::Meetei_Mayek: return format_to(ctx.out(), "Meetei_Mayek");
        case unicode::Script::Mende_Kikakui: return format_to(ctx.out(), "Mende_Kikakui");
        case unicode::Script::Meroitic_Cursive: return format_to(ctx.out(), "Meroitic_Cursive");
        case unicode::Script::Meroitic_Hieroglyphs: return format_to(ctx.out(), "Meroitic_Hieroglyphs");
        case unicode::Script::Miao: return format_to(ctx.out(), "Miao");
        case unicode::Script::Modi: return format_to(ctx.out(), "Modi");
        case unicode::Script::Mongolian: return format_to(ctx.out(), "Mongolian");
        case unicode::Script::Mro: return format_to(ctx.out(), "Mro");
        case unicode::Script::Multani: return format_to(ctx.out(), "Multani");
        case unicode::Script::Myanmar: return format_to(ctx.out(), "Myanmar");
        case unicode::Script::Nabataean: return format_to(ctx.out(), "Nabataean");
        case unicode::Script::Nandinagari: return format_to(ctx.out(), "Nandinagari");
        case unicode::Script::New_Tai_Lue: return format_to(ctx.out(), "New_Tai_Lue");
        case unicode::Script::Newa: return format_to(ctx.out(), "Newa");
        case unicode::Script::Nko: return format_to(ctx.out(), "Nko");
        case unicode::Script::Nushu: return format_to(ctx.out(), "Nushu");
        case unicode::Script::Nyiakeng_Puachue_Hmong: return format_to(ctx.out(), "Nyiakeng_Puachue_Hmong");
        case unicode::Script::Ogham: return format_to(ctx.out(), "Ogham");
        case unicode::Script::Ol_Chiki: return format_to(ctx.out(), "Ol_Chiki");
        case unicode::Script::Old_Hungarian: return format_to(ctx.out(), "Old_Hungarian");
        case unicode::Script::Old_Italic: return format_to(ctx.out(), "Old_Italic");
        case unicode::Script::Old_North_Arabian: return format_to(ctx.out(), "Old_North_Arabian");
        case unicode::Script::Old_Permic: return format_to(ctx.out(), "Old_Permic");
        case unicode::Script::Old_Persian: return format_to(ctx.out(), "Old_Persian");
        case unicode::Script::Old_Sogdian: return format_to(ctx.out(), "Old_Sogdian");
        case unicode::Script::Old_South_Arabian: return format_to(ctx.out(), "Old_South_Arabian");
        case unicode::Script::Old_Turkic: return format_to(ctx.out(), "Old_Turkic");
        case unicode::Script::Oriya: return format_to(ctx.out(), "Oriya");
        case unicode::Script::Osage: return format_to(ctx.out(), "Osage");
        case unicode::Script::Osmanya: return format_to(ctx.out(), "Osmanya");
        case unicode::Script::Pahawh_Hmong: return format_to(ctx.out(), "Pahawh_Hmong");
        case unicode::Script::Palmyrene: return format_to(ctx.out(), "Palmyrene");
        case unicode::Script::Pau_Cin_Hau: return format_to(ctx.out(), "Pau_Cin_Hau");
        case unicode::Script::Phags_Pa: return format_to(ctx.out(), "Phags_Pa");
        case unicode::Script::Phoenician: return format_to(ctx.out(), "Phoenician");
        case unicode::Script::Psalter_Pahlavi: return format_to(ctx.out(), "Psalter_Pahlavi");
        case unicode::Script::Rejang: return format_to(ctx.out(), "Rejang");
        case unicode::Script::Runic: return format_to(ctx.out(), "Runic");
        case unicode::Script::Samaritan: return format_to(ctx.out(), "Samaritan");
        case unicode::Script::Saurashtra: return format_to(ctx.out(), "Saurashtra");
        case unicode::Script::Sharada: return format_to(ctx.out(), "Sharada");
        case unicode::Script::Shavian: return format_to(ctx.out(), "Shavian");
        case unicode::Script::Siddham: return format_to(ctx.out(), "Siddham");
        case unicode::Script::SignWriting: return format_to(ctx.out(), "SignWriting");
        case unicode::Script::Sinhala: return format_to(ctx.out(), "Sinhala");
        case unicode::Script::Sogdian: return format_to(ctx.out(), "Sogdian");
        case unicode::Script::Sora_Sompeng: return format_to(ctx.out(), "Sora_Sompeng");
        case unicode::Script::Soyombo: return format_to(ctx.out(), "Soyombo");
        case unicode::Script::Sundanese: return format_to(ctx.out(), "Sundanese");
        case unicode::Script::Syloti_Nagri: return format_to(ctx.out(), "Syloti_Nagri");
        case unicode::Script::Syriac: return format_to(ctx.out(), "Syriac");
        case unicode::Script::Tagalog: return format_to(ctx.out(), "Tagalog");
        case unicode::Script::Tagbanwa: return format_to(ctx.out(), "Tagbanwa");
        case unicode::Script::Tai_Le: return format_to(ctx.out(), "Tai_Le");
        case unicode::Script::Tai_Tham: return format_to(ctx.out(), "Tai_Tham");
        case unicode::Script::Tai_Viet: return format_to(ctx.out(), "Tai_Viet");
        case unicode::Script::Takri: return format_to(ctx.out(), "Takri");
        case unicode::Script::Tamil: return format_to(ctx.out(), "Tamil");
        case unicode::Script::Tangut: return format_to(ctx.out(), "Tangut");
        case unicode::Script::Telugu: return format_to(ctx.out(), "Telugu");
        case unicode::Script::Thaana: return format_to(ctx.out(), "Thaana");
        case unicode::Script::Thai: return format_to(ctx.out(), "Thai");
        case unicode::Script::Tibetan: return format_to(ctx.out(), "Tibetan");
        case unicode::Script::Tifinagh: return format_to(ctx.out(), "Tifinagh");
        case unicode::Script::Tirhuta: return format_to(ctx.out(), "Tirhuta");
        case unicode::Script::Ugaritic: return format_to(ctx.out(), "Ugaritic");
        case unicode::Script::Vai: return format_to(ctx.out(), "Vai");
        case unicode::Script::Wancho: return format_to(ctx.out(), "Wancho");
        case unicode::Script::Warang_Citi: return format_to(ctx.out(), "Warang_Citi");
        case unicode::Script::Yezidi: return format_to(ctx.out(), "Yezidi");
        case unicode::Script::Yi: return format_to(ctx.out(), "Yi");
        case unicode::Script::Zanabazar_Square: return format_to(ctx.out(), "Zanabazar_Square");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Block>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Block _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Block::Unspecified: return format_to(ctx.out(), "Unspecified");
        case unicode::Block::Adlam: return format_to(ctx.out(), "Adlam");
        case unicode::Block::Aegean_Numbers: return format_to(ctx.out(), "Aegean_Numbers");
        case unicode::Block::Ahom: return format_to(ctx.out(), "Ahom");
        case unicode::Block::Alchemical_Symbols: return format_to(ctx.out(), "Alchemical_Symbols");
        case unicode::Block::Alphabetic_Presentation_Forms: return format_to(ctx.out(), "Alphabetic_Presentation_Forms");
        case unicode::Block::Anatolian_Hieroglyphs: return format_to(ctx.out(), "Anatolian_Hieroglyphs");
        case unicode::Block::Ancient_Greek_Musical_Notation: return format_to(ctx.out(), "Ancient_Greek_Musical_Notation");
        case unicode::Block::Ancient_Greek_Numbers: return format_to(ctx.out(), "Ancient_Greek_Numbers");
        case unicode::Block::Ancient_Symbols: return format_to(ctx.out(), "Ancient_Symbols");
        case unicode::Block::Arabic: return format_to(ctx.out(), "Arabic");
        case unicode::Block::Arabic_Extended_A: return format_to(ctx.out(), "Arabic_Extended_A");
        case unicode::Block::Arabic_Mathematical_Alphabetic_Symbols: return format_to(ctx.out(), "Arabic_Mathematical_Alphabetic_Symbols");
        case unicode::Block::Arabic_Presentation_Forms_A: return format_to(ctx.out(), "Arabic_Presentation_Forms_A");
        case unicode::Block::Arabic_Presentation_Forms_B: return format_to(ctx.out(), "Arabic_Presentation_Forms_B");
        case unicode::Block::Arabic_Supplement: return format_to(ctx.out(), "Arabic_Supplement");
        case unicode::Block::Armenian: return format_to(ctx.out(), "Armenian");
        case unicode::Block::Arrows: return format_to(ctx.out(), "Arrows");
        case unicode::Block::Avestan: return format_to(ctx.out(), "Avestan");
        case unicode::Block::Balinese: return format_to(ctx.out(), "Balinese");
        case unicode::Block::Bamum: return format_to(ctx.out(), "Bamum");
        case unicode::Block::Bamum_Supplement: return format_to(ctx.out(), "Bamum_Supplement");
        case unicode::Block::Basic_Latin: return format_to(ctx.out(), "Basic_Latin");
        case unicode::Block::Bassa_Vah: return format_to(ctx.out(), "Bassa_Vah");
        case unicode::Block::Batak: return format_to(ctx.out(), "Batak");
        case unicode::Block::Bengali: return format_to(ctx.out(), "Bengali");
        case unicode::Block::Bhaiksuki: return format_to(ctx.out(), "Bhaiksuki");
        case unicode::Block::Block_Elements: return format_to(ctx.out(), "Block_Elements");
        case unicode::Block::Bopomofo: return format_to(ctx.out(), "Bopomofo");
        case unicode::Block::Bopomofo_Extended: return format_to(ctx.out(), "Bopomofo_Extended");
        case unicode::Block::Box_Drawing: return format_to(ctx.out(), "Box_Drawing");
        case unicode::Block::Brahmi: return format_to(ctx.out(), "Brahmi");
        case unicode::Block::Braille_Patterns: return format_to(ctx.out(), "Braille_Patterns");
        case unicode::Block::Buginese: return format_to(ctx.out(), "Buginese");
        case unicode::Block::Buhid: return format_to(ctx.out(), "Buhid");
        case unicode::Block::Byzantine_Musical_Symbols: return format_to(ctx.out(), "Byzantine_Musical_Symbols");
        case unicode::Block::CJK_Compatibility: return format_to(ctx.out(), "CJK_Compatibility");
        case unicode::Block::CJK_Compatibility_Forms: return format_to(ctx.out(), "CJK_Compatibility_Forms");
        case unicode::Block::CJK_Compatibility_Ideographs: return format_to(ctx.out(), "CJK_Compatibility_Ideographs");
        case unicode::Block::CJK_Compatibility_Ideographs_Supplement: return format_to(ctx.out(), "CJK_Compatibility_Ideographs_Supplement");
        case unicode::Block::CJK_Radicals_Supplement: return format_to(ctx.out(), "CJK_Radicals_Supplement");
        case unicode::Block::CJK_Strokes: return format_to(ctx.out(), "CJK_Strokes");
        case unicode::Block::CJK_Symbols_and_Punctuation: return format_to(ctx.out(), "CJK_Symbols_and_Punctuation");
        case unicode::Block::CJK_Unified_Ideographs: return format_to(ctx.out(), "CJK_Unified_Ideographs");
        case unicode::Block::CJK_Unified_Ideographs_Extension_A: return format_to(ctx.out(), "CJK_Unified_Ideographs_Extension_A");
        case unicode::Block::CJK_Unified_Ideographs_Extension_B: return format_to(ctx.out(), "CJK_Unified_Ideographs_Extension_B");
        case unicode::Block::CJK_Unified_Ideographs_Extension_C: return format_to(ctx.out(), "CJK_Unified_Ideographs_Extension_C");
        case unicode::Block::CJK_Unified_Ideographs_Extension_D: return format_to(ctx.out(), "CJK_Unified_Ideographs_Extension_D");
        case unicode::Block::CJK_Unified_Ideographs_Extension_E: return format_to(ctx.out(), "CJK_Unified_Ideographs_Extension_E");
        case unicode::Block::CJK_Unified_Ideographs_Extension_F: return format_to(ctx.out(), "CJK_Unified_Ideographs_Extension_F");
        case unicode::Block::CJK_Unified_Ideographs_Extension_G: return format_to(ctx.out(), "CJK_Unified_Ideographs_Extension_G");
        case unicode::Block::Carian: return format_to(ctx.out(), "Carian");
        case unicode::Block::Caucasian_Albanian: return format_to(ctx.out(), "Caucasian_Albanian");
        case unicode::Block::Chakma: return format_to(ctx.out(), "Chakma");
        case unicode::Block::Cham: return format_to(ctx.out(), "Cham");
        case unicode::Block::Cherokee: return format_to(ctx.out(), "Cherokee");
        case unicode::Block::Cherokee_Supplement: return format_to(ctx.out(), "Cherokee_Supplement");
        case unicode::Block::Chess_Symbols: return format_to(ctx.out(), "Chess_Symbols");
        case unicode::Block::Chorasmian: return format_to(ctx.out(), "Chorasmian");
        case unicode::Block::Combining_Diacritical_Marks: return format_to(ctx.out(), "Combining_Diacritical_Marks");
        case unicode::Block::Combining_Diacritical_Marks_Extended: return format_to(ctx.out(), "Combining_Diacritical_Marks_Extended");
        case unicode::Block::Combining_Diacritical_Marks_Supplement: return format_to(ctx.out(), "Combining_Diacritical_Marks_Supplement");
        case unicode::Block::Combining_Diacritical_Marks_for_Symbols: return format_to(ctx.out(), "Combining_Diacritical_Marks_for_Symbols");
        case unicode::Block::Combining_Half_Marks: return format_to(ctx.out(), "Combining_Half_Marks");
        case unicode::Block::Common_Indic_Number_Forms: return format_to(ctx.out(), "Common_Indic_Number_Forms");
        case unicode::Block::Control_Pictures: return format_to(ctx.out(), "Control_Pictures");
        case unicode::Block::Coptic: return format_to(ctx.out(), "Coptic");
        case unicode::Block::Coptic_Epact_Numbers: return format_to(ctx.out(), "Coptic_Epact_Numbers");
        case unicode::Block::Counting_Rod_Numerals: return format_to(ctx.out(), "Counting_Rod_Numerals");
        case unicode::Block::Cuneiform: return format_to(ctx.out(), "Cuneiform");
        case unicode::Block::Cuneiform_Numbers_and_Punctuation: return format_to(ctx.out(), "Cuneiform_Numbers_and_Punctuation");
        case unicode::Block::Currency_Symbols: return format_to(ctx.out(), "Currency_Symbols");
        case unicode::Block::Cypriot_Syllabary: return format_to(ctx.out(), "Cypriot_Syllabary");
        case unicode::Block::Cyrillic: return format_to(ctx.out(), "Cyrillic");
        case unicode::Block::Cyrillic_Extended_A: return format_to(ctx.out(), "Cyrillic_Extended_A");
        case unicode::Block::Cyrillic_Extended_B: return format_to(ctx.out(), "Cyrillic_Extended_B");
        case unicode::Block::Cyrillic_Extended_C: return format_to(ctx.out(), "Cyrillic_Extended_C");
        case unicode::Block::Cyrillic_Supplement: return format_to(ctx.out(), "Cyrillic_Supplement");
        case unicode::Block::Deseret: return format_to(ctx.out(), "Deseret");
        case unicode::Block::Devanagari: return format_to(ctx.out(), "Devanagari");
        case unicode::Block::Devanagari_Extended: return format_to(ctx.out(), "Devanagari_Extended");
        case unicode::Block::Dingbats: return format_to(ctx.out(), "Dingbats");
        case unicode::Block::Dives_Akuru: return format_to(ctx.out(), "Dives_Akuru");
        case unicode::Block::Dogra: return format_to(ctx.out(), "Dogra");
        case unicode::Block::Domino_Tiles: return format_to(ctx.out(), "Domino_Tiles");
        case unicode::Block::Duployan: return format_to(ctx.out(), "Duployan");
        case unicode::Block::Early_Dynastic_Cuneiform: return format_to(ctx.out(), "Early_Dynastic_Cuneiform");
        case unicode::Block::Egyptian_Hieroglyph_Format_Controls: return format_to(ctx.out(), "Egyptian_Hieroglyph_Format_Controls");
        case unicode::Block::Egyptian_Hieroglyphs: return format_to(ctx.out(), "Egyptian_Hieroglyphs");
        case unicode::Block::Elbasan: return format_to(ctx.out(), "Elbasan");
        case unicode::Block::Elymaic: return format_to(ctx.out(), "Elymaic");
        case unicode::Block::Emoticons: return format_to(ctx.out(), "Emoticons");
        case unicode::Block::Enclosed_Alphanumeric_Supplement: return format_to(ctx.out(), "Enclosed_Alphanumeric_Supplement");
        case unicode::Block::Enclosed_Alphanumerics: return format_to(ctx.out(), "Enclosed_Alphanumerics");
        case unicode::Block::Enclosed_CJK_Letters_and_Months: return format_to(ctx.out(), "Enclosed_CJK_Letters_and_Months");
        case unicode::Block::Enclosed_Ideographic_Supplement: return format_to(ctx.out(), "Enclosed_Ideographic_Supplement");
        case unicode::Block::Ethiopic: return format_to(ctx.out(), "Ethiopic");
        case unicode::Block::Ethiopic_Extended: return format_to(ctx.out(), "Ethiopic_Extended");
        case unicode::Block::Ethiopic_Extended_A: return format_to(ctx.out(), "Ethiopic_Extended_A");
        case unicode::Block::Ethiopic_Supplement: return format_to(ctx.out(), "Ethiopic_Supplement");
        case unicode::Block::General_Punctuation: return format_to(ctx.out(), "General_Punctuation");
        case unicode::Block::Geometric_Shapes: return format_to(ctx.out(), "Geometric_Shapes");
        case unicode::Block::Geometric_Shapes_Extended: return format_to(ctx.out(), "Geometric_Shapes_Extended");
        case unicode::Block::Georgian: return format_to(ctx.out(), "Georgian");
        case unicode::Block::Georgian_Extended: return format_to(ctx.out(), "Georgian_Extended");
        case unicode::Block::Georgian_Supplement: return format_to(ctx.out(), "Georgian_Supplement");
        case unicode::Block::Glagolitic: return format_to(ctx.out(), "Glagolitic");
        case unicode::Block::Glagolitic_Supplement: return format_to(ctx.out(), "Glagolitic_Supplement");
        case unicode::Block::Gothic: return format_to(ctx.out(), "Gothic");
        case unicode::Block::Grantha: return format_to(ctx.out(), "Grantha");
        case unicode::Block::Greek_Extended: return format_to(ctx.out(), "Greek_Extended");
        case unicode::Block::Greek_and_Coptic: return format_to(ctx.out(), "Greek_and_Coptic");
        case unicode::Block::Gujarati: return format_to(ctx.out(), "Gujarati");
        case unicode::Block::Gunjala_Gondi: return format_to(ctx.out(), "Gunjala_Gondi");
        case unicode::Block::Gurmukhi: return format_to(ctx.out(), "Gurmukhi");
        case unicode::Block::Halfwidth_and_Fullwidth_Forms: return format_to(ctx.out(), "Halfwidth_and_Fullwidth_Forms");
        case unicode::Block::Hangul_Compatibility_Jamo: return format_to(ctx.out(), "Hangul_Compatibility_Jamo");
        case unicode::Block::Hangul_Jamo: return format_to(ctx.out(), "Hangul_Jamo");
        case unicode::Block::Hangul_Jamo_Extended_A: return format_to(ctx.out(), "Hangul_Jamo_Extended_A");
        case unicode::Block::Hangul_Jamo_Extended_B: return format_to(ctx.out(), "Hangul_Jamo_Extended_B");
        case unicode::Block::Hangul_Syllables: return format_to(ctx.out(), "Hangul_Syllables");
        case unicode::Block::Hanifi_Rohingya: return format_to(ctx.out(), "Hanifi_Rohingya");
        case unicode::Block::Hanunoo: return format_to(ctx.out(), "Hanunoo");
        case unicode::Block::Hatran: return format_to(ctx.out(), "Hatran");
        case unicode::Block::Hebrew: return format_to(ctx.out(), "Hebrew");
        case unicode::Block::High_Private_Use_Surrogates: return format_to(ctx.out(), "High_Private_Use_Surrogates");
        case unicode::Block::High_Surrogates: return format_to(ctx.out(), "High_Surrogates");
        case unicode::Block::Hiragana: return format_to(ctx.out(), "Hiragana");
        case unicode::Block::IPA_Extensions: return format_to(ctx.out(), "IPA_Extensions");
        case unicode::Block::Ideographic_Description_Characters: return format_to(ctx.out(), "Ideographic_Description_Characters");
        case unicode::Block::Ideographic_Symbols_and_Punctuation: return format_to(ctx.out(), "Ideographic_Symbols_and_Punctuation");
        case unicode::Block::Imperial_Aramaic: return format_to(ctx.out(), "Imperial_Aramaic");
        case unicode::Block::Indic_Siyaq_Numbers: return format_to(ctx.out(), "Indic_Siyaq_Numbers");
        case unicode::Block::Inscriptional_Pahlavi: return format_to(ctx.out(), "Inscriptional_Pahlavi");
        case unicode::Block::Inscriptional_Parthian: return format_to(ctx.out(), "Inscriptional_Parthian");
        case unicode::Block::Javanese: return format_to(ctx.out(), "Javanese");
        case unicode::Block::Kaithi: return format_to(ctx.out(), "Kaithi");
        case unicode::Block::Kana_Extended_A: return format_to(ctx.out(), "Kana_Extended_A");
        case unicode::Block::Kana_Supplement: return format_to(ctx.out(), "Kana_Supplement");
        case unicode::Block::Kanbun: return format_to(ctx.out(), "Kanbun");
        case unicode::Block::Kangxi_Radicals: return format_to(ctx.out(), "Kangxi_Radicals");
        case unicode::Block::Kannada: return format_to(ctx.out(), "Kannada");
        case unicode::Block::Katakana: return format_to(ctx.out(), "Katakana");
        case unicode::Block::Katakana_Phonetic_Extensions: return format_to(ctx.out(), "Katakana_Phonetic_Extensions");
        case unicode::Block::Kayah_Li: return format_to(ctx.out(), "Kayah_Li");
        case unicode::Block::Kharoshthi: return format_to(ctx.out(), "Kharoshthi");
        case unicode::Block::Khitan_Small_Script: return format_to(ctx.out(), "Khitan_Small_Script");
        case unicode::Block::Khmer: return format_to(ctx.out(), "Khmer");
        case unicode::Block::Khmer_Symbols: return format_to(ctx.out(), "Khmer_Symbols");
        case unicode::Block::Khojki: return format_to(ctx.out(), "Khojki");
        case unicode::Block::Khudawadi: return format_to(ctx.out(), "Khudawadi");
        case unicode::Block::Lao: return format_to(ctx.out(), "Lao");
        case unicode::Block::Latin_Extended_Additional: return format_to(ctx.out(), "Latin_Extended_Additional");
        case unicode::Block::Latin_Extended_A: return format_to(ctx.out(), "Latin_Extended_A");
        case unicode::Block::Latin_Extended_B: return format_to(ctx.out(), "Latin_Extended_B");
        case unicode::Block::Latin_Extended_C: return format_to(ctx.out(), "Latin_Extended_C");
        case unicode::Block::Latin_Extended_D: return format_to(ctx.out(), "Latin_Extended_D");
        case unicode::Block::Latin_Extended_E: return format_to(ctx.out(), "Latin_Extended_E");
        case unicode::Block::Latin_1_Supplement: return format_to(ctx.out(), "Latin_1_Supplement");
        case unicode::Block::Lepcha: return format_to(ctx.out(), "Lepcha");
        case unicode::Block::Letterlike_Symbols: return format_to(ctx.out(), "Letterlike_Symbols");
        case unicode::Block::Limbu: return format_to(ctx.out(), "Limbu");
        case unicode::Block::Linear_A: return format_to(ctx.out(), "Linear_A");
        case unicode::Block::Linear_B_Ideograms: return format_to(ctx.out(), "Linear_B_Ideograms");
        case unicode::Block::Linear_B_Syllabary: return format_to(ctx.out(), "Linear_B_Syllabary");
        case unicode::Block::Lisu: return format_to(ctx.out(), "Lisu");
        case unicode::Block::Lisu_Supplement: return format_to(ctx.out(), "Lisu_Supplement");
        case unicode::Block::Low_Surrogates: return format_to(ctx.out(), "Low_Surrogates");
        case unicode::Block::Lycian: return format_to(ctx.out(), "Lycian");
        case unicode::Block::Lydian: return format_to(ctx.out(), "Lydian");
        case unicode::Block::Mahajani: return format_to(ctx.out(), "Mahajani");
        case unicode::Block::Mahjong_Tiles: return format_to(ctx.out(), "Mahjong_Tiles");
        case unicode::Block::Makasar: return format_to(ctx.out(), "Makasar");
        case unicode::Block::Malayalam: return format_to(ctx.out(), "Malayalam");
        case unicode::Block::Mandaic: return format_to(ctx.out(), "Mandaic");
        case unicode::Block::Manichaean: return format_to(ctx.out(), "Manichaean");
        case unicode::Block::Marchen: return format_to(ctx.out(), "Marchen");
        case unicode::Block::Masaram_Gondi: return format_to(ctx.out(), "Masaram_Gondi");
        case unicode::Block::Mathematical_Alphanumeric_Symbols: return format_to(ctx.out(), "Mathematical_Alphanumeric_Symbols");
        case unicode::Block::Mathematical_Operators: return format_to(ctx.out(), "Mathematical_Operators");
        case unicode::Block::Mayan_Numerals: return format_to(ctx.out(), "Mayan_Numerals");
        case unicode::Block::Medefaidrin: return format_to(ctx.out(), "Medefaidrin");
        case unicode::Block::Meetei_Mayek: return format_to(ctx.out(), "Meetei_Mayek");
        case unicode::Block::Meetei_Mayek_Extensions: return format_to(ctx.out(), "Meetei_Mayek_Extensions");
        case unicode::Block::Mende_Kikakui: return format_to(ctx.out(), "Mende_Kikakui");
        case unicode::Block::Meroitic_Cursive: return format_to(ctx.out(), "Meroitic_Cursive");
        case unicode::Block::Meroitic_Hieroglyphs: return format_to(ctx.out(), "Meroitic_Hieroglyphs");
        case unicode::Block::Miao: return format_to(ctx.out(), "Miao");
        case unicode::Block::Miscellaneous_Mathematical_Symbols_A: return format_to(ctx.out(), "Miscellaneous_Mathematical_Symbols_A");
        case unicode::Block::Miscellaneous_Mathematical_Symbols_B: return format_to(ctx.out(), "Miscellaneous_Mathematical_Symbols_B");
        case unicode::Block::Miscellaneous_Symbols: return format_to(ctx.out(), "Miscellaneous_Symbols");
        case unicode::Block::Miscellaneous_Symbols_and_Arrows: return format_to(ctx.out(), "Miscellaneous_Symbols_and_Arrows");
        case unicode::Block::Miscellaneous_Symbols_and_Pictographs: return format_to(ctx.out(), "Miscellaneous_Symbols_and_Pictographs");
        case unicode::Block::Miscellaneous_Technical: return format_to(ctx.out(), "Miscellaneous_Technical");
        case unicode::Block::Modi: return format_to(ctx.out(), "Modi");
        case unicode::Block::Modifier_Tone_Letters: return format_to(ctx.out(), "Modifier_Tone_Letters");
        case unicode::Block::Mongolian: return format_to(ctx.out(), "Mongolian");
        case unicode::Block::Mongolian_Supplement: return format_to(ctx.out(), "Mongolian_Supplement");
        case unicode::Block::Mro: return format_to(ctx.out(), "Mro");
        case unicode::Block::Multani: return format_to(ctx.out(), "Multani");
        case unicode::Block::Musical_Symbols: return format_to(ctx.out(), "Musical_Symbols");
        case unicode::Block::Myanmar: return format_to(ctx.out(), "Myanmar");
        case unicode::Block::Myanmar_Extended_A: return format_to(ctx.out(), "Myanmar_Extended_A");
        case unicode::Block::Myanmar_Extended_B: return format_to(ctx.out(), "Myanmar_Extended_B");
        case unicode::Block::NKo: return format_to(ctx.out(), "NKo");
        case unicode::Block::Nabataean: return format_to(ctx.out(), "Nabataean");
        case unicode::Block::Nandinagari: return format_to(ctx.out(), "Nandinagari");
        case unicode::Block::New_Tai_Lue: return format_to(ctx.out(), "New_Tai_Lue");
        case unicode::Block::Newa: return format_to(ctx.out(), "Newa");
        case unicode::Block::Number_Forms: return format_to(ctx.out(), "Number_Forms");
        case unicode::Block::Nushu: return format_to(ctx.out(), "Nushu");
        case unicode::Block::Nyiakeng_Puachue_Hmong: return format_to(ctx.out(), "Nyiakeng_Puachue_Hmong");
        case unicode::Block::Ogham: return format_to(ctx.out(), "Ogham");
        case unicode::Block::Ol_Chiki: return format_to(ctx.out(), "Ol_Chiki");
        case unicode::Block::Old_Hungarian: return format_to(ctx.out(), "Old_Hungarian");
        case unicode::Block::Old_Italic: return format_to(ctx.out(), "Old_Italic");
        case unicode::Block::Old_North_Arabian: return format_to(ctx.out(), "Old_North_Arabian");
        case unicode::Block::Old_Permic: return format_to(ctx.out(), "Old_Permic");
        case unicode::Block::Old_Persian: return format_to(ctx.out(), "Old_Persian");
        case unicode::Block::Old_Sogdian: return format_to(ctx.out(), "Old_Sogdian");
        case unicode::Block::Old_South_Arabian: return format_to(ctx.out(), "Old_South_Arabian");
        case unicode::Block::Old_Turkic: return format_to(ctx.out(), "Old_Turkic");
        case unicode::Block::Optical_Character_Recognition: return format_to(ctx.out(), "Optical_Character_Recognition");
        case unicode::Block::Oriya: return format_to(ctx.out(), "Oriya");
        case unicode::Block::Ornamental_Dingbats: return format_to(ctx.out(), "Ornamental_Dingbats");
        case unicode::Block::Osage: return format_to(ctx.out(), "Osage");
        case unicode::Block::Osmanya: return format_to(ctx.out(), "Osmanya");
        case unicode::Block::Ottoman_Siyaq_Numbers: return format_to(ctx.out(), "Ottoman_Siyaq_Numbers");
        case unicode::Block::Pahawh_Hmong: return format_to(ctx.out(), "Pahawh_Hmong");
        case unicode::Block::Palmyrene: return format_to(ctx.out(), "Palmyrene");
        case unicode::Block::Pau_Cin_Hau: return format_to(ctx.out(), "Pau_Cin_Hau");
        case unicode::Block::Phags_pa: return format_to(ctx.out(), "Phags_pa");
        case unicode::Block::Phaistos_Disc: return format_to(ctx.out(), "Phaistos_Disc");
        case unicode::Block::Phoenician: return format_to(ctx.out(), "Phoenician");
        case unicode::Block::Phonetic_Extensions: return format_to(ctx.out(), "Phonetic_Extensions");
        case unicode::Block::Phonetic_Extensions_Supplement: return format_to(ctx.out(), "Phonetic_Extensions_Supplement");
        case unicode::Block::Playing_Cards: return format_to(ctx.out(), "Playing_Cards");
        case unicode::Block::Private_Use_Area: return format_to(ctx.out(), "Private_Use_Area");
        case unicode::Block::Psalter_Pahlavi: return format_to(ctx.out(), "Psalter_Pahlavi");
        case unicode::Block::Rejang: return format_to(ctx.out(), "Rejang");
        case unicode::Block::Rumi_Numeral_Symbols: return format_to(ctx.out(), "Rumi_Numeral_Symbols");
        case unicode::Block::Runic: return format_to(ctx.out(), "Runic");
        case unicode::Block::Samaritan: return format_to(ctx.out(), "Samaritan");
        case unicode::Block::Saurashtra: return format_to(ctx.out(), "Saurashtra");
        case unicode::Block::Sharada: return format_to(ctx.out(), "Sharada");
        case unicode::Block::Shavian: return format_to(ctx.out(), "Shavian");
        case unicode::Block::Shorthand_Format_Controls: return format_to(ctx.out(), "Shorthand_Format_Controls");
        case unicode::Block::Siddham: return format_to(ctx.out(), "Siddham");
        case unicode::Block::Sinhala: return format_to(ctx.out(), "Sinhala");
        case unicode::Block::Sinhala_Archaic_Numbers: return format_to(ctx.out(), "Sinhala_Archaic_Numbers");
        case unicode::Block::Small_Form_Variants: return format_to(ctx.out(), "Small_Form_Variants");
        case unicode::Block::Small_Kana_Extension: return format_to(ctx.out(), "Small_Kana_Extension");
        case unicode::Block::Sogdian: return format_to(ctx.out(), "Sogdian");
        case unicode::Block::Sora_Sompeng: return format_to(ctx.out(), "Sora_Sompeng");
        case unicode::Block::Soyombo: return format_to(ctx.out(), "Soyombo");
        case unicode::Block::Spacing_Modifier_Letters: return format_to(ctx.out(), "Spacing_Modifier_Letters");
        case unicode::Block::Specials: return format_to(ctx.out(), "Specials");
        case unicode::Block::Sundanese: return format_to(ctx.out(), "Sundanese");
        case unicode::Block::Sundanese_Supplement: return format_to(ctx.out(), "Sundanese_Supplement");
        case unicode::Block::Superscripts_and_Subscripts: return format_to(ctx.out(), "Superscripts_and_Subscripts");
        case unicode::Block::Supplemental_Arrows_A: return format_to(ctx.out(), "Supplemental_Arrows_A");
        case unicode::Block::Supplemental_Arrows_B: return format_to(ctx.out(), "Supplemental_Arrows_B");
        case unicode::Block::Supplemental_Arrows_C: return format_to(ctx.out(), "Supplemental_Arrows_C");
        case unicode::Block::Supplemental_Mathematical_Operators: return format_to(ctx.out(), "Supplemental_Mathematical_Operators");
        case unicode::Block::Supplemental_Punctuation: return format_to(ctx.out(), "Supplemental_Punctuation");
        case unicode::Block::Supplemental_Symbols_and_Pictographs: return format_to(ctx.out(), "Supplemental_Symbols_and_Pictographs");
        case unicode::Block::Supplementary_Private_Use_Area_A: return format_to(ctx.out(), "Supplementary_Private_Use_Area_A");
        case unicode::Block::Supplementary_Private_Use_Area_B: return format_to(ctx.out(), "Supplementary_Private_Use_Area_B");
        case unicode::Block::Sutton_SignWriting: return format_to(ctx.out(), "Sutton_SignWriting");
        case unicode::Block::Syloti_Nagri: return format_to(ctx.out(), "Syloti_Nagri");
        case unicode::Block::Symbols_and_Pictographs_Extended_A: return format_to(ctx.out(), "Symbols_and_Pictographs_Extended_A");
        case unicode::Block::Symbols_for_Legacy_Computing: return format_to(ctx.out(), "Symbols_for_Legacy_Computing");
        case unicode::Block::Syriac: return format_to(ctx.out(), "Syriac");
        case unicode::Block::Syriac_Supplement: return format_to(ctx.out(), "Syriac_Supplement");
        case unicode::Block::Tagalog: return format_to(ctx.out(), "Tagalog");
        case unicode::Block::Tagbanwa: return format_to(ctx.out(), "Tagbanwa");
        case unicode::Block::Tags: return format_to(ctx.out(), "Tags");
        case unicode::Block::Tai_Le: return format_to(ctx.out(), "Tai_Le");
        case unicode::Block::Tai_Tham: return format_to(ctx.out(), "Tai_Tham");
        case unicode::Block::Tai_Viet: return format_to(ctx.out(), "Tai_Viet");
        case unicode::Block::Tai_Xuan_Jing_Symbols: return format_to(ctx.out(), "Tai_Xuan_Jing_Symbols");
        case unicode::Block::Takri: return format_to(ctx.out(), "Takri");
        case unicode::Block::Tamil: return format_to(ctx.out(), "Tamil");
        case unicode::Block::Tamil_Supplement: return format_to(ctx.out(), "Tamil_Supplement");
        case unicode::Block::Tangut: return format_to(ctx.out(), "Tangut");
        case unicode::Block::Tangut_Components: return format_to(ctx.out(), "Tangut_Components");
        case unicode::Block::Tangut_Supplement: return format_to(ctx.out(), "Tangut_Supplement");
        case unicode::Block::Telugu: return format_to(ctx.out(), "Telugu");
        case unicode::Block::Thaana: return format_to(ctx.out(), "Thaana");
        case unicode::Block::Thai: return format_to(ctx.out(), "Thai");
        case unicode::Block::Tibetan: return format_to(ctx.out(), "Tibetan");
        case unicode::Block::Tifinagh: return format_to(ctx.out(), "Tifinagh");
        case unicode::Block::Tirhuta: return format_to(ctx.out(), "Tirhuta");
        case unicode::Block::Transport_and_Map_Symbols: return format_to(ctx.out(), "Transport_and_Map_Symbols");
        case unicode::Block::Ugaritic: return format_to(ctx.out(), "Ugaritic");
        case unicode::Block::Unified_Canadian_Aboriginal_Syllabics: return format_to(ctx.out(), "Unified_Canadian_Aboriginal_Syllabics");
        case unicode::Block::Unified_Canadian_Aboriginal_Syllabics_Extended: return format_to(ctx.out(), "Unified_Canadian_Aboriginal_Syllabics_Extended");
        case unicode::Block::Vai: return format_to(ctx.out(), "Vai");
        case unicode::Block::Variation_Selectors: return format_to(ctx.out(), "Variation_Selectors");
        case unicode::Block::Variation_Selectors_Supplement: return format_to(ctx.out(), "Variation_Selectors_Supplement");
        case unicode::Block::Vedic_Extensions: return format_to(ctx.out(), "Vedic_Extensions");
        case unicode::Block::Vertical_Forms: return format_to(ctx.out(), "Vertical_Forms");
        case unicode::Block::Wancho: return format_to(ctx.out(), "Wancho");
        case unicode::Block::Warang_Citi: return format_to(ctx.out(), "Warang_Citi");
        case unicode::Block::Yezidi: return format_to(ctx.out(), "Yezidi");
        case unicode::Block::Yi_Radicals: return format_to(ctx.out(), "Yi_Radicals");
        case unicode::Block::Yi_Syllables: return format_to(ctx.out(), "Yi_Syllables");
        case unicode::Block::Yijing_Hexagram_Symbols: return format_to(ctx.out(), "Yijing_Hexagram_Symbols");
        case unicode::Block::Zanabazar_Square: return format_to(ctx.out(), "Zanabazar_Square");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::Grapheme_Cluster_Break>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::Grapheme_Cluster_Break _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::Grapheme_Cluster_Break::CR: return format_to(ctx.out(), "CR");
        case unicode::Grapheme_Cluster_Break::Control: return format_to(ctx.out(), "Control");
        case unicode::Grapheme_Cluster_Break::Extend: return format_to(ctx.out(), "Extend");
        case unicode::Grapheme_Cluster_Break::L: return format_to(ctx.out(), "L");
        case unicode::Grapheme_Cluster_Break::LF: return format_to(ctx.out(), "LF");
        case unicode::Grapheme_Cluster_Break::LV: return format_to(ctx.out(), "LV");
        case unicode::Grapheme_Cluster_Break::LVT: return format_to(ctx.out(), "LVT");
        case unicode::Grapheme_Cluster_Break::Prepend: return format_to(ctx.out(), "Prepend");
        case unicode::Grapheme_Cluster_Break::Regional_Indicator: return format_to(ctx.out(), "Regional_Indicator");
        case unicode::Grapheme_Cluster_Break::SpacingMark: return format_to(ctx.out(), "SpacingMark");
        case unicode::Grapheme_Cluster_Break::T: return format_to(ctx.out(), "T");
        case unicode::Grapheme_Cluster_Break::V: return format_to(ctx.out(), "V");
        case unicode::Grapheme_Cluster_Break::ZWJ: return format_to(ctx.out(), "ZWJ");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

template <>
struct formatter<unicode::EastAsianWidth>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(unicode::EastAsianWidth _value, FormatContext& ctx)
    {
        switch (_value)
{
        case unicode::EastAsianWidth::Ambiguous: return format_to(ctx.out(), "Ambiguous");
        case unicode::EastAsianWidth::FullWidth: return format_to(ctx.out(), "FullWidth");
        case unicode::EastAsianWidth::HalfWidth: return format_to(ctx.out(), "HalfWidth");
        case unicode::EastAsianWidth::Neutral: return format_to(ctx.out(), "Neutral");
        case unicode::EastAsianWidth::Narrow: return format_to(ctx.out(), "Narrow");
        case unicode::EastAsianWidth::Wide: return format_to(ctx.out(), "Wide");
        case unicode::EastAsianWidth::Unspecified: return format_to(ctx.out(), "Unspecified");
        }
        return format_to(ctx.out(), "({})", unsigned(_value));
    }
};

} // namespace fmt
