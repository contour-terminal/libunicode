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
#include <libunicode/convert.h>
#include <libunicode/scan.h>
#include <libunicode/utf8.h>

#include <catch2/catch_test_macros.hpp>

#include <format>
#include <string_view>

using std::string_view;

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
                return std::format("\\{:03o}", static_cast<uint8_t>(ch) & 0xFF);
            else if (ch < 0x80)
                return std::format("{}", static_cast<char>(ch));
            else
                return std::format("\\x{:02x}", static_cast<uint8_t>(ch) & 0xFF);
    }
}

template <typename T>
std::string escape(T begin, T end)
{
    static_assert(sizeof(*std::declval<T>()) == 1, "should be only 1 byte, such as: char, char8_t, uint8_t, byte, ...");
    auto result = std::string {};
    while (begin != end)
        result += escape(static_cast<uint8_t>(*begin++));
    return result;
}

inline std::string escape(std::string_view s)
{
    return escape(begin(s), end(s));
}

class grapheme_cluster_collector final: public unicode::grapheme_cluster_receiver
{
  public:
    std::vector<std::u32string> output;

    void receiveAsciiSequence(std::string_view sequence) noexcept override
    {
        for (char const ch: sequence)
            output.emplace_back(1, static_cast<char32_t>(ch));
    }

    void receiveGraphemeCluster(std::string_view cluster, size_t) noexcept override
    {
        output.emplace_back(unicode::convert_to<char32_t>(cluster));
    }

    void receiveInvalidGraphemeCluster() noexcept override
    {
        auto constexpr ReplacementCharacter = U'\uFFFD';
        output.emplace_back(1, ReplacementCharacter);
    }
};

} // namespace

using unicode::detail::scan_for_text_ascii;

TEST_CASE("scan.ascii.empty")
{
    CHECK(scan_for_text_ascii("", 0) == 0);
    CHECK(scan_for_text_ascii("", 1) == 0);
}

TEST_CASE("scan.ascii.32")
{
    auto const text = "0123456789ABCDEF0123456789ABCDEF"sv;
    CHECK(scan_for_text_ascii(text, 32) == 32);
    CHECK(scan_for_text_ascii(text, 16) == 16);
    CHECK(scan_for_text_ascii(text, 8) == 8);
    CHECK(scan_for_text_ascii(text, 1) == 1);
}

TEST_CASE("scan.ascii.mixed_with_controls")
{
    CHECK(scan_for_text_ascii("\0331234", 80) == 0);
    CHECK(scan_for_text_ascii("1234\033", 80) == 4);
    CHECK(scan_for_text_ascii("12345678\033", 80) == 8);
    CHECK(scan_for_text_ascii("0123456789ABCDEF\033", 80) == 16);
    CHECK(scan_for_text_ascii("0123456789ABCDEF1\033", 80) == 17);
}

TEST_CASE("scan.ascii.until_complex")
{
    CHECK(scan_for_text_ascii("1234\x80", 80) == 4);
    CHECK(scan_for_text_ascii("0123456789{\xE2\x94\x80}ABCDEF", 80) == 11);
}

TEST_CASE("scan.complex.grapheme_cluster.1")
{
    auto state = unicode::scan_state {};
    auto const familyEmoji8 = u8(FamilyEmoji);
    auto const result = unicode::detail::scan_for_text_nonascii(state, familyEmoji8, 80, unicode::null_receiver::get());
    CHECK(result.count == 2);
    CHECK(state.next == familyEmoji8.data() + familyEmoji8.size());
}

TEST_CASE("scan.complex.grapheme_cluster.2")
{
    auto state = unicode::scan_state {};
    auto const familyEmoji8 = u8(FamilyEmoji) + u8(FamilyEmoji);
    auto const result = unicode::detail::scan_for_text_nonascii(state, familyEmoji8, 80, unicode::null_receiver::get());
    CHECK(result.count == 4);
    CHECK(state.next == familyEmoji8.data() + familyEmoji8.size());
}

TEST_CASE("scan.complex.mixed")
{
    auto state = unicode::scan_state {};
    auto const text = u8(FamilyEmoji) + "ABC"s + u8(FamilyEmoji);
    auto const result = unicode::detail::scan_for_text_nonascii(state, text, 80, unicode::null_receiver::get());
    CHECK(result.count == 2);
    CHECK(state.next == text.data() + u8(FamilyEmoji).size());
}

