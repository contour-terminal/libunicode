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
#include <libunicode/word_segmenter.h>

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

using namespace unicode;
using namespace std::string_view_literals;

namespace
{
/// Collects all word segments from a u32string_view into a vector.
std::vector<std::u32string_view> collect_segments(std::u32string_view text)
{
    std::vector<std::u32string_view> result;
    auto seg = word_segmenter(text);
    while (true)
    {
        auto const word = *seg;
        if (word.empty() && !seg.codepointsAvailable())
            break;
        result.push_back(word);
        if (!seg.codepointsAvailable())
            break;
        ++seg;
    }
    return result;
}

/// Collects break positions (offsets from start) for a codepoint sequence.
/// Returns a sorted list of offsets where word boundaries occur.
/// Includes position 0 (sot) and position N (eot).
std::vector<size_t> collect_break_positions(std::u32string_view text)
{
    std::vector<size_t> breaks;
    breaks.push_back(0); // WB1: sot

    if (text.empty())
        return breaks;

    auto seg = word_segmenter(text);
    size_t offset = 0;
    while (true)
    {
        auto const word = *seg;
        offset += word.size();
        breaks.push_back(offset);
        if (!seg.codepointsAvailable())
            break;
        ++seg;
    }
    return breaks;
}

/// Parse a hex string like "000D" to a char32_t.
char32_t parse_hex(std::string_view sv)
{
    unsigned long val = 0;
    for (auto c: sv)
    {
        val <<= 4;
        if (c >= '0' && c <= '9')
            val |= static_cast<unsigned long>(c - '0');
        else if (c >= 'A' && c <= 'F')
            val |= static_cast<unsigned long>(c - 'A' + 10);
        else if (c >= 'a' && c <= 'f')
            val |= static_cast<unsigned long>(c - 'a' + 10);
    }
    return static_cast<char32_t>(val);
}

struct WordBreakTestCase
{
    std::u32string codepoints;
    std::vector<size_t> expected_breaks; // offsets where breaks occur
    std::string comment;
    int line_number = 0;
};

/// Parse WordBreakTest.txt into test cases.
/// Format: ÷ 000D × 000A ÷ (# comment)
/// ÷ = break, × = no break
std::vector<WordBreakTestCase> parse_word_break_test_file(std::string const& path)
{
    std::vector<WordBreakTestCase> cases;
    std::ifstream file(path);
    if (!file.is_open())
        return cases;

    std::string line;
    auto lineNo = 0;
    while (std::getline(file, line))
    {
        ++lineNo;
        if (line.empty() || line[0] == '#')
            continue;

        // The line starts with ÷ or × characters (UTF-8 encoded)
        // ÷ = U+00F7 (UTF-8: C3 B7), × = U+00D7 (UTF-8: C3 97)

        WordBreakTestCase tc;
        tc.line_number = lineNo;

        // Extract comment part
        auto commentPos = line.find('#');
        auto dataStr = (commentPos != std::string::npos) ? line.substr(0, commentPos) : line;
        if (commentPos != std::string::npos)
            tc.comment = line.substr(commentPos);

        // Parse the data portion: sequence of (÷|×) XXXX pairs
        size_t cpIndex = 0;
        auto i = size_t { 0 };
        while (i < dataStr.size())
        {
            // Skip whitespace
            while (i < dataStr.size() && (dataStr[i] == ' ' || dataStr[i] == '\t'))
                ++i;
            if (i >= dataStr.size())
                break;

            // Check for ÷ (C3 B7) or × (C3 97)
            if (i + 1 < dataStr.size() && static_cast<unsigned char>(dataStr[i]) == 0xC3)
            {
                auto const secondByte = static_cast<unsigned char>(dataStr[i + 1]);
                if (secondByte == 0xB7) // ÷ = break
                {
                    tc.expected_breaks.push_back(cpIndex);
                    i += 2;
                    continue;
                }
                if (secondByte == 0x97) // × = no break
                {
                    i += 2;
                    continue;
                }
            }

            // Must be a hex codepoint
            auto start = i;
            while (i < dataStr.size() && dataStr[i] != ' ' && dataStr[i] != '\t'
                   && static_cast<unsigned char>(dataStr[i]) != 0xC3)
                ++i;

            if (i > start)
            {
                auto hexStr = dataStr.substr(start, i - start);
                tc.codepoints.push_back(parse_hex(hexStr));
                ++cpIndex;
            }
        }

        if (!tc.codepoints.empty())
            cases.push_back(std::move(tc));
    }
    return cases;
}
} // namespace

// =============================================================================
// Official Unicode Conformance Tests
// =============================================================================

