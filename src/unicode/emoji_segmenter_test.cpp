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
#include <unicode/emoji_segmenter.h>
#include <unicode/grapheme_segmenter.h>
#include <unicode/run_segmenter.h>
#include <unicode/utf8.h>
#include <catch2/catch.hpp>
#include <fmt/format.h>

using namespace unicode;
using namespace std::string_literals;
using namespace std;

namespace {
    struct Expectation
    {
        u32string_view text;
        size_t start;
        size_t end;
        bool emoji; //or that? RunPresentationStyle style;
    };

    void test_segments(int _lineNo, std::vector<std::pair<std::u32string_view, bool>> const& _expects)
    {
        vector<Expectation> expects;
        u32string fullText;
        size_t i = 0;
        for (auto && [text, isEmoji] : _expects)
        {
            expects.push_back(Expectation{
                text,
                i,
                i + text.size(),
                isEmoji
            });
            fullText += text;
            i += text.size();
        }

        INFO(fmt::format("Testing emoji segmentation from line {}: {}", _lineNo, to_utf8(fullText)));

        size_t size{};
        bool isEmoji{};
        auto segmenter = unicode::emoji_segmenter{fullText};
        for (size_t i = 0; i < _expects.size(); ++i)
        {
            INFO(fmt::format("run segmentation for part {}: \"{}\" to be {}",
                             i, to_utf8(_expects[i].first), _expects[i].second ? "an emoji" : "a text"));
            bool const consumeSuccess = segmenter.consume(out(size), out(isEmoji));
            REQUIRE(consumeSuccess);
            CHECK(_expects[i].first == *segmenter);
            CHECK(size == expects[i].end);
            CHECK(isEmoji == expects[i].emoji);
        }
        bool const consumeFail = segmenter.consume(out(size), out(isEmoji));
        REQUIRE_FALSE(consumeFail);
    }
}

TEST_CASE("emoji_segmenter.Emoji", "[emoji_segmenter]")
{
    test_segments(__LINE__, {
        {U"\U0001F600", true}
    });
}

TEST_CASE("emoji_segmenter.LatinEmoji", "[emoji_segmenter]")
{
    test_segments(__LINE__, {
        {U"AB", false},
        {U"😀", true}
    });
}

TEST_CASE("emoji_segmenter.EmojiLatin", "[emoji_segmenter]")
{
    test_segments(__LINE__, {
        {U"😀", true},
        {U"A", false},
    });
}

TEST_CASE("emoji_segmenter.TwoEmojis", "[emoji_segmenter]")
{
    test_segments(__LINE__, {
        {U"😀😀", true},
    });
}

TEST_CASE("emoji_segmenter.LatinCommonEmoji", "[emoji_segmenter]")
{
    test_segments(__LINE__, {
        {U"AB ", false},
        {U"😀", true},
    });
}

TEST_CASE("emoji_segmenter.EmojiTextPresentation", "[emoji_segmenter]")
{
    test_segments(__LINE__, {
        {U"\u270c\ufe0e", false},
    });
}

TEST_CASE("emoji_segmenter.emoji.text.emoji", "[emoji_segmenter]")
{
    test_segments(__LINE__, {
        {U"\u270c", true},
        {U"\u270c\ufe0e", false},
        {U"\u270c", true},
    });
}

TEST_CASE("emoji_segmenter.mixed_complex", "[emoji_segmenter]")
{
    test_segments(__LINE__, {
        {U"Hello(", false},                                      // Latin text
        {U"\u270c\U0001F926\U0001F3FC\u200D\u2642\uFE0F", true}, // 🤦🏼‍♂️ Face Palm
        {U"\u270c\ufe0e :-)", false},                            // ✌ Waving hand (text presentation)
        {U"\u270c", true},                                       // ✌ Waving hand
        {U")合!", false},
    });
}
