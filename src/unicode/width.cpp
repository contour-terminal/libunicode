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
#include <unicode/ucd.h>
#include <unicode/width.h>

namespace unicode
{

int width(char32_t _codepoint)
{
    // Small optimization to speadup US-ASCII width calculation.
    if (0x20 <= _codepoint && _codepoint <= 0xA0)
        return 1;

    // TODO: make this at most one lookup

    switch (general_category::get(_codepoint))
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

    switch (east_asian_width(_codepoint))
    {
    case EastAsianWidth::Narrow:
    case EastAsianWidth::Ambiguous:
    case EastAsianWidth::HalfWidth:
    case EastAsianWidth::Neutral: return 1;
    case EastAsianWidth::Wide:
    case EastAsianWidth::FullWidth: return 2;
    case EastAsianWidth::Unspecified: return 1;
    }

    return 1;
}

} // namespace unicode
