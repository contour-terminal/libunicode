#pragma once

#include <unicode/utf16.h>
#include <unicode/utf8.h>
#include <string_view>
#include <string>

namespace unicode {

// XXX This is experimental API.
inline std::u32string from_wstring(std::basic_string_view<wchar_t> _str)
{
    if constexpr (sizeof(wchar_t) == 4)
        return std::u32string((char32_t const*)_str.data(), _str.size());
    else
        return from_utf16(std::u16string_view((char16_t const*)_str.data(), _str.size()));
}

}
