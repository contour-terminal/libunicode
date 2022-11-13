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
#include <unicode/run_segmenter.h>
#include <unicode/ucd_ostream.h>
#include <unicode/utf8.h>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <catch2/catch.hpp>

#include <array>
#include <sstream>
#include <string>
#include <string_view>

using namespace std;
using namespace std::string_view_literals;
using namespace unicode;

namespace
{
struct Expectation
{
    u32string_view text;
    unicode::Script script;
    PresentationStyle presentationStyle;
};

void test_run_segmentation(int lineNo, std::vector<Expectation> const& expectations)
{
    vector<run_segmenter::range> expects;
    u32string text;
    size_t i = 0;
    for (Expectation const& expect: expectations)
    {
        expects.push_back(run_segmenter::range {
            i,
            i + expect.text.size(),
            { expect.script, expect.presentationStyle },
        });
        text += expect.text;
        i += expect.text.size();
    }

    auto segmenter = unicode::run_segmenter { text };
    unicode::run_segmenter::range actualSegment;
    for (size_t i = 0; i < expectations.size(); ++i)
    {
        auto s = std::ostringstream {};
        s << "Line " << lineNo << ": run segmentation failed for part " << i << ": \""
          << to_utf8(expectations[i].text) << "\" to be " << expects[i];
        INFO(s.str());
        bool const consumeSuccess = segmenter.consume(out(actualSegment));
        REQUIRE(consumeSuccess);
        CHECK(actualSegment == expects[i]);
    }
    bool const consumeFail = segmenter.consume(out(actualSegment));
    REQUIRE_FALSE(consumeFail);
}
} // namespace

TEST_CASE("run_segmenter.empty", "[run_segmenter]")
{
    auto rs = run_segmenter { U"" };
    auto result = run_segmenter::range {};
    auto const rv = rs.consume(out(result));
    CHECK_FALSE(rv);
    CHECK(result.start == 0);
    CHECK(result.end == 0);
    CHECK(get<Script>(result.properties) == Script::Invalid);
    CHECK(get<PresentationStyle>(result.properties) == PresentationStyle::Text);
}

TEST_CASE("run_segmenter.Emoji_VS15", "[emoji_segmenter]")
{
    test_run_segmentation(__LINE__,
                          {
                              { U"\U0001F600\uFE0E", Script::Common, PresentationStyle::Text },
                          });
}

TEST_CASE("run_segmenter.LatinEmoji", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          {
                              { U"A", Script::Latin, PresentationStyle::Text },
                              { U"😀", Script::Latin, PresentationStyle::Emoji },
                          });
}

TEST_CASE("run_segmenter.LatinCommonEmoji", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          {
                              { U"A ", Script::Latin, PresentationStyle::Text },
                              { U"😀", Script::Latin, PresentationStyle::Emoji },
                          });
}

TEST_CASE("run_segmenter.LatinEmojiLatin", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          {
                              { U"AB", Script::Latin, PresentationStyle::Text },
                              { U"😀", Script::Latin, PresentationStyle::Emoji },
                              { U"CD", Script::Latin, PresentationStyle::Text },
                          });
}

TEST_CASE("run_segmenter.LatinPunctuationSideways", "[run_segmenter]")
{
    test_run_segmentation(__LINE__, { { U"Abc.;?Xyz", Script::Latin, PresentationStyle::Text } });
}

TEST_CASE("run_segmenter.OneSpace", "[run_segmenter]")
{
    test_run_segmentation(__LINE__, { { U" ", Script::Common, PresentationStyle::Text } });
}

TEST_CASE("run_segmenter.ArabicHangul", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          { { U"نص", Script::Arabic, PresentationStyle::Text },
                            { U"키스의", Script::Hangul, PresentationStyle::Text } });
}

TEST_CASE("run_segmenter.JapaneseHindiEmojiMix", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          { { U"百家姓", Script::Han, PresentationStyle::Text },
                            { U"ऋषियों", Script::Devanagari, PresentationStyle::Text },
                            { U"🌱🌲🌳🌴", Script::Devanagari, PresentationStyle::Emoji },
                            { U"百家姓", Script::Han, PresentationStyle::Text },
                            { U"🌱🌲", Script::Han, PresentationStyle::Emoji } });
}

TEST_CASE("run_segmenter.CombiningCirlce", "[run_segmenter]")
{
    test_run_segmentation(__LINE__, { { U"◌́◌̀◌̈◌̂◌̄◌̊", Script::Common, PresentationStyle::Text } });
}

TEST_CASE("run_segmenter.Arab_Hangul", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          {
                              { U"نص", Script::Arabic, PresentationStyle::Text },
                              { U"키스의", Script::Hangul, PresentationStyle::Text },
                          });
}

