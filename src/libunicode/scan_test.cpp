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
#include <libunicode/width.h>

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <format>
#include <string>
#include <string_view>
#include <vector>

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

TEST_CASE("scan.ascii.64")
{
    auto const text = "0123456789ABCDEF0123456789ABCDEF"
                      "0123456789ABCDEF0123456789ABCDEF"sv;
    CHECK(scan_for_text_ascii(text, 64) == 64);
    CHECK(scan_for_text_ascii(text, 32) == 32);
    CHECK(scan_for_text_ascii(text, 16) == 16);
    CHECK(scan_for_text_ascii(text, 8) == 8);
    CHECK(scan_for_text_ascii(text, 1) == 1);
}

TEST_CASE("scan.ascii.128")
{
    auto const text = "0123456789ABCDEF0123456789ABCDEF"
                      "0123456789ABCDEF0123456789ABCDEF"
                      "0123456789ABCDEF0123456789ABCDEF"
                      "0123456789ABCDEF0123456789ABCDEF"sv;
    CHECK(scan_for_text_ascii(text, 128) == 128);
    CHECK(scan_for_text_ascii(text, 64) == 64);
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
    auto text = "0123456789ABCDEF0\033123456789ABCDEF"
                "0123456789ABCDEF0123456789ABCDEF"
                "0123456789ABCDEF0123456789ABCDEF"
                "0123456789ABCDEF0123456789ABCDEF"sv;
    CHECK(scan_for_text_ascii(text, 80) == 17);
    text = "0123456789ABCDEF0123456789ABCDEF"
           "0123456789ABCDEF0123456789ABCDEF"
           "0123456789ABCDEF0123456789ABCDEF"
           "0123456789ABCDEF\0330123456789ABCDEF"sv;
    CHECK(scan_for_text_ascii(text, 128) == 112);
}

