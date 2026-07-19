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

#include <libunicode/emoji_segmenter.h> // Only for EmojiSegmentationCategory.
#include <libunicode/multistage_table_view.h>
#include <libunicode/support.h>   // Only for LIBUNICODE_PACKED.
#include <libunicode/ucd_enums.h> // Only for the UCD enums.

#include <type_traits>

namespace unicode
{

struct LIBUNICODE_PACKED codepoint_properties
{
    uint8_t char_width = 0;
    uint8_t flags = 0;
    uint8_t flags2 = 0;
    Script script = Script::Unknown;
    Grapheme_Cluster_Break grapheme_cluster_break = Grapheme_Cluster_Break::Other;
    East_Asian_Width east_asian_width = East_Asian_Width::Narrow;
    General_Category general_category = General_Category::Unassigned;
    EmojiSegmentationCategory emoji_segmentation_category = EmojiSegmentationCategory::Invalid;
    Age age = Age::Unassigned;
    Indic_Conjunct_Break indic_conjunct_break = Indic_Conjunct_Break::None;
    Word_Break word_break = Word_Break::Other;

    static uint8_t constexpr FlagEmoji = 0x01;                // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagEmojiPresentation = 0x02;    // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagEmojiComponent = 0x04;       // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagEmojiModifier = 0x08;        // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagEmojiModifierBase = 0x10;    // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagExtendedPictographic = 0x20; // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagCoreGraphemeExtend = 0x40;   // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagVirama = 0x80;               // NOLINT(readability-identifier-naming)

    // `flags` is exhausted; further single-bit properties live in `flags2`.
    static uint8_t constexpr Flag2EmojiVariationBase = 0x01; // NOLINT(readability-identifier-naming)

    constexpr bool is_emoji() const noexcept { return flags & FlagEmoji; }
    constexpr bool is_emoji_presentation() const noexcept { return flags & FlagEmojiPresentation; }
    constexpr bool is_emoji_component() const noexcept { return flags & FlagEmojiComponent; }
    constexpr bool is_emoji_modifier() const noexcept { return flags & FlagEmojiModifier; }
    constexpr bool is_emoji_modifier_base() const noexcept { return flags & FlagEmojiModifierBase; }
    constexpr bool is_extended_pictographic() const noexcept { return flags & FlagExtendedPictographic; }
    constexpr bool is_core_grapheme_extend() const noexcept { return flags & FlagCoreGraphemeExtend; }

    /// Whether an emoji variation sequence is defined for this codepoint, i.e. whether a following
    /// VS15 or VS16 is defined to re-present it.
    ///
    /// Which of the two selectors has an effect follows from this codepoint's own width: a base that
    /// is one column wide is what VS16 widens, and one that is two columns wide is what VS15
    /// narrows. Together those two subsets reproduce wcwidth's VS16_NARROW_TO_WIDE and
    /// VS15_WIDE_TO_NARROW tables exactly.
    constexpr bool is_emoji_variation_base() const noexcept { return flags2 & Flag2EmojiVariationBase; }

    /// Indic_Syllabic_Category=Virama or =Invisible_Stacker: a codepoint that suppresses the
    /// inherent vowel of the consonant it follows, joining it to the next into a conjunct.
    ///
    /// Deliberately the FULL virama set rather than Indic_Conjunct_Break=Linker. Linker is not
    /// script-limited the way it is sometimes described -- in UCD 17.0 it already covers Khmer,
    /// Myanmar, Javanese, Chakma and Tai Tham -- but it is a smaller set: 20 codepoints against 41.
    /// The 21 it omits are viramas that do not form InCB conjuncts, among them Gurmukhi U+0A4D,
    /// Tamil U+0BCD, Kannada U+0CCD, Sinhala U+0DCA and the Brahmi-family stackers. They still
    /// stack a consonant onto the previous one, so they still widen the cluster.
    constexpr bool is_virama() const noexcept { return flags & FlagVirama; }

    using tables_view = support::multistage_table_view<codepoint_properties,
                                                       uint32_t,     // source type
                                                       uint8_t,      // stage 1
                                                       uint16_t,     // stage 2
                                                       256,          // block size
                                                       0x110'000 - 1 // max value
                                                       >;

    using names_view = support::multistage_table_view<std::string_view,
                                                      uint32_t,     // source type
                                                      uint8_t,      // stage 1
                                                      uint16_t,     // stage 2
                                                      256,          // block size
                                                      0x110'000 - 1 // max value
                                                      >;

    static tables_view configured_tables;
    static names_view configured_names;

    /// Retrieves the codepoint properties for the given codepoint.
    [[nodiscard]] static codepoint_properties get(char32_t codepoint) noexcept { return configured_tables.get(codepoint); }

    [[nodiscard]] static std::string_view name(char32_t codepoint) { return configured_names.get(codepoint); }
};

static_assert(std::has_unique_object_representations_v<codepoint_properties>);

constexpr bool operator==(codepoint_properties const& a, codepoint_properties const& b) noexcept
{
    return __builtin_memcmp(&a, &b, sizeof(codepoint_properties)) == 0;
}

constexpr bool operator!=(codepoint_properties const& a, codepoint_properties const& b) noexcept
{
    return !(a == b);
}

} // namespace unicode
