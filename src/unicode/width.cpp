#include <unicode/width.h>
#include <unicode/ucd.h>

namespace unicode {

int width(char32_t _codepoint)
{
    // TODO construct  table that also contains widths precomputed. also considering Mn/L
    // auto const w = east_asian_width(_codepoint);
    // printf("wcwidth: U+%08X -> %s\n", static_cast<unsigned>(_codepoint), to_string(w).c_str());
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
