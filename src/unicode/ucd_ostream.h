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

#include <ostream>
#include <unicode/ucd.h>

namespace unicode {

inline std::ostream& operator<<(std::ostream& os, Plane _value) noexcept {
    switch (_value) {
        case Plane::Basic_Multilingual_Plane: return os << "Basic_Multilingual_Plane";
        case Plane::Supplementary_Ideographic_Plane: return os << "Supplementary_Ideographic_Plane";
        case Plane::Supplementary_Multilingual_Plane: return os << "Supplementary_Multilingual_Plane";
        case Plane::Supplementary_Private_Use_Area_Plane: return os << "Supplementary_Private_Use_Area_Plane";
        case Plane::Supplementary_Special_purpose_Plane: return os << "Supplementary_Special_purpose_Plane";
        case Plane::Tertiary_Ideographic_Plane: return os << "Tertiary_Ideographic_Plane";
        case Plane::Unassigned: return os << "Unassigned";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Age _value) noexcept {
    switch (_value) {
        case Age::Unassigned: return os << "Unassigned";
        case Age::V10_0: return os << "V10_0";
        case Age::V11_0: return os << "V11_0";
        case Age::V12_0: return os << "V12_0";
        case Age::V12_1: return os << "V12_1";
        case Age::V13_0: return os << "V13_0";
        case Age::V1_1: return os << "V1_1";
        case Age::V2_0: return os << "V2_0";
        case Age::V2_1: return os << "V2_1";
        case Age::V3_0: return os << "V3_0";
        case Age::V3_1: return os << "V3_1";
        case Age::V3_2: return os << "V3_2";
        case Age::V4_0: return os << "V4_0";
        case Age::V4_1: return os << "V4_1";
        case Age::V5_0: return os << "V5_0";
        case Age::V5_1: return os << "V5_1";
        case Age::V5_2: return os << "V5_2";
        case Age::V6_0: return os << "V6_0";
        case Age::V6_1: return os << "V6_1";
        case Age::V6_2: return os << "V6_2";
        case Age::V6_3: return os << "V6_3";
        case Age::V7_0: return os << "V7_0";
        case Age::V8_0: return os << "V8_0";
        case Age::V9_0: return os << "V9_0";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Bidi_Class _value) noexcept {
    switch (_value) {
        case Bidi_Class::Arabic_Letter: return os << "Arabic_Letter";
        case Bidi_Class::Arabic_Number: return os << "Arabic_Number";
        case Bidi_Class::Boundary_Neutral: return os << "Boundary_Neutral";
        case Bidi_Class::Common_Separator: return os << "Common_Separator";
        case Bidi_Class::European_Number: return os << "European_Number";
        case Bidi_Class::European_Separator: return os << "European_Separator";
        case Bidi_Class::European_Terminator: return os << "European_Terminator";
        case Bidi_Class::First_Strong_Isolate: return os << "First_Strong_Isolate";
        case Bidi_Class::Left_To_Right: return os << "Left_To_Right";
        case Bidi_Class::Left_To_Right_Embedding: return os << "Left_To_Right_Embedding";
        case Bidi_Class::Left_To_Right_Isolate: return os << "Left_To_Right_Isolate";
        case Bidi_Class::Left_To_Right_Override: return os << "Left_To_Right_Override";
        case Bidi_Class::Nonspacing_Mark: return os << "Nonspacing_Mark";
        case Bidi_Class::Other_Neutral: return os << "Other_Neutral";
        case Bidi_Class::Paragraph_Separator: return os << "Paragraph_Separator";
        case Bidi_Class::Pop_Directional_Format: return os << "Pop_Directional_Format";
        case Bidi_Class::Pop_Directional_Isolate: return os << "Pop_Directional_Isolate";
        case Bidi_Class::Right_To_Left: return os << "Right_To_Left";
        case Bidi_Class::Right_To_Left_Embedding: return os << "Right_To_Left_Embedding";
        case Bidi_Class::Right_To_Left_Isolate: return os << "Right_To_Left_Isolate";
        case Bidi_Class::Right_To_Left_Override: return os << "Right_To_Left_Override";
        case Bidi_Class::Segment_Separator: return os << "Segment_Separator";
        case Bidi_Class::White_Space: return os << "White_Space";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Bidi_Paired_Bracket_Type _value) noexcept {
    switch (_value) {
        case Bidi_Paired_Bracket_Type::Close: return os << "Close";
        case Bidi_Paired_Bracket_Type::None: return os << "None";
        case Bidi_Paired_Bracket_Type::Open: return os << "Open";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Canonical_Combining_Class _value) noexcept {
    switch (_value) {
        case Canonical_Combining_Class::A: return os << "A";
        case Canonical_Combining_Class::AL: return os << "AL";
        case Canonical_Combining_Class::AR: return os << "AR";
        case Canonical_Combining_Class::ATA: return os << "ATA";
        case Canonical_Combining_Class::ATAR: return os << "ATAR";
        case Canonical_Combining_Class::ATB: return os << "ATB";
        case Canonical_Combining_Class::ATBL: return os << "ATBL";
        case Canonical_Combining_Class::B: return os << "B";
        case Canonical_Combining_Class::BL: return os << "BL";
        case Canonical_Combining_Class::BR: return os << "BR";
        case Canonical_Combining_Class::CCC10: return os << "CCC10";
        case Canonical_Combining_Class::CCC103: return os << "CCC103";
        case Canonical_Combining_Class::CCC107: return os << "CCC107";
        case Canonical_Combining_Class::CCC11: return os << "CCC11";
        case Canonical_Combining_Class::CCC118: return os << "CCC118";
        case Canonical_Combining_Class::CCC12: return os << "CCC12";
        case Canonical_Combining_Class::CCC122: return os << "CCC122";
        case Canonical_Combining_Class::CCC129: return os << "CCC129";
        case Canonical_Combining_Class::CCC13: return os << "CCC13";
        case Canonical_Combining_Class::CCC130: return os << "CCC130";
        case Canonical_Combining_Class::CCC132: return os << "CCC132";
        case Canonical_Combining_Class::CCC133: return os << "CCC133";
        case Canonical_Combining_Class::CCC14: return os << "CCC14";
        case Canonical_Combining_Class::CCC15: return os << "CCC15";
        case Canonical_Combining_Class::CCC16: return os << "CCC16";
        case Canonical_Combining_Class::CCC17: return os << "CCC17";
        case Canonical_Combining_Class::CCC18: return os << "CCC18";
        case Canonical_Combining_Class::CCC19: return os << "CCC19";
        case Canonical_Combining_Class::CCC20: return os << "CCC20";
        case Canonical_Combining_Class::CCC21: return os << "CCC21";
        case Canonical_Combining_Class::CCC22: return os << "CCC22";
        case Canonical_Combining_Class::CCC23: return os << "CCC23";
        case Canonical_Combining_Class::CCC24: return os << "CCC24";
        case Canonical_Combining_Class::CCC25: return os << "CCC25";
        case Canonical_Combining_Class::CCC26: return os << "CCC26";
        case Canonical_Combining_Class::CCC27: return os << "CCC27";
        case Canonical_Combining_Class::CCC28: return os << "CCC28";
        case Canonical_Combining_Class::CCC29: return os << "CCC29";
        case Canonical_Combining_Class::CCC30: return os << "CCC30";
        case Canonical_Combining_Class::CCC31: return os << "CCC31";
        case Canonical_Combining_Class::CCC32: return os << "CCC32";
        case Canonical_Combining_Class::CCC33: return os << "CCC33";
        case Canonical_Combining_Class::CCC34: return os << "CCC34";
        case Canonical_Combining_Class::CCC35: return os << "CCC35";
        case Canonical_Combining_Class::CCC36: return os << "CCC36";
        case Canonical_Combining_Class::CCC84: return os << "CCC84";
        case Canonical_Combining_Class::CCC91: return os << "CCC91";
        case Canonical_Combining_Class::DA: return os << "DA";
        case Canonical_Combining_Class::DB: return os << "DB";
        case Canonical_Combining_Class::HANR: return os << "HANR";
        case Canonical_Combining_Class::IS: return os << "IS";
        case Canonical_Combining_Class::KV: return os << "KV";
        case Canonical_Combining_Class::L: return os << "L";
        case Canonical_Combining_Class::NK: return os << "NK";
        case Canonical_Combining_Class::NR: return os << "NR";
        case Canonical_Combining_Class::OV: return os << "OV";
        case Canonical_Combining_Class::R: return os << "R";
        case Canonical_Combining_Class::VR: return os << "VR";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Decomposition_Type _value) noexcept {
    switch (_value) {
        case Decomposition_Type::Canonical: return os << "Canonical";
        case Decomposition_Type::Circle: return os << "Circle";
        case Decomposition_Type::Compat: return os << "Compat";
        case Decomposition_Type::Final: return os << "Final";
        case Decomposition_Type::Font: return os << "Font";
        case Decomposition_Type::Fraction: return os << "Fraction";
        case Decomposition_Type::Initial: return os << "Initial";
        case Decomposition_Type::Isolated: return os << "Isolated";
        case Decomposition_Type::Medial: return os << "Medial";
        case Decomposition_Type::Narrow: return os << "Narrow";
        case Decomposition_Type::Nobreak: return os << "Nobreak";
        case Decomposition_Type::None: return os << "None";
        case Decomposition_Type::Small: return os << "Small";
        case Decomposition_Type::Square: return os << "Square";
        case Decomposition_Type::Sub: return os << "Sub";
        case Decomposition_Type::Super: return os << "Super";
        case Decomposition_Type::Vertical: return os << "Vertical";
        case Decomposition_Type::Wide: return os << "Wide";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, East_Asian_Width _value) noexcept {
    switch (_value) {
        case East_Asian_Width::Ambiguous: return os << "Ambiguous";
        case East_Asian_Width::Fullwidth: return os << "Fullwidth";
        case East_Asian_Width::Halfwidth: return os << "Halfwidth";
        case East_Asian_Width::Narrow: return os << "Narrow";
        case East_Asian_Width::Neutral: return os << "Neutral";
        case East_Asian_Width::Wide: return os << "Wide";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Hangul_Syllable_Type _value) noexcept {
    switch (_value) {
        case Hangul_Syllable_Type::LVT_Syllable: return os << "LVT_Syllable";
        case Hangul_Syllable_Type::LV_Syllable: return os << "LV_Syllable";
        case Hangul_Syllable_Type::Leading_Jamo: return os << "Leading_Jamo";
        case Hangul_Syllable_Type::Not_Applicable: return os << "Not_Applicable";
        case Hangul_Syllable_Type::Trailing_Jamo: return os << "Trailing_Jamo";
        case Hangul_Syllable_Type::Vowel_Jamo: return os << "Vowel_Jamo";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Indic_Positional_Category _value) noexcept {
    switch (_value) {
        case Indic_Positional_Category::Bottom: return os << "Bottom";
        case Indic_Positional_Category::Bottom_And_Left: return os << "Bottom_And_Left";
        case Indic_Positional_Category::Bottom_And_Right: return os << "Bottom_And_Right";
        case Indic_Positional_Category::Left: return os << "Left";
        case Indic_Positional_Category::Left_And_Right: return os << "Left_And_Right";
        case Indic_Positional_Category::NA: return os << "NA";
        case Indic_Positional_Category::Overstruck: return os << "Overstruck";
        case Indic_Positional_Category::Right: return os << "Right";
        case Indic_Positional_Category::Top: return os << "Top";
        case Indic_Positional_Category::Top_And_Bottom: return os << "Top_And_Bottom";
        case Indic_Positional_Category::Top_And_Bottom_And_Left: return os << "Top_And_Bottom_And_Left";
        case Indic_Positional_Category::Top_And_Bottom_And_Right: return os << "Top_And_Bottom_And_Right";
        case Indic_Positional_Category::Top_And_Left: return os << "Top_And_Left";
        case Indic_Positional_Category::Top_And_Left_And_Right: return os << "Top_And_Left_And_Right";
        case Indic_Positional_Category::Top_And_Right: return os << "Top_And_Right";
        case Indic_Positional_Category::Visual_Order_Left: return os << "Visual_Order_Left";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Indic_Syllabic_Category _value) noexcept {
    switch (_value) {
        case Indic_Syllabic_Category::Avagraha: return os << "Avagraha";
        case Indic_Syllabic_Category::Bindu: return os << "Bindu";
        case Indic_Syllabic_Category::Brahmi_Joining_Number: return os << "Brahmi_Joining_Number";
        case Indic_Syllabic_Category::Cantillation_Mark: return os << "Cantillation_Mark";
        case Indic_Syllabic_Category::Consonant: return os << "Consonant";
        case Indic_Syllabic_Category::Consonant_Dead: return os << "Consonant_Dead";
        case Indic_Syllabic_Category::Consonant_Final: return os << "Consonant_Final";
        case Indic_Syllabic_Category::Consonant_Head_Letter: return os << "Consonant_Head_Letter";
        case Indic_Syllabic_Category::Consonant_Initial_Postfixed: return os << "Consonant_Initial_Postfixed";
        case Indic_Syllabic_Category::Consonant_Killer: return os << "Consonant_Killer";
        case Indic_Syllabic_Category::Consonant_Medial: return os << "Consonant_Medial";
        case Indic_Syllabic_Category::Consonant_Placeholder: return os << "Consonant_Placeholder";
        case Indic_Syllabic_Category::Consonant_Preceding_Repha: return os << "Consonant_Preceding_Repha";
        case Indic_Syllabic_Category::Consonant_Prefixed: return os << "Consonant_Prefixed";
        case Indic_Syllabic_Category::Consonant_Subjoined: return os << "Consonant_Subjoined";
        case Indic_Syllabic_Category::Consonant_Succeeding_Repha: return os << "Consonant_Succeeding_Repha";
        case Indic_Syllabic_Category::Consonant_With_Stacker: return os << "Consonant_With_Stacker";
        case Indic_Syllabic_Category::Gemination_Mark: return os << "Gemination_Mark";
        case Indic_Syllabic_Category::Invisible_Stacker: return os << "Invisible_Stacker";
        case Indic_Syllabic_Category::Joiner: return os << "Joiner";
        case Indic_Syllabic_Category::Modifying_Letter: return os << "Modifying_Letter";
        case Indic_Syllabic_Category::Non_Joiner: return os << "Non_Joiner";
        case Indic_Syllabic_Category::Nukta: return os << "Nukta";
        case Indic_Syllabic_Category::Number: return os << "Number";
        case Indic_Syllabic_Category::Number_Joiner: return os << "Number_Joiner";
        case Indic_Syllabic_Category::Other: return os << "Other";
        case Indic_Syllabic_Category::Pure_Killer: return os << "Pure_Killer";
        case Indic_Syllabic_Category::Register_Shifter: return os << "Register_Shifter";
        case Indic_Syllabic_Category::Syllable_Modifier: return os << "Syllable_Modifier";
        case Indic_Syllabic_Category::Tone_Letter: return os << "Tone_Letter";
        case Indic_Syllabic_Category::Tone_Mark: return os << "Tone_Mark";
        case Indic_Syllabic_Category::Virama: return os << "Virama";
        case Indic_Syllabic_Category::Visarga: return os << "Visarga";
        case Indic_Syllabic_Category::Vowel: return os << "Vowel";
        case Indic_Syllabic_Category::Vowel_Dependent: return os << "Vowel_Dependent";
        case Indic_Syllabic_Category::Vowel_Independent: return os << "Vowel_Independent";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Jamo_Short_Name _value) noexcept {
    switch (_value) {
        case Jamo_Short_Name::A: return os << "A";
        case Jamo_Short_Name::AE: return os << "AE";
        case Jamo_Short_Name::B: return os << "B";
        case Jamo_Short_Name::BB: return os << "BB";
        case Jamo_Short_Name::BS: return os << "BS";
        case Jamo_Short_Name::C: return os << "C";
        case Jamo_Short_Name::D: return os << "D";
        case Jamo_Short_Name::DD: return os << "DD";
        case Jamo_Short_Name::E: return os << "E";
        case Jamo_Short_Name::EO: return os << "EO";
        case Jamo_Short_Name::EU: return os << "EU";
        case Jamo_Short_Name::G: return os << "G";
        case Jamo_Short_Name::GG: return os << "GG";
        case Jamo_Short_Name::GS: return os << "GS";
        case Jamo_Short_Name::H: return os << "H";
        case Jamo_Short_Name::I: return os << "I";
        case Jamo_Short_Name::J: return os << "J";
        case Jamo_Short_Name::JJ: return os << "JJ";
        case Jamo_Short_Name::K: return os << "K";
        case Jamo_Short_Name::L: return os << "L";
        case Jamo_Short_Name::LB: return os << "LB";
        case Jamo_Short_Name::LG: return os << "LG";
        case Jamo_Short_Name::LH: return os << "LH";
        case Jamo_Short_Name::LM: return os << "LM";
        case Jamo_Short_Name::LP: return os << "LP";
        case Jamo_Short_Name::LS: return os << "LS";
        case Jamo_Short_Name::LT: return os << "LT";
        case Jamo_Short_Name::M: return os << "M";
        case Jamo_Short_Name::N: return os << "N";
        case Jamo_Short_Name::NG: return os << "NG";
        case Jamo_Short_Name::NH: return os << "NH";
        case Jamo_Short_Name::NJ: return os << "NJ";
        case Jamo_Short_Name::O: return os << "O";
        case Jamo_Short_Name::OE: return os << "OE";
        case Jamo_Short_Name::P: return os << "P";
        case Jamo_Short_Name::R: return os << "R";
        case Jamo_Short_Name::S: return os << "S";
        case Jamo_Short_Name::SS: return os << "SS";
        case Jamo_Short_Name::T: return os << "T";
        case Jamo_Short_Name::U: return os << "U";
        case Jamo_Short_Name::WA: return os << "WA";
        case Jamo_Short_Name::WAE: return os << "WAE";
        case Jamo_Short_Name::WE: return os << "WE";
        case Jamo_Short_Name::WEO: return os << "WEO";
        case Jamo_Short_Name::WI: return os << "WI";
        case Jamo_Short_Name::YA: return os << "YA";
        case Jamo_Short_Name::YAE: return os << "YAE";
        case Jamo_Short_Name::YE: return os << "YE";
        case Jamo_Short_Name::YEO: return os << "YEO";
        case Jamo_Short_Name::YI: return os << "YI";
        case Jamo_Short_Name::YO: return os << "YO";
        case Jamo_Short_Name::YU: return os << "YU";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Joining_Group _value) noexcept {
    switch (_value) {
        case Joining_Group::African_Feh: return os << "African_Feh";
        case Joining_Group::African_Noon: return os << "African_Noon";
        case Joining_Group::African_Qaf: return os << "African_Qaf";
        case Joining_Group::Ain: return os << "Ain";
        case Joining_Group::Alaph: return os << "Alaph";
        case Joining_Group::Alef: return os << "Alef";
        case Joining_Group::Beh: return os << "Beh";
        case Joining_Group::Beth: return os << "Beth";
        case Joining_Group::Burushaski_Yeh_Barree: return os << "Burushaski_Yeh_Barree";
        case Joining_Group::Dal: return os << "Dal";
        case Joining_Group::Dalath_Rish: return os << "Dalath_Rish";
        case Joining_Group::E: return os << "E";
        case Joining_Group::Farsi_Yeh: return os << "Farsi_Yeh";
        case Joining_Group::Fe: return os << "Fe";
        case Joining_Group::Feh: return os << "Feh";
        case Joining_Group::Final_Semkath: return os << "Final_Semkath";
        case Joining_Group::Gaf: return os << "Gaf";
        case Joining_Group::Gamal: return os << "Gamal";
        case Joining_Group::Hah: return os << "Hah";
        case Joining_Group::Hamza_On_Heh_Goal: return os << "Hamza_On_Heh_Goal";
        case Joining_Group::Hanifi_Rohingya_Kinna_Ya: return os << "Hanifi_Rohingya_Kinna_Ya";
        case Joining_Group::Hanifi_Rohingya_Pa: return os << "Hanifi_Rohingya_Pa";
        case Joining_Group::He: return os << "He";
        case Joining_Group::Heh: return os << "Heh";
        case Joining_Group::Heh_Goal: return os << "Heh_Goal";
        case Joining_Group::Heth: return os << "Heth";
        case Joining_Group::Kaf: return os << "Kaf";
        case Joining_Group::Kaph: return os << "Kaph";
        case Joining_Group::Khaph: return os << "Khaph";
        case Joining_Group::Knotted_Heh: return os << "Knotted_Heh";
        case Joining_Group::Lam: return os << "Lam";
        case Joining_Group::Lamadh: return os << "Lamadh";
        case Joining_Group::Malayalam_Bha: return os << "Malayalam_Bha";
        case Joining_Group::Malayalam_Ja: return os << "Malayalam_Ja";
        case Joining_Group::Malayalam_Lla: return os << "Malayalam_Lla";
        case Joining_Group::Malayalam_Llla: return os << "Malayalam_Llla";
        case Joining_Group::Malayalam_Nga: return os << "Malayalam_Nga";
        case Joining_Group::Malayalam_Nna: return os << "Malayalam_Nna";
        case Joining_Group::Malayalam_Nnna: return os << "Malayalam_Nnna";
        case Joining_Group::Malayalam_Nya: return os << "Malayalam_Nya";
        case Joining_Group::Malayalam_Ra: return os << "Malayalam_Ra";
        case Joining_Group::Malayalam_Ssa: return os << "Malayalam_Ssa";
        case Joining_Group::Malayalam_Tta: return os << "Malayalam_Tta";
        case Joining_Group::Manichaean_Aleph: return os << "Manichaean_Aleph";
        case Joining_Group::Manichaean_Ayin: return os << "Manichaean_Ayin";
        case Joining_Group::Manichaean_Beth: return os << "Manichaean_Beth";
        case Joining_Group::Manichaean_Daleth: return os << "Manichaean_Daleth";
        case Joining_Group::Manichaean_Dhamedh: return os << "Manichaean_Dhamedh";
        case Joining_Group::Manichaean_Five: return os << "Manichaean_Five";
        case Joining_Group::Manichaean_Gimel: return os << "Manichaean_Gimel";
        case Joining_Group::Manichaean_Heth: return os << "Manichaean_Heth";
        case Joining_Group::Manichaean_Hundred: return os << "Manichaean_Hundred";
        case Joining_Group::Manichaean_Kaph: return os << "Manichaean_Kaph";
        case Joining_Group::Manichaean_Lamedh: return os << "Manichaean_Lamedh";
        case Joining_Group::Manichaean_Mem: return os << "Manichaean_Mem";
        case Joining_Group::Manichaean_Nun: return os << "Manichaean_Nun";
        case Joining_Group::Manichaean_One: return os << "Manichaean_One";
        case Joining_Group::Manichaean_Pe: return os << "Manichaean_Pe";
        case Joining_Group::Manichaean_Qoph: return os << "Manichaean_Qoph";
        case Joining_Group::Manichaean_Resh: return os << "Manichaean_Resh";
        case Joining_Group::Manichaean_Sadhe: return os << "Manichaean_Sadhe";
        case Joining_Group::Manichaean_Samekh: return os << "Manichaean_Samekh";
        case Joining_Group::Manichaean_Taw: return os << "Manichaean_Taw";
        case Joining_Group::Manichaean_Ten: return os << "Manichaean_Ten";
        case Joining_Group::Manichaean_Teth: return os << "Manichaean_Teth";
        case Joining_Group::Manichaean_Thamedh: return os << "Manichaean_Thamedh";
        case Joining_Group::Manichaean_Twenty: return os << "Manichaean_Twenty";
        case Joining_Group::Manichaean_Waw: return os << "Manichaean_Waw";
        case Joining_Group::Manichaean_Yodh: return os << "Manichaean_Yodh";
        case Joining_Group::Manichaean_Zayin: return os << "Manichaean_Zayin";
        case Joining_Group::Meem: return os << "Meem";
        case Joining_Group::Mim: return os << "Mim";
        case Joining_Group::No_Joining_Group: return os << "No_Joining_Group";
        case Joining_Group::Noon: return os << "Noon";
        case Joining_Group::Nun: return os << "Nun";
        case Joining_Group::Nya: return os << "Nya";
        case Joining_Group::Pe: return os << "Pe";
        case Joining_Group::Qaf: return os << "Qaf";
        case Joining_Group::Qaph: return os << "Qaph";
        case Joining_Group::Reh: return os << "Reh";
        case Joining_Group::Reversed_Pe: return os << "Reversed_Pe";
        case Joining_Group::Rohingya_Yeh: return os << "Rohingya_Yeh";
        case Joining_Group::Sad: return os << "Sad";
        case Joining_Group::Sadhe: return os << "Sadhe";
        case Joining_Group::Seen: return os << "Seen";
        case Joining_Group::Semkath: return os << "Semkath";
        case Joining_Group::Shin: return os << "Shin";
        case Joining_Group::Straight_Waw: return os << "Straight_Waw";
        case Joining_Group::Swash_Kaf: return os << "Swash_Kaf";
        case Joining_Group::Syriac_Waw: return os << "Syriac_Waw";
        case Joining_Group::Tah: return os << "Tah";
        case Joining_Group::Taw: return os << "Taw";
        case Joining_Group::Teh_Marbuta: return os << "Teh_Marbuta";
        case Joining_Group::Teth: return os << "Teth";
        case Joining_Group::Waw: return os << "Waw";
        case Joining_Group::Yeh: return os << "Yeh";
        case Joining_Group::Yeh_Barree: return os << "Yeh_Barree";
        case Joining_Group::Yeh_With_Tail: return os << "Yeh_With_Tail";
        case Joining_Group::Yudh: return os << "Yudh";
        case Joining_Group::Yudh_He: return os << "Yudh_He";
        case Joining_Group::Zain: return os << "Zain";
        case Joining_Group::Zhain: return os << "Zhain";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Joining_Type _value) noexcept {
    switch (_value) {
        case Joining_Type::Dual_Joining: return os << "Dual_Joining";
        case Joining_Type::Join_Causing: return os << "Join_Causing";
        case Joining_Type::Left_Joining: return os << "Left_Joining";
        case Joining_Type::Non_Joining: return os << "Non_Joining";
        case Joining_Type::Right_Joining: return os << "Right_Joining";
        case Joining_Type::Transparent: return os << "Transparent";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Line_Break _value) noexcept {
    switch (_value) {
        case Line_Break::Alphabetic: return os << "Alphabetic";
        case Line_Break::Ambiguous: return os << "Ambiguous";
        case Line_Break::Break_After: return os << "Break_After";
        case Line_Break::Break_Before: return os << "Break_Before";
        case Line_Break::Break_Both: return os << "Break_Both";
        case Line_Break::Break_Symbols: return os << "Break_Symbols";
        case Line_Break::Carriage_Return: return os << "Carriage_Return";
        case Line_Break::Close_Parenthesis: return os << "Close_Parenthesis";
        case Line_Break::Close_Punctuation: return os << "Close_Punctuation";
        case Line_Break::Combining_Mark: return os << "Combining_Mark";
        case Line_Break::Complex_Context: return os << "Complex_Context";
        case Line_Break::Conditional_Japanese_Starter: return os << "Conditional_Japanese_Starter";
        case Line_Break::Contingent_Break: return os << "Contingent_Break";
        case Line_Break::E_Base: return os << "E_Base";
        case Line_Break::E_Modifier: return os << "E_Modifier";
        case Line_Break::Exclamation: return os << "Exclamation";
        case Line_Break::Glue: return os << "Glue";
        case Line_Break::H2: return os << "H2";
        case Line_Break::H3: return os << "H3";
        case Line_Break::Hebrew_Letter: return os << "Hebrew_Letter";
        case Line_Break::Hyphen: return os << "Hyphen";
        case Line_Break::Ideographic: return os << "Ideographic";
        case Line_Break::Infix_Numeric: return os << "Infix_Numeric";
        case Line_Break::Inseparable: return os << "Inseparable";
        case Line_Break::JL: return os << "JL";
        case Line_Break::JT: return os << "JT";
        case Line_Break::JV: return os << "JV";
        case Line_Break::Line_Feed: return os << "Line_Feed";
        case Line_Break::Mandatory_Break: return os << "Mandatory_Break";
        case Line_Break::Next_Line: return os << "Next_Line";
        case Line_Break::Nonstarter: return os << "Nonstarter";
        case Line_Break::Numeric: return os << "Numeric";
        case Line_Break::Open_Punctuation: return os << "Open_Punctuation";
        case Line_Break::Postfix_Numeric: return os << "Postfix_Numeric";
        case Line_Break::Prefix_Numeric: return os << "Prefix_Numeric";
        case Line_Break::Quotation: return os << "Quotation";
        case Line_Break::Regional_Indicator: return os << "Regional_Indicator";
        case Line_Break::Space: return os << "Space";
        case Line_Break::Surrogate: return os << "Surrogate";
        case Line_Break::Unknown: return os << "Unknown";
        case Line_Break::Word_Joiner: return os << "Word_Joiner";
        case Line_Break::ZWJ: return os << "ZWJ";
        case Line_Break::ZWSpace: return os << "ZWSpace";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, NFC_Quick_Check _value) noexcept {
    switch (_value) {
        case NFC_Quick_Check::Maybe: return os << "Maybe";
        case NFC_Quick_Check::No: return os << "No";
        case NFC_Quick_Check::Yes: return os << "Yes";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, NFKC_Quick_Check _value) noexcept {
    switch (_value) {
        case NFKC_Quick_Check::Maybe: return os << "Maybe";
        case NFKC_Quick_Check::No: return os << "No";
        case NFKC_Quick_Check::Yes: return os << "Yes";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Numeric_Type _value) noexcept {
    switch (_value) {
        case Numeric_Type::Decimal: return os << "Decimal";
        case Numeric_Type::Digit: return os << "Digit";
        case Numeric_Type::None: return os << "None";
        case Numeric_Type::Numeric: return os << "Numeric";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Sentence_Break _value) noexcept {
    switch (_value) {
        case Sentence_Break::ATerm: return os << "ATerm";
        case Sentence_Break::CR: return os << "CR";
        case Sentence_Break::Close: return os << "Close";
        case Sentence_Break::Extend: return os << "Extend";
        case Sentence_Break::Format: return os << "Format";
        case Sentence_Break::LF: return os << "LF";
        case Sentence_Break::Lower: return os << "Lower";
        case Sentence_Break::Numeric: return os << "Numeric";
        case Sentence_Break::OLetter: return os << "OLetter";
        case Sentence_Break::Other: return os << "Other";
        case Sentence_Break::SContinue: return os << "SContinue";
        case Sentence_Break::STerm: return os << "STerm";
        case Sentence_Break::Sep: return os << "Sep";
        case Sentence_Break::Sp: return os << "Sp";
        case Sentence_Break::Upper: return os << "Upper";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Vertical_Orientation _value) noexcept {
    switch (_value) {
        case Vertical_Orientation::Rotated: return os << "Rotated";
        case Vertical_Orientation::Transformed_Rotated: return os << "Transformed_Rotated";
        case Vertical_Orientation::Transformed_Upright: return os << "Transformed_Upright";
        case Vertical_Orientation::Upright: return os << "Upright";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Word_Break _value) noexcept {
    switch (_value) {
        case Word_Break::ALetter: return os << "ALetter";
        case Word_Break::CR: return os << "CR";
        case Word_Break::Double_Quote: return os << "Double_Quote";
        case Word_Break::E_Base: return os << "E_Base";
        case Word_Break::E_Base_GAZ: return os << "E_Base_GAZ";
        case Word_Break::E_Modifier: return os << "E_Modifier";
        case Word_Break::Extend: return os << "Extend";
        case Word_Break::ExtendNumLet: return os << "ExtendNumLet";
        case Word_Break::Format: return os << "Format";
        case Word_Break::Glue_After_Zwj: return os << "Glue_After_Zwj";
        case Word_Break::Hebrew_Letter: return os << "Hebrew_Letter";
        case Word_Break::Katakana: return os << "Katakana";
        case Word_Break::LF: return os << "LF";
        case Word_Break::MidLetter: return os << "MidLetter";
        case Word_Break::MidNum: return os << "MidNum";
        case Word_Break::MidNumLet: return os << "MidNumLet";
        case Word_Break::Newline: return os << "Newline";
        case Word_Break::Numeric: return os << "Numeric";
        case Word_Break::Other: return os << "Other";
        case Word_Break::Regional_Indicator: return os << "Regional_Indicator";
        case Word_Break::Single_Quote: return os << "Single_Quote";
        case Word_Break::WSegSpace: return os << "WSegSpace";
        case Word_Break::ZWJ: return os << "ZWJ";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Core_Property _value) noexcept {
    switch (_value) {
        case Core_Property::Alphabetic: return os << "Alphabetic";
        case Core_Property::Case_Ignorable: return os << "Case_Ignorable";
        case Core_Property::Cased: return os << "Cased";
        case Core_Property::Changes_When_Casefolded: return os << "Changes_When_Casefolded";
        case Core_Property::Changes_When_Casemapped: return os << "Changes_When_Casemapped";
        case Core_Property::Changes_When_Lowercased: return os << "Changes_When_Lowercased";
        case Core_Property::Changes_When_Titlecased: return os << "Changes_When_Titlecased";
        case Core_Property::Changes_When_Uppercased: return os << "Changes_When_Uppercased";
        case Core_Property::Default_Ignorable_Code_Point: return os << "Default_Ignorable_Code_Point";
        case Core_Property::Grapheme_Base: return os << "Grapheme_Base";
        case Core_Property::Grapheme_Extend: return os << "Grapheme_Extend";
        case Core_Property::Grapheme_Link: return os << "Grapheme_Link";
        case Core_Property::ID_Continue: return os << "ID_Continue";
        case Core_Property::ID_Start: return os << "ID_Start";
        case Core_Property::Lowercase: return os << "Lowercase";
        case Core_Property::Math: return os << "Math";
        case Core_Property::Uppercase: return os << "Uppercase";
        case Core_Property::XID_Continue: return os << "XID_Continue";
        case Core_Property::XID_Start: return os << "XID_Start";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, General_Category _value) noexcept {
    switch (_value) {
        case General_Category::Unspecified: return os << "Unspecified";
        case General_Category::Close_Punctuation: return os << "Close_Punctuation";
        case General_Category::Connector_Punctuation: return os << "Connector_Punctuation";
        case General_Category::Control: return os << "Control";
        case General_Category::Currency_Symbol: return os << "Currency_Symbol";
        case General_Category::Dash_Punctuation: return os << "Dash_Punctuation";
        case General_Category::Decimal_Number: return os << "Decimal_Number";
        case General_Category::Enclosing_Mark: return os << "Enclosing_Mark";
        case General_Category::Final_Punctuation: return os << "Final_Punctuation";
        case General_Category::Format: return os << "Format";
        case General_Category::Initial_Punctuation: return os << "Initial_Punctuation";
        case General_Category::Letter_Number: return os << "Letter_Number";
        case General_Category::Line_Separator: return os << "Line_Separator";
        case General_Category::Lowercase_Letter: return os << "Lowercase_Letter";
        case General_Category::Math_Symbol: return os << "Math_Symbol";
        case General_Category::Modifier_Letter: return os << "Modifier_Letter";
        case General_Category::Modifier_Symbol: return os << "Modifier_Symbol";
        case General_Category::Nonspacing_Mark: return os << "Nonspacing_Mark";
        case General_Category::Open_Punctuation: return os << "Open_Punctuation";
        case General_Category::Other_Letter: return os << "Other_Letter";
        case General_Category::Other_Number: return os << "Other_Number";
        case General_Category::Other_Punctuation: return os << "Other_Punctuation";
        case General_Category::Other_Symbol: return os << "Other_Symbol";
        case General_Category::Paragraph_Separator: return os << "Paragraph_Separator";
        case General_Category::Private_Use: return os << "Private_Use";
        case General_Category::Space_Separator: return os << "Space_Separator";
        case General_Category::Spacing_Mark: return os << "Spacing_Mark";
        case General_Category::Surrogate: return os << "Surrogate";
        case General_Category::Titlecase_Letter: return os << "Titlecase_Letter";
        case General_Category::Unassigned: return os << "Unassigned";
        case General_Category::Uppercase_Letter: return os << "Uppercase_Letter";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Script _value) noexcept {
    switch (_value) {
        case Script::Invalid: return os << "Invalid";
        case Script::Unknown: return os << "Unknown";
        case Script::Common: return os << "Common";
        case Script::Adlam: return os << "Adlam";
        case Script::Ahom: return os << "Ahom";
        case Script::Anatolian_Hieroglyphs: return os << "Anatolian_Hieroglyphs";
        case Script::Arabic: return os << "Arabic";
        case Script::Armenian: return os << "Armenian";
        case Script::Avestan: return os << "Avestan";
        case Script::Balinese: return os << "Balinese";
        case Script::Bamum: return os << "Bamum";
        case Script::Bassa_Vah: return os << "Bassa_Vah";
        case Script::Batak: return os << "Batak";
        case Script::Bengali: return os << "Bengali";
        case Script::Bhaiksuki: return os << "Bhaiksuki";
        case Script::Bopomofo: return os << "Bopomofo";
        case Script::Brahmi: return os << "Brahmi";
        case Script::Braille: return os << "Braille";
        case Script::Buginese: return os << "Buginese";
        case Script::Buhid: return os << "Buhid";
        case Script::Canadian_Aboriginal: return os << "Canadian_Aboriginal";
        case Script::Carian: return os << "Carian";
        case Script::Caucasian_Albanian: return os << "Caucasian_Albanian";
        case Script::Chakma: return os << "Chakma";
        case Script::Cham: return os << "Cham";
        case Script::Cherokee: return os << "Cherokee";
        case Script::Chorasmian: return os << "Chorasmian";
        case Script::Coptic: return os << "Coptic";
        case Script::Cuneiform: return os << "Cuneiform";
        case Script::Cypriot: return os << "Cypriot";
        case Script::Cyrillic: return os << "Cyrillic";
        case Script::Deseret: return os << "Deseret";
        case Script::Devanagari: return os << "Devanagari";
        case Script::Dives_Akuru: return os << "Dives_Akuru";
        case Script::Dogra: return os << "Dogra";
        case Script::Duployan: return os << "Duployan";
        case Script::Egyptian_Hieroglyphs: return os << "Egyptian_Hieroglyphs";
        case Script::Elbasan: return os << "Elbasan";
        case Script::Elymaic: return os << "Elymaic";
        case Script::Ethiopic: return os << "Ethiopic";
        case Script::Georgian: return os << "Georgian";
        case Script::Glagolitic: return os << "Glagolitic";
        case Script::Gothic: return os << "Gothic";
        case Script::Grantha: return os << "Grantha";
        case Script::Greek: return os << "Greek";
        case Script::Gujarati: return os << "Gujarati";
        case Script::Gunjala_Gondi: return os << "Gunjala_Gondi";
        case Script::Gurmukhi: return os << "Gurmukhi";
        case Script::Han: return os << "Han";
        case Script::Hangul: return os << "Hangul";
        case Script::Hanifi_Rohingya: return os << "Hanifi_Rohingya";
        case Script::Hanunoo: return os << "Hanunoo";
        case Script::Hatran: return os << "Hatran";
        case Script::Hebrew: return os << "Hebrew";
        case Script::Hiragana: return os << "Hiragana";
        case Script::Imperial_Aramaic: return os << "Imperial_Aramaic";
        case Script::Inherited: return os << "Inherited";
        case Script::Inscriptional_Pahlavi: return os << "Inscriptional_Pahlavi";
        case Script::Inscriptional_Parthian: return os << "Inscriptional_Parthian";
        case Script::Javanese: return os << "Javanese";
        case Script::Kaithi: return os << "Kaithi";
        case Script::Kannada: return os << "Kannada";
        case Script::Katakana: return os << "Katakana";
        case Script::Kayah_Li: return os << "Kayah_Li";
        case Script::Kharoshthi: return os << "Kharoshthi";
        case Script::Khitan_Small_Script: return os << "Khitan_Small_Script";
        case Script::Khmer: return os << "Khmer";
        case Script::Khojki: return os << "Khojki";
        case Script::Khudawadi: return os << "Khudawadi";
        case Script::Lao: return os << "Lao";
        case Script::Latin: return os << "Latin";
        case Script::Lepcha: return os << "Lepcha";
        case Script::Limbu: return os << "Limbu";
        case Script::Linear_A: return os << "Linear_A";
        case Script::Linear_B: return os << "Linear_B";
        case Script::Lisu: return os << "Lisu";
        case Script::Lycian: return os << "Lycian";
        case Script::Lydian: return os << "Lydian";
        case Script::Mahajani: return os << "Mahajani";
        case Script::Makasar: return os << "Makasar";
        case Script::Malayalam: return os << "Malayalam";
        case Script::Mandaic: return os << "Mandaic";
        case Script::Manichaean: return os << "Manichaean";
        case Script::Marchen: return os << "Marchen";
        case Script::Masaram_Gondi: return os << "Masaram_Gondi";
        case Script::Medefaidrin: return os << "Medefaidrin";
        case Script::Meetei_Mayek: return os << "Meetei_Mayek";
        case Script::Mende_Kikakui: return os << "Mende_Kikakui";
        case Script::Meroitic_Cursive: return os << "Meroitic_Cursive";
        case Script::Meroitic_Hieroglyphs: return os << "Meroitic_Hieroglyphs";
        case Script::Miao: return os << "Miao";
        case Script::Modi: return os << "Modi";
        case Script::Mongolian: return os << "Mongolian";
        case Script::Mro: return os << "Mro";
        case Script::Multani: return os << "Multani";
        case Script::Myanmar: return os << "Myanmar";
        case Script::Nabataean: return os << "Nabataean";
        case Script::Nandinagari: return os << "Nandinagari";
        case Script::New_Tai_Lue: return os << "New_Tai_Lue";
        case Script::Newa: return os << "Newa";
        case Script::Nko: return os << "Nko";
        case Script::Nushu: return os << "Nushu";
        case Script::Nyiakeng_Puachue_Hmong: return os << "Nyiakeng_Puachue_Hmong";
        case Script::Ogham: return os << "Ogham";
        case Script::Ol_Chiki: return os << "Ol_Chiki";
        case Script::Old_Hungarian: return os << "Old_Hungarian";
        case Script::Old_Italic: return os << "Old_Italic";
        case Script::Old_North_Arabian: return os << "Old_North_Arabian";
        case Script::Old_Permic: return os << "Old_Permic";
        case Script::Old_Persian: return os << "Old_Persian";
        case Script::Old_Sogdian: return os << "Old_Sogdian";
        case Script::Old_South_Arabian: return os << "Old_South_Arabian";
        case Script::Old_Turkic: return os << "Old_Turkic";
        case Script::Oriya: return os << "Oriya";
        case Script::Osage: return os << "Osage";
        case Script::Osmanya: return os << "Osmanya";
        case Script::Pahawh_Hmong: return os << "Pahawh_Hmong";
        case Script::Palmyrene: return os << "Palmyrene";
        case Script::Pau_Cin_Hau: return os << "Pau_Cin_Hau";
        case Script::Phags_Pa: return os << "Phags_Pa";
        case Script::Phoenician: return os << "Phoenician";
        case Script::Psalter_Pahlavi: return os << "Psalter_Pahlavi";
        case Script::Rejang: return os << "Rejang";
        case Script::Runic: return os << "Runic";
        case Script::Samaritan: return os << "Samaritan";
        case Script::Saurashtra: return os << "Saurashtra";
        case Script::Sharada: return os << "Sharada";
        case Script::Shavian: return os << "Shavian";
        case Script::Siddham: return os << "Siddham";
        case Script::SignWriting: return os << "SignWriting";
        case Script::Sinhala: return os << "Sinhala";
        case Script::Sogdian: return os << "Sogdian";
        case Script::Sora_Sompeng: return os << "Sora_Sompeng";
        case Script::Soyombo: return os << "Soyombo";
        case Script::Sundanese: return os << "Sundanese";
        case Script::Syloti_Nagri: return os << "Syloti_Nagri";
        case Script::Syriac: return os << "Syriac";
        case Script::Tagalog: return os << "Tagalog";
        case Script::Tagbanwa: return os << "Tagbanwa";
        case Script::Tai_Le: return os << "Tai_Le";
        case Script::Tai_Tham: return os << "Tai_Tham";
        case Script::Tai_Viet: return os << "Tai_Viet";
        case Script::Takri: return os << "Takri";
        case Script::Tamil: return os << "Tamil";
        case Script::Tangut: return os << "Tangut";
        case Script::Telugu: return os << "Telugu";
        case Script::Thaana: return os << "Thaana";
        case Script::Thai: return os << "Thai";
        case Script::Tibetan: return os << "Tibetan";
        case Script::Tifinagh: return os << "Tifinagh";
        case Script::Tirhuta: return os << "Tirhuta";
        case Script::Ugaritic: return os << "Ugaritic";
        case Script::Vai: return os << "Vai";
        case Script::Wancho: return os << "Wancho";
        case Script::Warang_Citi: return os << "Warang_Citi";
        case Script::Yezidi: return os << "Yezidi";
        case Script::Yi: return os << "Yi";
        case Script::Zanabazar_Square: return os << "Zanabazar_Square";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Block _value) noexcept {
    switch (_value) {
        case Block::Unspecified: return os << "Unspecified";
        case Block::Adlam: return os << "Adlam";
        case Block::Aegean_Numbers: return os << "Aegean_Numbers";
        case Block::Ahom: return os << "Ahom";
        case Block::Alchemical_Symbols: return os << "Alchemical_Symbols";
        case Block::Alphabetic_Presentation_Forms: return os << "Alphabetic_Presentation_Forms";
        case Block::Anatolian_Hieroglyphs: return os << "Anatolian_Hieroglyphs";
        case Block::Ancient_Greek_Musical_Notation: return os << "Ancient_Greek_Musical_Notation";
        case Block::Ancient_Greek_Numbers: return os << "Ancient_Greek_Numbers";
        case Block::Ancient_Symbols: return os << "Ancient_Symbols";
        case Block::Arabic: return os << "Arabic";
        case Block::Arabic_Extended_A: return os << "Arabic_Extended_A";
        case Block::Arabic_Mathematical_Alphabetic_Symbols: return os << "Arabic_Mathematical_Alphabetic_Symbols";
        case Block::Arabic_Presentation_Forms_A: return os << "Arabic_Presentation_Forms_A";
        case Block::Arabic_Presentation_Forms_B: return os << "Arabic_Presentation_Forms_B";
        case Block::Arabic_Supplement: return os << "Arabic_Supplement";
        case Block::Armenian: return os << "Armenian";
        case Block::Arrows: return os << "Arrows";
        case Block::Avestan: return os << "Avestan";
        case Block::Balinese: return os << "Balinese";
        case Block::Bamum: return os << "Bamum";
        case Block::Bamum_Supplement: return os << "Bamum_Supplement";
        case Block::Basic_Latin: return os << "Basic_Latin";
        case Block::Bassa_Vah: return os << "Bassa_Vah";
        case Block::Batak: return os << "Batak";
        case Block::Bengali: return os << "Bengali";
        case Block::Bhaiksuki: return os << "Bhaiksuki";
        case Block::Block_Elements: return os << "Block_Elements";
        case Block::Bopomofo: return os << "Bopomofo";
        case Block::Bopomofo_Extended: return os << "Bopomofo_Extended";
        case Block::Box_Drawing: return os << "Box_Drawing";
        case Block::Brahmi: return os << "Brahmi";
        case Block::Braille_Patterns: return os << "Braille_Patterns";
        case Block::Buginese: return os << "Buginese";
        case Block::Buhid: return os << "Buhid";
        case Block::Byzantine_Musical_Symbols: return os << "Byzantine_Musical_Symbols";
        case Block::CJK_Compatibility: return os << "CJK_Compatibility";
        case Block::CJK_Compatibility_Forms: return os << "CJK_Compatibility_Forms";
        case Block::CJK_Compatibility_Ideographs: return os << "CJK_Compatibility_Ideographs";
        case Block::CJK_Compatibility_Ideographs_Supplement: return os << "CJK_Compatibility_Ideographs_Supplement";
        case Block::CJK_Radicals_Supplement: return os << "CJK_Radicals_Supplement";
        case Block::CJK_Strokes: return os << "CJK_Strokes";
        case Block::CJK_Symbols_and_Punctuation: return os << "CJK_Symbols_and_Punctuation";
        case Block::CJK_Unified_Ideographs: return os << "CJK_Unified_Ideographs";
        case Block::CJK_Unified_Ideographs_Extension_A: return os << "CJK_Unified_Ideographs_Extension_A";
        case Block::CJK_Unified_Ideographs_Extension_B: return os << "CJK_Unified_Ideographs_Extension_B";
        case Block::CJK_Unified_Ideographs_Extension_C: return os << "CJK_Unified_Ideographs_Extension_C";
        case Block::CJK_Unified_Ideographs_Extension_D: return os << "CJK_Unified_Ideographs_Extension_D";
        case Block::CJK_Unified_Ideographs_Extension_E: return os << "CJK_Unified_Ideographs_Extension_E";
        case Block::CJK_Unified_Ideographs_Extension_F: return os << "CJK_Unified_Ideographs_Extension_F";
        case Block::CJK_Unified_Ideographs_Extension_G: return os << "CJK_Unified_Ideographs_Extension_G";
        case Block::Carian: return os << "Carian";
        case Block::Caucasian_Albanian: return os << "Caucasian_Albanian";
        case Block::Chakma: return os << "Chakma";
        case Block::Cham: return os << "Cham";
        case Block::Cherokee: return os << "Cherokee";
        case Block::Cherokee_Supplement: return os << "Cherokee_Supplement";
        case Block::Chess_Symbols: return os << "Chess_Symbols";
        case Block::Chorasmian: return os << "Chorasmian";
        case Block::Combining_Diacritical_Marks: return os << "Combining_Diacritical_Marks";
        case Block::Combining_Diacritical_Marks_Extended: return os << "Combining_Diacritical_Marks_Extended";
        case Block::Combining_Diacritical_Marks_Supplement: return os << "Combining_Diacritical_Marks_Supplement";
        case Block::Combining_Diacritical_Marks_for_Symbols: return os << "Combining_Diacritical_Marks_for_Symbols";
        case Block::Combining_Half_Marks: return os << "Combining_Half_Marks";
        case Block::Common_Indic_Number_Forms: return os << "Common_Indic_Number_Forms";
        case Block::Control_Pictures: return os << "Control_Pictures";
        case Block::Coptic: return os << "Coptic";
        case Block::Coptic_Epact_Numbers: return os << "Coptic_Epact_Numbers";
        case Block::Counting_Rod_Numerals: return os << "Counting_Rod_Numerals";
        case Block::Cuneiform: return os << "Cuneiform";
        case Block::Cuneiform_Numbers_and_Punctuation: return os << "Cuneiform_Numbers_and_Punctuation";
        case Block::Currency_Symbols: return os << "Currency_Symbols";
        case Block::Cypriot_Syllabary: return os << "Cypriot_Syllabary";
        case Block::Cyrillic: return os << "Cyrillic";
        case Block::Cyrillic_Extended_A: return os << "Cyrillic_Extended_A";
        case Block::Cyrillic_Extended_B: return os << "Cyrillic_Extended_B";
        case Block::Cyrillic_Extended_C: return os << "Cyrillic_Extended_C";
        case Block::Cyrillic_Supplement: return os << "Cyrillic_Supplement";
        case Block::Deseret: return os << "Deseret";
        case Block::Devanagari: return os << "Devanagari";
        case Block::Devanagari_Extended: return os << "Devanagari_Extended";
        case Block::Dingbats: return os << "Dingbats";
        case Block::Dives_Akuru: return os << "Dives_Akuru";
        case Block::Dogra: return os << "Dogra";
        case Block::Domino_Tiles: return os << "Domino_Tiles";
        case Block::Duployan: return os << "Duployan";
        case Block::Early_Dynastic_Cuneiform: return os << "Early_Dynastic_Cuneiform";
        case Block::Egyptian_Hieroglyph_Format_Controls: return os << "Egyptian_Hieroglyph_Format_Controls";
        case Block::Egyptian_Hieroglyphs: return os << "Egyptian_Hieroglyphs";
        case Block::Elbasan: return os << "Elbasan";
        case Block::Elymaic: return os << "Elymaic";
        case Block::Emoticons: return os << "Emoticons";
        case Block::Enclosed_Alphanumeric_Supplement: return os << "Enclosed_Alphanumeric_Supplement";
        case Block::Enclosed_Alphanumerics: return os << "Enclosed_Alphanumerics";
        case Block::Enclosed_CJK_Letters_and_Months: return os << "Enclosed_CJK_Letters_and_Months";
        case Block::Enclosed_Ideographic_Supplement: return os << "Enclosed_Ideographic_Supplement";
        case Block::Ethiopic: return os << "Ethiopic";
        case Block::Ethiopic_Extended: return os << "Ethiopic_Extended";
        case Block::Ethiopic_Extended_A: return os << "Ethiopic_Extended_A";
        case Block::Ethiopic_Supplement: return os << "Ethiopic_Supplement";
        case Block::General_Punctuation: return os << "General_Punctuation";
        case Block::Geometric_Shapes: return os << "Geometric_Shapes";
        case Block::Geometric_Shapes_Extended: return os << "Geometric_Shapes_Extended";
        case Block::Georgian: return os << "Georgian";
        case Block::Georgian_Extended: return os << "Georgian_Extended";
        case Block::Georgian_Supplement: return os << "Georgian_Supplement";
        case Block::Glagolitic: return os << "Glagolitic";
        case Block::Glagolitic_Supplement: return os << "Glagolitic_Supplement";
        case Block::Gothic: return os << "Gothic";
        case Block::Grantha: return os << "Grantha";
        case Block::Greek_Extended: return os << "Greek_Extended";
        case Block::Greek_and_Coptic: return os << "Greek_and_Coptic";
        case Block::Gujarati: return os << "Gujarati";
        case Block::Gunjala_Gondi: return os << "Gunjala_Gondi";
        case Block::Gurmukhi: return os << "Gurmukhi";
        case Block::Halfwidth_and_Fullwidth_Forms: return os << "Halfwidth_and_Fullwidth_Forms";
        case Block::Hangul_Compatibility_Jamo: return os << "Hangul_Compatibility_Jamo";
        case Block::Hangul_Jamo: return os << "Hangul_Jamo";
        case Block::Hangul_Jamo_Extended_A: return os << "Hangul_Jamo_Extended_A";
        case Block::Hangul_Jamo_Extended_B: return os << "Hangul_Jamo_Extended_B";
        case Block::Hangul_Syllables: return os << "Hangul_Syllables";
        case Block::Hanifi_Rohingya: return os << "Hanifi_Rohingya";
        case Block::Hanunoo: return os << "Hanunoo";
        case Block::Hatran: return os << "Hatran";
        case Block::Hebrew: return os << "Hebrew";
        case Block::High_Private_Use_Surrogates: return os << "High_Private_Use_Surrogates";
        case Block::High_Surrogates: return os << "High_Surrogates";
        case Block::Hiragana: return os << "Hiragana";
        case Block::IPA_Extensions: return os << "IPA_Extensions";
        case Block::Ideographic_Description_Characters: return os << "Ideographic_Description_Characters";
        case Block::Ideographic_Symbols_and_Punctuation: return os << "Ideographic_Symbols_and_Punctuation";
        case Block::Imperial_Aramaic: return os << "Imperial_Aramaic";
        case Block::Indic_Siyaq_Numbers: return os << "Indic_Siyaq_Numbers";
        case Block::Inscriptional_Pahlavi: return os << "Inscriptional_Pahlavi";
        case Block::Inscriptional_Parthian: return os << "Inscriptional_Parthian";
        case Block::Javanese: return os << "Javanese";
        case Block::Kaithi: return os << "Kaithi";
        case Block::Kana_Extended_A: return os << "Kana_Extended_A";
        case Block::Kana_Supplement: return os << "Kana_Supplement";
        case Block::Kanbun: return os << "Kanbun";
        case Block::Kangxi_Radicals: return os << "Kangxi_Radicals";
        case Block::Kannada: return os << "Kannada";
        case Block::Katakana: return os << "Katakana";
        case Block::Katakana_Phonetic_Extensions: return os << "Katakana_Phonetic_Extensions";
        case Block::Kayah_Li: return os << "Kayah_Li";
        case Block::Kharoshthi: return os << "Kharoshthi";
        case Block::Khitan_Small_Script: return os << "Khitan_Small_Script";
        case Block::Khmer: return os << "Khmer";
        case Block::Khmer_Symbols: return os << "Khmer_Symbols";
        case Block::Khojki: return os << "Khojki";
        case Block::Khudawadi: return os << "Khudawadi";
        case Block::Lao: return os << "Lao";
        case Block::Latin_Extended_Additional: return os << "Latin_Extended_Additional";
        case Block::Latin_Extended_A: return os << "Latin_Extended_A";
        case Block::Latin_Extended_B: return os << "Latin_Extended_B";
        case Block::Latin_Extended_C: return os << "Latin_Extended_C";
        case Block::Latin_Extended_D: return os << "Latin_Extended_D";
        case Block::Latin_Extended_E: return os << "Latin_Extended_E";
        case Block::Latin_1_Supplement: return os << "Latin_1_Supplement";
        case Block::Lepcha: return os << "Lepcha";
        case Block::Letterlike_Symbols: return os << "Letterlike_Symbols";
        case Block::Limbu: return os << "Limbu";
        case Block::Linear_A: return os << "Linear_A";
        case Block::Linear_B_Ideograms: return os << "Linear_B_Ideograms";
        case Block::Linear_B_Syllabary: return os << "Linear_B_Syllabary";
        case Block::Lisu: return os << "Lisu";
        case Block::Lisu_Supplement: return os << "Lisu_Supplement";
        case Block::Low_Surrogates: return os << "Low_Surrogates";
        case Block::Lycian: return os << "Lycian";
        case Block::Lydian: return os << "Lydian";
        case Block::Mahajani: return os << "Mahajani";
        case Block::Mahjong_Tiles: return os << "Mahjong_Tiles";
        case Block::Makasar: return os << "Makasar";
        case Block::Malayalam: return os << "Malayalam";
        case Block::Mandaic: return os << "Mandaic";
        case Block::Manichaean: return os << "Manichaean";
        case Block::Marchen: return os << "Marchen";
        case Block::Masaram_Gondi: return os << "Masaram_Gondi";
        case Block::Mathematical_Alphanumeric_Symbols: return os << "Mathematical_Alphanumeric_Symbols";
        case Block::Mathematical_Operators: return os << "Mathematical_Operators";
        case Block::Mayan_Numerals: return os << "Mayan_Numerals";
        case Block::Medefaidrin: return os << "Medefaidrin";
        case Block::Meetei_Mayek: return os << "Meetei_Mayek";
        case Block::Meetei_Mayek_Extensions: return os << "Meetei_Mayek_Extensions";
        case Block::Mende_Kikakui: return os << "Mende_Kikakui";
        case Block::Meroitic_Cursive: return os << "Meroitic_Cursive";
        case Block::Meroitic_Hieroglyphs: return os << "Meroitic_Hieroglyphs";
        case Block::Miao: return os << "Miao";
        case Block::Miscellaneous_Mathematical_Symbols_A: return os << "Miscellaneous_Mathematical_Symbols_A";
        case Block::Miscellaneous_Mathematical_Symbols_B: return os << "Miscellaneous_Mathematical_Symbols_B";
        case Block::Miscellaneous_Symbols: return os << "Miscellaneous_Symbols";
        case Block::Miscellaneous_Symbols_and_Arrows: return os << "Miscellaneous_Symbols_and_Arrows";
        case Block::Miscellaneous_Symbols_and_Pictographs: return os << "Miscellaneous_Symbols_and_Pictographs";
        case Block::Miscellaneous_Technical: return os << "Miscellaneous_Technical";
        case Block::Modi: return os << "Modi";
        case Block::Modifier_Tone_Letters: return os << "Modifier_Tone_Letters";
        case Block::Mongolian: return os << "Mongolian";
        case Block::Mongolian_Supplement: return os << "Mongolian_Supplement";
        case Block::Mro: return os << "Mro";
        case Block::Multani: return os << "Multani";
        case Block::Musical_Symbols: return os << "Musical_Symbols";
        case Block::Myanmar: return os << "Myanmar";
        case Block::Myanmar_Extended_A: return os << "Myanmar_Extended_A";
        case Block::Myanmar_Extended_B: return os << "Myanmar_Extended_B";
        case Block::NKo: return os << "NKo";
        case Block::Nabataean: return os << "Nabataean";
        case Block::Nandinagari: return os << "Nandinagari";
        case Block::New_Tai_Lue: return os << "New_Tai_Lue";
        case Block::Newa: return os << "Newa";
        case Block::Number_Forms: return os << "Number_Forms";
        case Block::Nushu: return os << "Nushu";
        case Block::Nyiakeng_Puachue_Hmong: return os << "Nyiakeng_Puachue_Hmong";
        case Block::Ogham: return os << "Ogham";
        case Block::Ol_Chiki: return os << "Ol_Chiki";
        case Block::Old_Hungarian: return os << "Old_Hungarian";
        case Block::Old_Italic: return os << "Old_Italic";
        case Block::Old_North_Arabian: return os << "Old_North_Arabian";
        case Block::Old_Permic: return os << "Old_Permic";
        case Block::Old_Persian: return os << "Old_Persian";
        case Block::Old_Sogdian: return os << "Old_Sogdian";
        case Block::Old_South_Arabian: return os << "Old_South_Arabian";
        case Block::Old_Turkic: return os << "Old_Turkic";
        case Block::Optical_Character_Recognition: return os << "Optical_Character_Recognition";
        case Block::Oriya: return os << "Oriya";
        case Block::Ornamental_Dingbats: return os << "Ornamental_Dingbats";
        case Block::Osage: return os << "Osage";
        case Block::Osmanya: return os << "Osmanya";
        case Block::Ottoman_Siyaq_Numbers: return os << "Ottoman_Siyaq_Numbers";
        case Block::Pahawh_Hmong: return os << "Pahawh_Hmong";
        case Block::Palmyrene: return os << "Palmyrene";
        case Block::Pau_Cin_Hau: return os << "Pau_Cin_Hau";
        case Block::Phags_pa: return os << "Phags_pa";
        case Block::Phaistos_Disc: return os << "Phaistos_Disc";
        case Block::Phoenician: return os << "Phoenician";
        case Block::Phonetic_Extensions: return os << "Phonetic_Extensions";
        case Block::Phonetic_Extensions_Supplement: return os << "Phonetic_Extensions_Supplement";
        case Block::Playing_Cards: return os << "Playing_Cards";
        case Block::Private_Use_Area: return os << "Private_Use_Area";
        case Block::Psalter_Pahlavi: return os << "Psalter_Pahlavi";
        case Block::Rejang: return os << "Rejang";
        case Block::Rumi_Numeral_Symbols: return os << "Rumi_Numeral_Symbols";
        case Block::Runic: return os << "Runic";
        case Block::Samaritan: return os << "Samaritan";
        case Block::Saurashtra: return os << "Saurashtra";
        case Block::Sharada: return os << "Sharada";
        case Block::Shavian: return os << "Shavian";
        case Block::Shorthand_Format_Controls: return os << "Shorthand_Format_Controls";
        case Block::Siddham: return os << "Siddham";
        case Block::Sinhala: return os << "Sinhala";
        case Block::Sinhala_Archaic_Numbers: return os << "Sinhala_Archaic_Numbers";
        case Block::Small_Form_Variants: return os << "Small_Form_Variants";
        case Block::Small_Kana_Extension: return os << "Small_Kana_Extension";
        case Block::Sogdian: return os << "Sogdian";
        case Block::Sora_Sompeng: return os << "Sora_Sompeng";
        case Block::Soyombo: return os << "Soyombo";
        case Block::Spacing_Modifier_Letters: return os << "Spacing_Modifier_Letters";
        case Block::Specials: return os << "Specials";
        case Block::Sundanese: return os << "Sundanese";
        case Block::Sundanese_Supplement: return os << "Sundanese_Supplement";
        case Block::Superscripts_and_Subscripts: return os << "Superscripts_and_Subscripts";
        case Block::Supplemental_Arrows_A: return os << "Supplemental_Arrows_A";
        case Block::Supplemental_Arrows_B: return os << "Supplemental_Arrows_B";
        case Block::Supplemental_Arrows_C: return os << "Supplemental_Arrows_C";
        case Block::Supplemental_Mathematical_Operators: return os << "Supplemental_Mathematical_Operators";
        case Block::Supplemental_Punctuation: return os << "Supplemental_Punctuation";
        case Block::Supplemental_Symbols_and_Pictographs: return os << "Supplemental_Symbols_and_Pictographs";
        case Block::Supplementary_Private_Use_Area_A: return os << "Supplementary_Private_Use_Area_A";
        case Block::Supplementary_Private_Use_Area_B: return os << "Supplementary_Private_Use_Area_B";
        case Block::Sutton_SignWriting: return os << "Sutton_SignWriting";
        case Block::Syloti_Nagri: return os << "Syloti_Nagri";
        case Block::Symbols_and_Pictographs_Extended_A: return os << "Symbols_and_Pictographs_Extended_A";
        case Block::Symbols_for_Legacy_Computing: return os << "Symbols_for_Legacy_Computing";
        case Block::Syriac: return os << "Syriac";
        case Block::Syriac_Supplement: return os << "Syriac_Supplement";
        case Block::Tagalog: return os << "Tagalog";
        case Block::Tagbanwa: return os << "Tagbanwa";
        case Block::Tags: return os << "Tags";
        case Block::Tai_Le: return os << "Tai_Le";
        case Block::Tai_Tham: return os << "Tai_Tham";
        case Block::Tai_Viet: return os << "Tai_Viet";
        case Block::Tai_Xuan_Jing_Symbols: return os << "Tai_Xuan_Jing_Symbols";
        case Block::Takri: return os << "Takri";
        case Block::Tamil: return os << "Tamil";
        case Block::Tamil_Supplement: return os << "Tamil_Supplement";
        case Block::Tangut: return os << "Tangut";
        case Block::Tangut_Components: return os << "Tangut_Components";
        case Block::Tangut_Supplement: return os << "Tangut_Supplement";
        case Block::Telugu: return os << "Telugu";
        case Block::Thaana: return os << "Thaana";
        case Block::Thai: return os << "Thai";
        case Block::Tibetan: return os << "Tibetan";
        case Block::Tifinagh: return os << "Tifinagh";
        case Block::Tirhuta: return os << "Tirhuta";
        case Block::Transport_and_Map_Symbols: return os << "Transport_and_Map_Symbols";
        case Block::Ugaritic: return os << "Ugaritic";
        case Block::Unified_Canadian_Aboriginal_Syllabics: return os << "Unified_Canadian_Aboriginal_Syllabics";
        case Block::Unified_Canadian_Aboriginal_Syllabics_Extended: return os << "Unified_Canadian_Aboriginal_Syllabics_Extended";
        case Block::Vai: return os << "Vai";
        case Block::Variation_Selectors: return os << "Variation_Selectors";
        case Block::Variation_Selectors_Supplement: return os << "Variation_Selectors_Supplement";
        case Block::Vedic_Extensions: return os << "Vedic_Extensions";
        case Block::Vertical_Forms: return os << "Vertical_Forms";
        case Block::Wancho: return os << "Wancho";
        case Block::Warang_Citi: return os << "Warang_Citi";
        case Block::Yezidi: return os << "Yezidi";
        case Block::Yi_Radicals: return os << "Yi_Radicals";
        case Block::Yi_Syllables: return os << "Yi_Syllables";
        case Block::Yijing_Hexagram_Symbols: return os << "Yijing_Hexagram_Symbols";
        case Block::Zanabazar_Square: return os << "Zanabazar_Square";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, Grapheme_Cluster_Break _value) noexcept {
    switch (_value) {
        case Grapheme_Cluster_Break::CR: return os << "CR";
        case Grapheme_Cluster_Break::Control: return os << "Control";
        case Grapheme_Cluster_Break::Extend: return os << "Extend";
        case Grapheme_Cluster_Break::L: return os << "L";
        case Grapheme_Cluster_Break::LF: return os << "LF";
        case Grapheme_Cluster_Break::LV: return os << "LV";
        case Grapheme_Cluster_Break::LVT: return os << "LVT";
        case Grapheme_Cluster_Break::Prepend: return os << "Prepend";
        case Grapheme_Cluster_Break::Regional_Indicator: return os << "Regional_Indicator";
        case Grapheme_Cluster_Break::SpacingMark: return os << "SpacingMark";
        case Grapheme_Cluster_Break::T: return os << "T";
        case Grapheme_Cluster_Break::V: return os << "V";
        case Grapheme_Cluster_Break::ZWJ: return os << "ZWJ";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

inline std::ostream& operator<<(std::ostream& os, EastAsianWidth _value) noexcept {
    switch (_value) {
        case EastAsianWidth::Ambiguous: return os << "Ambiguous";
        case EastAsianWidth::FullWidth: return os << "FullWidth";
        case EastAsianWidth::HalfWidth: return os << "HalfWidth";
        case EastAsianWidth::Neutral: return os << "Neutral";
        case EastAsianWidth::Narrow: return os << "Narrow";
        case EastAsianWidth::Wide: return os << "Wide";
        case EastAsianWidth::Unspecified: return os << "Unspecified";
    }
    return os << "(" << static_cast<unsigned>(_value) << ")";
}

} // end namespace
