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
#include <libunicode/word_segmenter.h>

#include <catch2/catch_test_macros.hpp>

using namespace unicode;
using namespace std::string_literals;
using namespace std;

TEST_CASE("word_segmenter.HelloWorld", "[word_segmenter]")
{
    auto constexpr s = U"Hello, \t World!"sv;

    auto ws = word_segmenter(s);
    CHECK(*ws == U"Hello,");
    CHECK(ws.size() == 6);

    ++ws;
    CHECK(*ws == U" \t ");
    CHECK(ws.size() == 3);

    ++ws;
    CHECK(*ws == U"World!");
    CHECK(ws.size() == 6);

    ++ws;
    CHECK(*ws == U"");
    CHECK(ws.size() == 0);
}