TEST_CASE("scan.ascii.until_complex")
{
    CHECK(scan_for_text_ascii("1234\x80", 80) == 4);
    CHECK(scan_for_text_ascii("0123456789{\xE2\x94\x80}ABCDEF", 80) == 11);
    constexpr auto text = "0123456789{\xE2\x94\x80}ABCDEF0323456789ABCDEF"
                          "0123456789ABCDEF0123456789ABCDEF"
                          "0123456789ABCDEF0123456789ABCDEF"
                          "0123456789ABCDEF0123456789ABCDEF"sv;
    CHECK(scan_for_text_ascii(text, 80) == 11);
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

TEST_CASE("scan.width_agrees_with_grapheme_cluster_width")
{
    // scan_text() and grapheme_cluster_width() are the library's two ways of asking how wide a piece
    // of text is, and they must never answer differently: a terminal scans with the former to advance
    // the cursor and measures with the latter when laying out, so any disagreement draws text at a
    // different width than was reserved for it and corrupts the rest of the line.
    auto const cases = std::array {
        U"का"sv,                   // Devanagari ka + AA matra (spacing mark)
        U"क्न"sv,                   // ka + virama + na (conjunct)
        U"กำ"sv,                   // Thai ko kai + sara am
        U"ຠຳ"sv,                   // Lao pho + sign AM
        U"ក្រ"sv,                   // Khmer conjunct (Invisible_Stacker)
        U"©️"sv,                    // copyright + VS16 (a defined variation base)
        U"✓️"sv,                    // check mark + VS16 (NOT a variation base)
        U"⌚︎"sv,                   // watch + VS15
        U"漢︎"sv,                   // CJK ideograph + VS15 (must not narrow)
        U"❤\U0001F3FB"sv,          // heart + skin tone modifier
        U"❤‍\U0001F525"sv,     // heart ZWJ fire
        U"\U0001F1E9\U0001F1EA"sv, // regional indicator pair
        U"中é"sv,                  // wide then narrow: the narrow one must not inherit width 2
        U"中éé"sv,                 // ...and the error must not compound
        U"\U0001F600é"sv,          // emoji then narrow
        U"中α"sv,                  // CJK then Greek alpha
        U"❤‍A"sv,              // heart ZWJ 'A': GB11 does not join, so this is two clusters
    };

    for (auto const cluster: cases)
    {
        auto const utf8 = u8(cluster);
        auto state = unicode::scan_state {};
        auto const scanned = unicode::scan_text(state, utf8, 80);
        INFO("cluster: " << escape(utf8));
        // The oracle is the std::string_view overload, which segments. The u32string_view one does
        // not -- it is documented as taking a *single* cluster -- and several cases above are two,
        // so measuring them with it would let a ZWJ swallow the codepoint across a cluster boundary
        // and report on the oracle rather than on scan_text().
        CHECK(scanned.count == unicode::grapheme_cluster_width(utf8));
    }
}

TEST_CASE("scan.cluster_split_across_calls_is_measured_once")
{
    // A grapheme cluster that straddles two scan_text() calls must total exactly what the whole
    // string totals: chunked input is how a terminal reads a PTY, so any drift here mis-advances the
    // cursor on ordinary text.
    //
    // Regression: the width accumulator was a local of scan_for_text_nonascii() while the grapheme
    // state it tracks lives in scan_state, so the second call started from an empty accumulator and
    // the selector it carried widened nothing.
    auto const text = u8(U"©️"sv); // copyright (2 bytes) + VS16 (3 bytes)
    REQUIRE(text.size() == 5);

    auto state = unicode::scan_state {};
    auto const first = unicode::scan_text(state, string_view(text.data(), 2), 80);
    auto const rest = string_view(state.next, static_cast<size_t>(std::distance(state.next, text.data() + text.size())));
    auto const second = unicode::scan_text(state, rest, 80);

    CHECK(first.count + second.count == unicode::grapheme_cluster_width(text));
    CHECK(first.count + second.count == 2);
    CHECK(state.next == text.data() + text.size());
}

TEST_CASE("scan.growing_cluster_does_not_overrun_the_scan_end")
{
    // scan_text() promises result.end <= state.next. The overflow rewind used to reset the input
    // pointer to a cluster start that lagged one cluster behind while result.end had already
    // advanced, so the caller was handed an end past what was scanned and re-rendered those bytes.
    //
    // The rewind was reachable only through VS16 before this path learned to honour spacing marks
    // and conjuncts, which is why ordinary Devanagari now reaches it.
    auto const text = u8(U"中काab"sv); // CJK + ka + AA matra + "ab"
    auto state = unicode::scan_state {};
    auto const result = unicode::scan_text(state, text, 3);

    CHECK(result.start <= result.end);
    CHECK(result.end <= state.next);

    // 中 is two columns and का is two more, so only 中 fits in three.
    CHECK(result.count == 2);
    CHECK(result.end == text.data() + 3);
}

TEST_CASE("scan.ascii_base_carries_into_a_following_spacing_mark")
{
    // The ASCII fast path is a separate scanner, but a cluster does not have to respect that
    // boundary: an ASCII base followed by a non-ASCII spacing mark is one cluster two columns wide.
    // Regression: the fast path recorded no last codepoint, so the mark looked like the start of a
    // fresh cluster and the pair measured 1.
    struct Case
    {
        std::u32string_view text;
        size_t expected;
        std::string_view what;
    };
    auto const cases = std::array {
        Case { U"aः"sv, 2, "a + Devanagari visarga" },
        Case { U"aா"sv, 2, "a + Tamil AA matra" },
        Case { U"abः"sv, 3, "trailing ASCII char is the base" },
        Case { U"abc"sv, 3, "pure ASCII still counts every column" },
        Case { U"ab中"sv, 4, "ASCII then a wide char that opens its own cluster" },
    };

    for (auto const& testCase: cases)
    {
        auto const utf8 = u8(testCase.text);
        auto state = unicode::scan_state {};
        auto const result = unicode::scan_text(state, utf8, 80);
        INFO(testCase.what);
        CHECK(result.count == testCase.expected);
        CHECK(result.count == unicode::grapheme_cluster_width(utf8));
    }
}

namespace
{
struct cluster_collector final: public unicode::grapheme_cluster_receiver
{
    std::vector<std::string> clusters;
    std::vector<size_t> widths;

    void receiveAsciiSequence(std::string_view text) noexcept override
    {
        for (auto const ch: text)
        {
            clusters.emplace_back(1, ch);
            widths.push_back(1);
        }
    }
    void receiveGraphemeCluster(std::string_view text, size_t columnCount) noexcept override
    {
        clusters.emplace_back(text);
        widths.push_back(columnCount);
    }
    void receiveInvalidGraphemeCluster() noexcept override
    {
        clusters.emplace_back("<invalid>");
        widths.push_back(1);
    }
};
} // namespace

TEST_CASE("scan.receiver_gets_whole_clusters")
{
    // Regression: the byte count handed to receiveGraphemeCluster() was reset only *after* the call,
    // so each cluster was delivered with the byte length of the next cluster's first codepoint --
    // truncated UTF-8 whenever the two differ in length, and the final cluster was dropped entirely.
    // Clusters of equal byte length hid it, which is all the older tests used.
    auto const text = u8(U"中é"sv); // CJK (3 bytes) + e-acute (2 bytes)
    auto collector = cluster_collector {};
    auto state = unicode::scan_state {};
    auto const result = unicode::scan_text(state, text, 80, collector);

    CHECK(result.count == 3);
    REQUIRE(collector.clusters.size() == 2);
    CHECK(collector.clusters[0] == u8(U"中"sv));
    CHECK(collector.clusters[1] == u8(U"é"sv));
    CHECK(collector.widths[0] == 2);
    CHECK(collector.widths[1] == 1);
}

TEST_CASE("scan.c1_control.stops_mid_run")
{
    // A raw C1 control byte (0x80..0x9F) sitting at a character boundary is a control, not text, so
    // scan_text stops at it and hands it back to the caller -- even when text follows. Regression:
    // with trailing bytes present the C1 byte used to be consumed as U+FFFD (a terminal parser would
    // then lose a mid-run 8-bit SPA/EPA, i.e. 0x96/0x97).
    auto state = unicode::scan_state {};
    auto const text = "ab\x96"
                      "cd"sv; // 'a', 'b', C1 SPA (0x96), then more text
    auto const result = unicode::scan_text(state, text, 80);
    CHECK(result.count == 2);
    CHECK(result.end == text.data() + 2);
    CHECK(state.next == text.data() + 2);
    CHECK(static_cast<uint8_t>(*state.next) == 0x96);
}

TEST_CASE("scan.c1_control.continuation_byte_stays_text")
{
    // A byte in 0x80..0x9F that is a UTF-8 *continuation* byte must remain part of its codepoint, not
    // be mistaken for a C1 control: U+2018 encodes as 0xE2 0x80 0x98, both trailing bytes falling in
    // the C1 range. The scan consumes the whole codepoint and stops at the following C0 control.
    auto state = unicode::scan_state {};
    auto const text = u8(U"\u2018"sv) + "\n"s; // LEFT SINGLE QUOTATION MARK, then LF
    auto const result = unicode::scan_text(state, text, 80);
    CHECK(result.count == 1);
    CHECK(state.next == text.data() + 3);
    CHECK(*state.next == '\n');
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
