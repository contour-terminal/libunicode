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
#include <unicode/text_segmenter.h>
#include <string>
#include <string_view>
#include <catch2/catch.hpp>

using namespace std;
using namespace std::string_view_literals;

template <typename T> using out = unicode::out<T>;

TEST_CASE("text_segmenter.simple", "[text_segmenter]")
{
    auto seg = unicode::text_segmenter{U"Hello"sv};
    unicode::segment res;
    bool b1 = seg.consume(out<unicode::segment>(res));
    REQUIRE(b1);
}