TEST_CASE("scan.complex.half-overflowing")
{
    auto state = unicode::scan_state {};
    auto const oneEmoji = u8(SmileyEmoji);
    auto const text = oneEmoji + oneEmoji + oneEmoji;

    // match at boundary
    auto const result2 = unicode::detail::scan_for_text_nonascii(state, text, 2, unicode::null_receiver::get());
    CHECK(result2.count == 2);
    CHECK(state.next == text.data() + oneEmoji.size());

    // one grapheme cluster is half overflowing
    auto const result3 = unicode::detail::scan_for_text_nonascii(state, text, 3, unicode::null_receiver::get());
    CHECK(result3.count == 2);
    CHECK(state.next == text.data() + oneEmoji.size());

    // match buondary
    auto const result4 = unicode::detail::scan_for_text_nonascii(state, text, 4, unicode::null_receiver::get());
    CHECK(result4.count == 4);
    CHECK(state.next == text.data() + 2 * oneEmoji.size());
}

TEST_CASE("scan.any.tiny")
{
    // Ensure that we're really only scanning up to the input's size (1 byte, here).
    auto state = unicode::scan_state {};
    auto const storage = "X{0123456789ABCDEF}"sv;
    auto const input = storage.substr(0, 1);
    auto const result = unicode::scan_text(state, input, 80);
    CHECK(result.count == 1);
    CHECK(state.next == input.data() + input.size());
    CHECK(*state.next == '{');
}

TEST_CASE("scan.complex.sliced_calls")
{
    auto state = unicode::scan_state {};
    auto const text = "\xF0\x9F\x98\x80\033\\0123456789ABCDEF"sv; // U+1F600
    auto constexpr splitOffset = 3;
    auto const chunkOne = std::string_view(text.data(), splitOffset);

    auto result = unicode::scan_text(state, chunkOne, 80);

    REQUIRE(state.utf8.expectedLength == 4);
    REQUIRE(state.utf8.currentLength == 3);
    CHECK(result.count == 0);
    CHECK(result.start == text.data());
    CHECK(result.end == text.data());
    CHECK(state.next == (text.data() + splitOffset));

    auto const chunkTwo = std::string_view(state.next, (size_t) std::distance(state.next, text.data() + text.size()));
    result = unicode::scan_text(state, chunkTwo, 80, unicode::null_receiver::get());

    REQUIRE(state.utf8.expectedLength == 0);
    CHECK(result.count == 2);
    REQUIRE(result.start == text.data());
    REQUIRE(result.end == text.data() + 4);
    REQUIRE(state.next == text.data() + 4);
    auto const resultingText = string_view(result.start, static_cast<size_t>(std::distance(result.start, result.end)));
    REQUIRE(resultingText == text.substr(0, 4));
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

        auto state = unicode::scan_state {};
        auto const result = scan_text(state, s, 80);
        auto const countSimple = ((i + 1) / 2) * 20;
        auto const countComplex = (i / 2) * 2;

        INFO(std::format("i = {}, ascii# {}, complex# {}, count {}, actual {}, s = \"{}\"",
                         i,
                         countSimple,
                         countComplex,
                         result.count,
                         countSimple + countComplex,
                         escape(s)));

        CHECK(result.count == countSimple + countComplex);
        CHECK(state.next == s.data() + s.size() - ControlCodes.size());
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

        auto state = unicode::scan_state {};
        auto const result = unicode::scan_text(state, s, 80);
        CHECK(result.count == (i / 2) * 20 + ((i + 1) / 2) * 2);
        CHECK(state.next == s.data() + s.size() - ControlCodes.size());
    }
}

TEST_CASE("scan.complex.VS16")
{
    auto const oneComplex = u8(CopyrightSign);
    auto const modifierVS16 = u8(U"\uFE0F"sv);

    // // narrow copyright sign
    auto state = unicode::scan_state {};
    auto const result1 = unicode::scan_text(state, oneComplex, 80);
    CHECK(result1.count == 1);
    CHECK(state.next == oneComplex.data() + oneComplex.size());

    // copyright sign in emoji presentation
    state = {};
    auto const s = oneComplex + modifierVS16;
    auto const result = unicode::scan_text(state, s, 80);
    CHECK(result.count == 2);
    CHECK(state.next == s.data() + s.size());

    state = {};
    auto const result3 = unicode::scan_text(state, s, 1);
    CHECK(result3.count == 0);
    CHECK(state.next == s.data());
}

