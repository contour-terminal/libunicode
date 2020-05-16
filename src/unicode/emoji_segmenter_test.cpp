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
#include <catch2/catch.hpp>

using namespace unicode;
using namespace std::string_literals;
using namespace std;

TEST_CASE("emoji_segmenter.emoji.simple1", "[emoji_segmenter]")
{
    auto const codepoints = u32string_view{U"\U0001f600"};
    auto es = emoji_segmenter{ codepoints };

    CHECK(*es == U"\U0001f600");
    CHECK(es.isEmoji());

    ++es;
    CHECK(*es == U"");
}

TEST_CASE("emoji_segmenter.emoji.simple2", "[emoji_segmenter]")
{
    auto const codepoints = u32string_view{U"\U0001f600\U0001f600"};
    auto es = emoji_segmenter{ codepoints };

    CHECK(es.isEmoji());
    CHECK(*es == U"\U0001f600");

    ++es;
    CHECK(*es == U"\U0001f600");

    ++es;
    CHECK(*es == U"");
}

TEST_CASE("emoji_segmenter.text.simple1", "[emoji_segmenter]")
{
    auto const codepoints = u32string_view{U"\u270c\ufe0e"};
    auto es = emoji_segmenter{ codepoints };

    CHECK(es.isText());
    CHECK(*es == U"\u270c\ufe0e");

    ++es;
    CHECK(*es == U"");
}

TEST_CASE("emoji_segmenter.emoji.text.emoji", "[emoji_segmenter]")
{
    auto const codepoints = u32string_view{U"\u270c\u270c\ufe0e\u270c"};
    auto es = emoji_segmenter{ codepoints };

    CHECK(es.isEmoji());
    CHECK(*es == U"\u270c");

    ++es;
    CHECK(es.isText());
    CHECK(*es == U"\u270c\ufe0e");

    ++es;
    CHECK(es.isEmoji());
    CHECK(*es == U"\u270c");

    ++es;
    CHECK(*es == U"");
}

TEST_CASE("emoji_segmenter.mixed_complex", "[emoji_segmenter]")
{
    auto const codepoints = u32string_view{
        U"\u270c"                                       // ✌ Waving hand
        U"\U0001F926\U0001F3FC\u200D\u2642\uFE0F"       // 🤦🏼‍♂️ Face Palm
        U"\u270c\ufe0e"                                 // ✌ Waving hand (text presentation)
        U"\u270c"                                       // ✌ Waving hand
    };
    auto es = emoji_segmenter{ codepoints };

    CHECK(es.isEmoji());
    CHECK(*es == U"\u270c");

    ++es;
    CHECK(es.isEmoji());
    CHECK(*es == U"\U0001F926\U0001F3FC\u200D\u2642\uFE0F");

    ++es;
    CHECK(es.isText());
    CHECK(*es == U"\u270c\ufe0e");

    ++es;
    CHECK(es.isEmoji());
    CHECK(*es == U"\u270c");

    ++es;
    CHECK(*es == U"");
}
