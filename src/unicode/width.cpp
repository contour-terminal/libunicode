#include <unicode/width.h>
#include <unicode/ucd.h>

namespace unicode {

// -> width = 1

int width(char32_t _codepoint)
{
    // TODO: make this at most one lookup

    switch (general_category::get(_codepoint))
    {
        case General_Category::Control: // XXX really?
        case General_Category::Enclosing_Mark:
        case General_Category::Format:
        case General_Category::Line_Separator:
        //case General_Category::Modifier_Symbol:
        case General_Category::Nonspacing_Mark:
        case General_Category::Paragraph_Separator:
        case General_Category::Spacing_Mark:
        case General_Category::Surrogate:
            return 0;
        default:
            break;
    }

    switch (east_asian_width(_codepoint))
    {
        case EastAsianWidth::Narrow:
        case EastAsianWidth::Ambiguous:
        case EastAsianWidth::HalfWidth:
        case EastAsianWidth::Neutral:
            return 1;
        case EastAsianWidth::Wide:
        case EastAsianWidth::FullWidth:
            return 2;
        case EastAsianWidth::Unspecified:
            return 1;
    }

    return 1;
}

} // end namespace
