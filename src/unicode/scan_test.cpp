/**
 * This file is part of the "libterminal" project
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
#include <unicode/convert.h>
#include <unicode/scan.h>

#include <fmt/format.h>

#include <catch2/catch.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{

auto constexpr FamilyEmoji = U"\U0001F468\u200D\U0001F469\u200D\U0001F467\u200D\U0001F466"sv;
auto constexpr SmileyEmoji = U"\U0001F600"sv;
auto constexpr CopyrightSign = U"\u00A9"sv;
auto constexpr ControlCodes = "\r\n"sv; // Used to ensure that C0 codes properly cancel scanning.

template <typename T>
auto u8(T text)
{
    return unicode::convert_to<char>(text);
}

std::string escape(uint8_t ch)
{
    switch (ch)
    {
        case '\\': return "\\\\";
        case 0x1B: return "\\e";
        case '\t': return "\\t";
        case '\r': return "\\r";
        case '\n': return "\\n";
        case '"': return "\\\"";
        default:
            if (ch < 0x20)
                return fmt::format("\\{:03o}", static_cast<uint8_t>(ch) & 0xFF);
            else if (ch < 0x80)
                return fmt::format("{}", static_cast<char>(ch));
            else
                return fmt::format("\\x{:02x}", static_cast<uint8_t>(ch) & 0xFF);
    }
}

template <typename T>
std::string escape(T begin, T end)
{
    static_assert(sizeof(*std::declval<T>()) == 1,
                  "should be only 1 byte, such as: char, char8_t, uint8_t, byte, ...");
    auto result = std::string {};
    while (begin != end)
        result += escape(static_cast<uint8_t>(*begin++));
    return result;
}

inline std::string escape(std::string_view s)
{
    return escape(begin(s), end(s));
}

} // namespace

TEST_CASE("scan.ascii.empty")
{
    CHECK(unicode::scan_for_text_ascii("", 0) == 0);
    CHECK(unicode::scan_for_text_ascii("", 1) == 0);
}

TEST_CASE("scan.ascii.32")
{
    auto const text = "0123456789ABCDEF0123456789ABCDEF"sv;
    CHECK(unicode::scan_for_text_ascii(text, 32) == 32);
    CHECK(unicode::scan_for_text_ascii(text, 16) == 16);
    CHECK(unicode::scan_for_text_ascii(text, 8) == 8);
    CHECK(unicode::scan_for_text_ascii(text, 1) == 1);
}

TEST_CASE("scan.ascii.mixed_with_controls")
{
    CHECK(unicode::scan_for_text_ascii("\0331234", 80) == 0);
    CHECK(unicode::scan_for_text_ascii("1234\033", 80) == 4);
    CHECK(unicode::scan_for_text_ascii("12345678\033", 80) == 8);
    CHECK(unicode::scan_for_text_ascii("0123456789ABCDEF\033", 80) == 16);
    CHECK(unicode::scan_for_text_ascii("0123456789ABCDEF1\033", 80) == 17);
}

TEST_CASE("scan.ascii.until_complex")
{
    CHECK(unicode::scan_for_text_ascii("1234\x80", 80) == 4);
    CHECK(unicode::scan_for_text_ascii("0123456789{\xE2\x94\x80}ABCDEF", 80) == 11);
}

TEST_CASE("scan.complex.grapheme_cluster.1")
{
    auto const familyEmoji8 = u8(FamilyEmoji);
    auto const result = unicode::scan_for_text_nonascii(familyEmoji8, 80);
    CHECK(result.count == 2);
    CHECK(result.next == familyEmoji8.data() + familyEmoji8.size());
}

TEST_CASE("scan.complex.grapheme_cluster.2")
{
    auto const familyEmoji8 = u8(FamilyEmoji) + u8(FamilyEmoji);
    auto const result = unicode::scan_for_text_nonascii(familyEmoji8, 80);
    CHECK(result.count == 4);
    CHECK(result.next == familyEmoji8.data() + familyEmoji8.size());
}

TEST_CASE("scan.complex.mixed")
{
    auto const text = u8(FamilyEmoji) + "ABC"s + u8(FamilyEmoji);
    auto const result = unicode::scan_for_text_nonascii(text, 80);
    CHECK(result.count == 2);
    CHECK(result.next == text.data() + u8(FamilyEmoji).size());
}

TEST_CASE("scan.complex.half-overflowing")
{
    auto const oneEmoji = u8(SmileyEmoji);
    auto const text = oneEmoji + oneEmoji + oneEmoji;

    // match at boundary
    auto const result2 = unicode::scan_for_text_nonascii(text, 2);
    CHECK(result2.count == 2);
    CHECK(result2.next == text.data() + oneEmoji.size());

    // one grapheme cluster is half overflowing
    auto const result3 = unicode::scan_for_text_nonascii(text, 3);
    CHECK(result3.count == 2);
    CHECK(result3.next == text.data() + oneEmoji.size());

    // match buondary
    auto const result4 = unicode::scan_for_text_nonascii(text, 4);
    CHECK(result4.count == 4);
    CHECK(result4.next == text.data() + 2 * oneEmoji.size());
}

TEST_CASE("scan.any.tiny")
{
    // Ensure that we're really only scanning up to the input's size (1 byte, here).
    auto const storage = "X{0123456789ABCDEF}"sv;
    auto const input = storage.substr(0, 1);
    auto const result = unicode::scan_for_text(input, 80);
    CHECK(result.count == 1);
    CHECK(result.next == input.data() + input.size());
    CHECK(*result.next == '{');
}

TEST_CASE("scan.any.ascii_complex_repeat")
{
    auto const oneComplex = u8(SmileyEmoji);        // 2
    auto const oneSimple = "0123456789ABCDEF0123"s; // 20

    for (size_t i = 1; i <= 6; ++i)
    {
        auto s = ""s;
        for (size_t k = 1; k <= i; ++k)
            s += (k % 2) != 0 ? oneSimple : oneComplex;
        s += ControlCodes;

        auto const result = unicode::scan_for_text(s, 80);
        auto const countSimple = ((i + 1) / 2) * 20;
        auto const countComplex = (i / 2) * 2;

        INFO(fmt::format("i = {}, ascii# {}, complex# {}, count {}, actual {}, s = \"{}\"",
                         i,
                         countSimple,
                         countComplex,
                         result.count,
                         countSimple + countComplex,
                         escape(s)));

        CHECK(result.count == countSimple + countComplex);
        CHECK(result.next == s.data() + s.size() - ControlCodes.size());
    }
}

TEST_CASE("scan.any.complex_ascii_repeat")
{
    auto const oneComplex = u8(SmileyEmoji);        // 2
    auto const oneSimple = "0123456789ABCDEF0123"s; // 20

    for (size_t i = 1; i <= 6; ++i)
    {
        auto s = ""s;
        for (size_t k = 1; k <= i; ++k)
            s += (k % 2) != 0 ? oneComplex : oneSimple;
        s += ControlCodes;

        auto const result = unicode::scan_for_text(s, 80);
        CHECK(result.count == (i / 2) * 20 + ((i + 1) / 2) * 2);
        CHECK(result.next == s.data() + s.size() - ControlCodes.size());
    }
}

TEST_CASE("scan.complex.VS16")
{
    auto const oneComplex = u8(CopyrightSign);
    auto const modifierVS16 = u8(U"\uFE0F"sv);

    // // narrow copyright sign
    auto const result1 = unicode::scan_for_text(oneComplex, 80);
    CHECK(result1.count == 1);
    CHECK(result1.next == oneComplex.data() + oneComplex.size());

    // copyright sign in emoji presentation
    auto const s = oneComplex + modifierVS16;
    auto const result = unicode::scan_for_text(s, 80);
    CHECK(result.count == 2);
    CHECK(result.next == s.data() + s.size());

    auto const result3 = unicode::scan_for_text(s, 1);
    CHECK(result3.count == 0);
    CHECK(result3.next == s.data());
}
