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

#include <ostream>
#include <unicode/ucd.h>

namespace unicode {

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
        case Script::Common: return os << "Common";
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