// TODO: orientation
// TEST_CASE("run_segmenter.HangulSpace", "[run_segmenter]")
// {
//     test_run_segmentation(__LINE__, {
//         {U"키스의", Script::Hangul, PresentationStyle::Text},     // Orientation::Keep
//         {U" ", Script::Hangul, PresentationStyle::Text},          // Orientation::Sideways
//         {U"고유조건은", Script::Hangul, PresentationStyle::Text}  // Orientation::Keep
//     });
// }

TEST_CASE("run_segmenter.TechnicalCommonUpright", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          {
                              { U"⌀⌁⌂", Script::Common, PresentationStyle::Text },
                          });
}

TEST_CASE("run_segmenter.PunctuationCommonSideways", "[run_segmenter]")
{
    test_run_segmentation(__LINE__, { { U".…¡", Script::Common, PresentationStyle::Text } });
}

// TODO: orientation
// TEST_CASE("run_segmenter.JapanesePunctuationMixedInside", "[run_segmenter]")
// {
//     test_run_segmentation(__LINE__, {
//         {U"いろはに", Script::Hiragana, PresentationStyle::Text}, // Orientation::Keep
//         {U".…¡", Script::Hiragana, PresentationStyle::Text},      // Orientation::RotateSideways
//         {U"ほへと", Script::Hiragana, PresentationStyle::Text},   // Orientation::Keep
//     });
// }

TEST_CASE("run_segmenter.JapanesePunctuationMixedInsideHorizontal", "[run_segmenter]")
{
    test_run_segmentation(
        __LINE__,
        {
            { U"いろはに.…¡ほへと", Script::Hiragana, PresentationStyle::Text }, // Orientation::Keep
        });
}

TEST_CASE("run_segmenter.PunctuationDevanagariCombining", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          {
                              { U"क+े", Script::Devanagari, PresentationStyle::Text }, // Orientation::Keep
                          });
}

TEST_CASE("run_segmenter.EmojiZWJSequences", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          {
                              { U"👩‍👩‍👧‍👦👩‍❤️‍💋‍👨",
                                Script::Latin,
                                PresentationStyle::Emoji },
                              { U"abcd", Script::Latin, PresentationStyle::Text },
                              { U"👩‍👩", Script::Latin, PresentationStyle::Emoji },
                              { U"\U0000200D‍efg", Script::Latin, PresentationStyle::Text },
                          });
}

// TODO: Orientation
// TEST_CASE("run_segmenter.JapaneseLetterlikeEnd", "[run_segmenter]")
// {
//     test_run_segmentation(__LINE__, {
//         {U"いろは", Script::Hiragana, PresentationStyle::Text}, // Orientation::Keep
//         {U"ℐℒℐℒℐℒℐℒℐℒℐℒℐℒ", Script::Hiragana, PresentationStyle::Text}, // Orientation::RotateSideways
//     });
// }

// TODO: Orientation
// TEST_CASE("run_segmenter.JapaneseCase", "[run_segmenter]")
// {
//     test_run_segmentation(__LINE__, {
//         {U"いろは", Script::Hiragana, PresentationStyle::Text},   // Keep
//         {U"aaAA", Script::Latin, PresentationStyle::Text},        // RotateSideways
//         {U"いろは", Script::Hiragana, PresentationStyle::Text},   // Keep
//     });
// }

TEST_CASE("run_segmenter.DingbatsMiscSymbolsModifier", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          { { U"⛹🏻✍🏻✊🏼", Script::Common, PresentationStyle::Emoji } });
}

TEST_CASE("run_segmenter.ArmenianCyrillicCase", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          { { U"աբգ", Script::Armenian, PresentationStyle::Text },
                            { U"αβγ", Script::Greek, PresentationStyle::Text },
                            { U"ԱԲԳ", Script::Armenian, PresentationStyle::Text } });
}

TEST_CASE("run_segmenter.EmojiSubdivisionFlags", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          { { U"🏴󠁧󠁢󠁷󠁬󠁳󠁿🏴󠁧󠁢󠁳󠁣󠁴󠁿🏴󠁧󠁢"
                              U"󠁥󠁮󠁧󠁿",
                              Script::Common,
                              PresentationStyle::Emoji } });
}

TEST_CASE("run_segmenter.NonEmojiPresentationSymbols", "[run_segmenter]")
{
    test_run_segmentation(__LINE__,
                          { { U"\U00002626\U0000262a\U00002638\U0000271d\U00002721\U00002627"
                              U"\U00002628\U00002629\U0000262b\U0000262c\U00002670"
                              U"\U00002671\U0000271f\U00002720",
                              Script::Common,
                              PresentationStyle::Text } }); // Orientation::Keep
}
