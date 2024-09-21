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
#include <libunicode/emoji_segmenter.h>
#include <libunicode/grapheme_segmenter.h>
#include <libunicode/run_segmenter.h>
#include <libunicode/utf8.h>

#include <catch2/catch_test_macros.hpp>

#include <format>

using namespace unicode;
using namespace std::string_literals;
using namespace std;

namespace
{
struct Expectation
{
    u32string_view text;
    size_t start;
    size_t end;
    PresentationStyle presentationStyle;
};

void test_segments(int lineNo, std::vector<std::pair<std::u32string_view, PresentationStyle>> const& expectations)
{
    vector<Expectation> expects;
    u32string fullText;
    size_t i = 0;
    for (auto&& [text, isEmoji]: expectations)
    {
        expects.push_back(Expectation { text, i, i + text.size(), isEmoji });
        fullText += text;
        i += text.size();
    }

    INFO(std::format("Testing emoji segmentation from line {}: {}", lineNo, to_utf8(fullText)));

    size_t size {};
    auto presentationStyle = PresentationStyle {};
    auto segmenter = unicode::emoji_segmenter { fullText };
    for (size_t i = 0; i < expectations.size(); ++i)
    {
        INFO(std::format("run segmentation for part {}: \"{}\" to be {}",
                         i,
                         to_utf8(expectations[i].first),
                         (unsigned) expectations[i].second));
        bool const consumeSuccess = segmenter.consume(out(size), out(presentationStyle));
        REQUIRE(consumeSuccess);
        CHECK(expectations[i].first == *segmenter);
        CHECK(size == expects[i].end);
        CHECK(presentationStyle == expects[i].presentationStyle);
    }
    bool const consumeFail = segmenter.consume(out(size), out(presentationStyle));
    REQUIRE_FALSE(consumeFail);
}
} // namespace

TEST_CASE("emoji_segmenter.Emoji", "[emoji_segmenter]")
{
    test_segments(__LINE__, { { U"\U0001F600", PresentationStyle::Emoji } });
}

TEST_CASE("emoji_segmenter.Emoji_VS15", "[emoji_segmenter]")
{
    test_segments(__LINE__, { { U"\U0001F600\uFE0E", PresentationStyle::Text } });
}

TEST_CASE("emoji_segmenter.LatinEmoji", "[emoji_segmenter]")
{
    test_segments(__LINE__, { { U"AB", PresentationStyle::Text }, { U"😀", PresentationStyle::Emoji } });
}

TEST_CASE("emoji_segmenter.EmojiLatin", "[emoji_segmenter]")
{
    test_segments(__LINE__,
                  {
                      { U"😀", PresentationStyle::Emoji },
                      { U"A", PresentationStyle::Text },
                  });
}

TEST_CASE("emoji_segmenter.TwoEmojis", "[emoji_segmenter]")
{
    test_segments(__LINE__,
                  {
                      { U"😀😀", PresentationStyle::Emoji },
                  });
}

TEST_CASE("emoji_segmenter.LatinCommonEmoji", "[emoji_segmenter]")
{
    test_segments(__LINE__,
                  {
                      { U"AB ", PresentationStyle::Text },
                      { U"😀", PresentationStyle::Emoji },
                  });
}

TEST_CASE("emoji_segmenter.EmojiTextPresentation", "[emoji_segmenter]")
{
    test_segments(__LINE__,
                  {
                      { U"\u270c\ufe0e", PresentationStyle::Text },
                  });
}

TEST_CASE("emoji_segmenter.emoji.text.emoji", "[emoji_segmenter]")
{
    test_segments(__LINE__,
                  {
                      { U"\u270c", PresentationStyle::Emoji },
                      { U"\u270c\ufe0e", PresentationStyle::Text },
                      { U"\u270c", PresentationStyle::Emoji },
                  });
}

TEST_CASE("emoji_segmenter.mixed_complex", "[emoji_segmenter]")
{
    test_segments(
        __LINE__,
        {
            { U"Hello(", PresentationStyle::Text },                                        // Latin text
            { U"\u270c\U0001F926\U0001F3FC\u200D\u2642\uFE0F", PresentationStyle::Emoji }, // 🤦🏼‍♂️ Face Palm
            { U"\u270c\ufe0e :-)", PresentationStyle::Text },                              // ✌ Waving hand (text presentation)
            { U"\u270c", PresentationStyle::Emoji },                                       // ✌ Waving hand
            { U")合!", PresentationStyle::Text },                                          // Kanji text
        });
}