TEST_CASE("word_segmenter.unicode_conformance", "[word_segmenter]")
{
    auto const path = std::string(LIBUNICODE_UCD_DIR) + "/auxiliary/WordBreakTest.txt";
    auto const testCases = parse_word_break_test_file(path);

    if (testCases.empty())
    {
        WARN("Skipping conformance tests: WordBreakTest.txt not found at " << path);
        return;
    }
    INFO("Loaded " << testCases.size() << " test cases from WordBreakTest.txt");

    for (auto const& tc: testCases)
    {
        auto const actual = collect_break_positions(tc.codepoints);
        if (actual != tc.expected_breaks)
        {
            std::ostringstream msg;
            msg << "FAIL line " << tc.line_number << ": ";

            msg << "codepoints: ";
            for (auto cp: tc.codepoints)
                msg << std::hex << "U+" << static_cast<uint32_t>(cp) << " ";

            msg << " expected breaks: ";
            for (auto b: tc.expected_breaks)
                msg << std::dec << b << " ";

            msg << " actual breaks: ";
            for (auto b: actual)
                msg << std::dec << b << " ";

            msg << tc.comment;
            FAIL(msg.str());
        }
    }
}

// =============================================================================
// Manual Unit Tests
// =============================================================================

TEST_CASE("word_segmenter.empty", "[word_segmenter]")
{
    auto const segments = collect_segments(U""sv);
    CHECK(segments.empty());
}

TEST_CASE("word_segmenter.basic_latin", "[word_segmenter]")
{
    // Each word and non-word character sequence forms a separate segment.
    auto const segments = collect_segments(U"Hello World"sv);
    REQUIRE(segments.size() == 3);
    CHECK(segments[0] == U"Hello");
    CHECK(segments[1] == U" ");
    CHECK(segments[2] == U"World");
}

TEST_CASE("word_segmenter.punctuation_boundary", "[word_segmenter]")
{
    auto const segments = collect_segments(U"Hello, World!"sv);
    // "Hello" | "," | " " | "World" | "!"
    REQUIRE(segments.size() == 5);
    CHECK(segments[0] == U"Hello");
    CHECK(segments[1] == U",");
    CHECK(segments[2] == U" ");
    CHECK(segments[3] == U"World");
    CHECK(segments[4] == U"!");
}

TEST_CASE("word_segmenter.contraction", "[word_segmenter]")
{
    // WB6/WB7: AHLetter (MidLetter|MidNumLetQ) AHLetter stays together
    // Apostrophe (U+0027) has WB=Single_Quote, which is part of MidNumLetQ
    auto const segments = collect_segments(U"don't"sv);
    REQUIRE(segments.size() == 1);
    CHECK(segments[0] == U"don't");
}

TEST_CASE("word_segmenter.numeric_with_separator", "[word_segmenter]")
{
    // WB11/WB12: Numeric (MidNum|MidNumLetQ) Numeric
    // Period (U+002E) has WB=MidNumLet
    auto const segments = collect_segments(U"3.14"sv);
    REQUIRE(segments.size() == 1);
    CHECK(segments[0] == U"3.14");
}

TEST_CASE("word_segmenter.crlf", "[word_segmenter]")
{
    // WB3: CR x LF (no break between them)
    auto const segments = collect_segments(U"a\r\nb"sv);
    REQUIRE(segments.size() == 3);
    CHECK(segments[0] == U"a");
    CHECK(segments[1] == U"\r\n");
    CHECK(segments[2] == U"b");
}

TEST_CASE("word_segmenter.newline_break", "[word_segmenter]")
{
    // WB3a/WB3b: break around newlines
    auto const segments = collect_segments(U"a\nb"sv);
    REQUIRE(segments.size() == 3);
    CHECK(segments[0] == U"a");
    CHECK(segments[1] == U"\n");
    CHECK(segments[2] == U"b");
}

TEST_CASE("word_segmenter.numeric_sequence", "[word_segmenter]")
{
    // WB8: Numeric x Numeric
    auto const segments = collect_segments(U"12345"sv);
    REQUIRE(segments.size() == 1);
    CHECK(segments[0] == U"12345");
}

TEST_CASE("word_segmenter.letter_number_transition", "[word_segmenter]")
{
    // WB9: AHLetter x Numeric, WB10: Numeric x AHLetter
    auto const segments = collect_segments(U"abc123def"sv);
    REQUIRE(segments.size() == 1);
    CHECK(segments[0] == U"abc123def");
}

