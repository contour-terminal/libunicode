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

#include <unicode/emoji_segmenter.h> // Only for EmojiSegmentationCategory.
#include <unicode/support.h>         // Only for LIBUNICODE_PACKED.
#include <unicode/support/multistage_table_view.h>
#include <unicode/ucd_enums.h> // Only for the UCD enums.

#include <gsl/span>

#include <type_traits>

namespace unicode
{

struct LIBUNICODE_PACKED codepoint_properties
{
    uint8_t char_width = 0;
    uint8_t flags = 0;
    Script script = Script::Unknown;
    Grapheme_Cluster_Break grapheme_cluster_break = Grapheme_Cluster_Break::Other;
    East_Asian_Width east_asian_width = East_Asian_Width::Narrow;
    General_Category general_category = General_Category::Unassigned;
    EmojiSegmentationCategory emoji_segmentation_category = EmojiSegmentationCategory::Invalid;
    Age age = Age::Unassigned;

    static uint8_t constexpr FlagEmoji = 0x01;                // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagEmojiPresentation = 0x02;    // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagEmojiComponent = 0x04;       // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagEmojiModifier = 0x08;        // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagEmojiModifierBase = 0x10;    // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagExtendedPictographic = 0x20; // NOLINT(readability-identifier-naming)
    static uint8_t constexpr FlagCoreGraphemeExtend = 0x40;   // NOLINT(readability-identifier-naming)

    constexpr bool emoji() const noexcept { return flags & FlagEmoji; }
    constexpr bool emoji_presentation() const noexcept { return flags & FlagEmojiPresentation; }
    constexpr bool emoji_component() const noexcept { return flags & FlagEmojiComponent; }
    constexpr bool emoji_modifier() const noexcept { return flags & FlagEmojiModifier; }
    constexpr bool emoji_modifier_base() const noexcept { return flags & FlagEmojiModifierBase; }
    constexpr bool extended_pictographic() const noexcept { return flags & FlagExtendedPictographic; }
    constexpr bool core_grapheme_extend() const noexcept { return flags & FlagCoreGraphemeExtend; }

    using tables_view = support::multistage_table_view<codepoint_properties,
                                                       uint32_t,     // source type
                                                       uint8_t,      // stage 1
                                                       uint16_t,     // stage 2
                                                       256,          // block size
                                                       0x110'000 - 1 // max value
                                                       >;

    static tables_view configured_tables;

    /// Retrieves the codepoint properties for the given codepoint.
    [[nodiscard]] static codepoint_properties const& get(char32_t codepoint) noexcept
    {
        return configured_tables.get(codepoint);
    }
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