#if 0
namespace
{

// NOLINTNEXTLINE(readability-identifier-naming)
struct ColumnCount
{
    size_t value;
};

constexpr ColumnCount operator""_columns(unsigned long long value) noexcept
{
    return ColumnCount { static_cast<size_t>(value) };
}

std::vector<uint8_t> operator""_bvec(char const* value, size_t n)
{
    std::vector<uint8_t> v;
    v.reserve(n);
    while (*value)
        v.push_back(static_cast<uint8_t>(*value++));
    return v;
}

template <typename T>
std::string hex(T const& text)
{
    std::string encodedText;
    for (auto const ch: text)
    {
        if (!encodedText.empty())
            encodedText += ' ';
        char buf[3];
        snprintf(buf, sizeof(buf), "%02X", static_cast<unsigned>(ch));
        encodedText.append(buf, 2);
    }
    return encodedText;
}

// Single scan from clean start to stopByte.
void testScanText(int lineNo,
                  ColumnCount expectedColumnCount,
                  std::vector<uint8_t> const& expectation,
                  uint8_t stopByte,
                  std::vector<std::u32string_view> const& analyzedGraphemeClusters)
{
    INFO(std::format("Testing scan segment from line {}: {} ({:02X})", lineNo, hex(expectation), stopByte));
    auto const maxColumnCount = 80;

    std::string fullText;
    fullText.insert(fullText.end(), expectation.begin(), expectation.end());
    fullText.push_back(static_cast<char>(stopByte));

    auto graphemeClusterCollector = grapheme_cluster_collector {};

    auto state = unicode::scan_state {};
    auto const result = unicode::scan_text(state, fullText, maxColumnCount, graphemeClusterCollector);
    auto const start = (char const*) fullText.data();

    CHECK(size_t(result.start - start) == 0);
    CHECK(size_t(result.end - start) == expectation.size());
    CHECK(result.count == expectedColumnCount.value);
    CHECK(result.next[0] == stopByte);
    CHECK(result.next == fullText.data() + expectation.size());

    CHECK(graphemeClusterCollector.output.size() == analyzedGraphemeClusters.size());
    auto const iMax = std::min(analyzedGraphemeClusters.size(), graphemeClusterCollector.output.size());
    for (size_t i = 0; i < iMax; ++i)
    {
        INFO(std::format("i: {}, lhs: {}, rhs: {}",
                         i,
                         u8(std::u32string_view(graphemeClusterCollector.output[i].data(),
                                                graphemeClusterCollector.output[i].size())),
                         u8(analyzedGraphemeClusters[i])));
        CHECK(graphemeClusterCollector.output[i] == analyzedGraphemeClusters[i]);
    }
}

} // namespace

TEST_CASE("scan.invalid")
{
    auto constexpr LF = '\n';
    auto const RC = U"\uFFFD"sv;

    // 0xB1 is an invalid UTF-8 byte
    // 0xF5 is valid beginning of a 4-byte UTF-8 sequence but incomplete if not finished and hence, invalid.

    // clang-format off
    testScanText(__LINE__, 0_columns, {}, LF, {});
    testScanText(__LINE__, 1_columns, { 'A' }, LF, { U"A" });
    testScanText(__LINE__, 2_columns, { 'A', 'B' }, LF, { U"A", U"B" });
    testScanText(__LINE__, 3_columns, { 'A', 0xB1, 'B' }, LF, { U"A", RC, U"B" });            // invalid UTF-8
    testScanText(__LINE__, 4_columns, { 'A', 0xB1, 0xB1, 'B' }, LF, { U"A", RC, RC, U"B" });  // invalid UTF-8
    testScanText(__LINE__, 3_columns, { 'A', 0xF5, 'B' }, LF, { U"A", RC, U"B" });            // incomplete UTF-8
    testScanText(__LINE__, 4_columns, { 'A', 0xB1, 0xF5, 'B' }, LF, { U"A", RC, RC, U"B" });  // mixed case of the 2 above
    testScanText(__LINE__, 6_columns, { 'A', 0xB1, 0xF5, 'H', 'e', 'y' }, LF, { U"A", RC, RC, U"H", U"e", U"y" });
    testScanText(__LINE__, 2_columns, "\xf0\x9f\x98\200"_bvec, LF, { U"\U0001F600" });        // U+1F600
    testScanText(__LINE__,
                 18_columns,
                 "\xf0\x9f\x98\2000123456789ABCDEF"_bvec,
                 LF,
                 { U"\U0001F600",
                   U"0", U"1", U"2", U"3", U"4", U"5", U"6", U"7", U"8", U"9",
                   U"A", U"B", U"C", U"D", U"E", U"F" });
    // clang-format on
}
#endif
