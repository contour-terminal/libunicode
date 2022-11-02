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
#include <unicode/codepoint_properties.h>
#include <unicode/ucd.h>
#include <unicode/width.h>

namespace unicode
{

int width(char32_t codepoint)
{
    // Small optimization to speadup US-ASCII width calculation.
    if (0x20 <= codepoint && codepoint <= 0xA0)
        return 1;

    // TODO: make this at most one lookup
    auto const& properties = codepoint_properties::get(codepoint);

    switch (properties.general_category)
    {
        case General_Category::Control: // XXX really?
        case General_Category::Enclosing_Mark:
        case General_Category::Format:
        case General_Category::Line_Separator:
        // case General_Category::Modifier_Symbol:
        case General_Category::Nonspacing_Mark:
        case General_Category::Paragraph_Separator:
        case General_Category::Spacing_Mark:
        case General_Category::Surrogate: return 0;
        default: break;
    }

    if (properties.emoji_presentation())
        // UAX #11 §5 Recommendations:
        //     [UTS51] emoji presentation sequences behave as though they were East Asian Wide,
        //     regardless of their assigned East_Asian_Width property value.
        return 2;

    switch (properties.east_asian_width)
    {
        case East_Asian_Width::Narrow:
        case East_Asian_Width::Ambiguous:
        case East_Asian_Width::Halfwidth:
        case East_Asian_Width::Neutral: return 1;
        case East_Asian_Width::Wide:
        case East_Asian_Width::Fullwidth: return 2;
    }

    return 1;
}

} // namespace unicode