TEST_CASE("word_segmenter.katakana", "[word_segmenter]")
{
    // WB13: Katakana x Katakana
    // U+30AB = KATAKANA LETTER KA, U+30BF = KATAKANA LETTER TA, U+30CA = KATAKANA LETTER NA
    auto const segments = collect_segments(U"\u30AB\u30BF\u30CA"sv);
    REQUIRE(segments.size() == 1);
    CHECK(segments[0] == U"\u30AB\u30BF\u30CA");
}

TEST_CASE("word_segmenter.extend_numlet", "[word_segmenter]")
{
    // WB13a/WB13b: ExtendNumLet connects letters, numbers, katakana
    // U+005F = UNDERSCORE has WB=ExtendNumLet
    auto const segments = collect_segments(U"hello_world"sv);
    REQUIRE(segments.size() == 1);
    CHECK(segments[0] == U"hello_world");
}

TEST_CASE("word_segmenter.wsegspace", "[word_segmenter]")
{
    // WB3d: WSegSpace x WSegSpace (multiple spaces stay together)
    auto const segments = collect_segments(U"a   b"sv);
    REQUIRE(segments.size() == 3);
    CHECK(segments[0] == U"a");
    CHECK(segments[1] == U"   ");
    CHECK(segments[2] == U"b");
}

TEST_CASE("word_segmenter.regional_indicator_pair", "[word_segmenter]")
{
    // WB15/WB16: Regional Indicator pairs
    // U+1F1FA U+1F1F8 = US flag (two RI characters)
    auto const segments = collect_segments(U"\U0001F1FA\U0001F1F8"sv);
    REQUIRE(segments.size() == 1);
    CHECK(segments[0] == U"\U0001F1FA\U0001F1F8");
}

TEST_CASE("word_segmenter.regional_indicator_odd", "[word_segmenter]")
{
    // Three RI characters: first two pair, third is separate
    auto const segments = collect_segments(U"\U0001F1FA\U0001F1F8\U0001F1EC"sv);
    REQUIRE(segments.size() == 2);
    CHECK(segments[0] == U"\U0001F1FA\U0001F1F8");
    CHECK(segments[1] == U"\U0001F1EC");
}

TEST_CASE("word_segmenter.extend_transparency", "[word_segmenter]")
{
    // WB4: Extend characters are transparent
    // U+0308 = COMBINING DIAERESIS (Extend)
    // "a" + combining diaeresis + "b" should be: "a\u0308b" (one word, AHLetter x Extend x AHLetter)
    auto const segments = collect_segments(U"a\u0308b"sv);
    REQUIRE(segments.size() == 1);
    CHECK(segments[0] == U"a\u0308b");
}

TEST_CASE("word_segmenter.hebrew_single_quote", "[word_segmenter]")
{
    // WB7a: Hebrew_Letter x Single_Quote
    // U+05D0 = HEBREW LETTER ALEF (Hebrew_Letter)
    auto const segments = collect_segments(U"\u05D0'"sv);
    REQUIRE(segments.size() == 1);
    CHECK(segments[0] == U"\u05D0'");
}

TEST_CASE("word_segmenter.hebrew_double_quote", "[word_segmenter]")
{
    // WB7b/WB7c: Hebrew_Letter x Double_Quote x Hebrew_Letter
    // U+05D0 = HEBREW LETTER ALEF, U+0022 = QUOTATION MARK (Double_Quote)
    auto const segments = collect_segments(U"\u05D0\"\u05D1"sv);
    REQUIRE(segments.size() == 1);
    CHECK(segments[0] == U"\u05D0\"\u05D1");
}

TEST_CASE("word_segmenter.emoji_zwj", "[word_segmenter]")
{
    // WB3c: ZWJ x Extended_Pictographic
    // U+1F468 (man) + U+200D (ZWJ) + U+1F469 (woman) - ZWJ prevents break
    auto const segments = collect_segments(U"\U0001F468\u200D\U0001F469"sv);
    REQUIRE(segments.size() == 1);
    CHECK(segments[0] == U"\U0001F468\u200D\U0001F469");
}

TEST_CASE("word_segmenter.codepointsAvailable", "[word_segmenter]")
{
    auto seg = word_segmenter(U"abc"sv);
    CHECK_FALSE(seg.codepointsAvailable()); // "abc" is a single segment, no more after it
    CHECK(static_cast<bool>(seg) == false);
    CHECK(*seg == U"abc");
}

TEST_CASE("word_segmenter.single_char", "[word_segmenter]")
{
    auto const segments = collect_segments(U"a"sv);
    REQUIRE(segments.size() == 1);
    CHECK(segments[0] == U"a");
}
