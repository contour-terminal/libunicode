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
#include <unicode/utf16.h>
#include <catch2/catch.hpp>

#include <string>
#include <string_view>

#include <fmt/format.h>

using namespace std;
using namespace std::string_view_literals;
using namespace unicode;

TEST_CASE("utf16.from_utf16", "[utf8]")
{
    auto const a16 = u"Hello, World!"sv;
    auto const a32 = from_utf16(a16);
    CHECK(a32 == U"Hello, World!");

    auto const b16 = u"😖:-)"sv;
    auto const b32 = from_utf16(b16);
    CHECK(b32 == U"😖:-)");
}

TEST_CASE("utf16.to_utf16", "[utf16]")
{
    // TODO
}
