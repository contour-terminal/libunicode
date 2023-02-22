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
#include <libunicode/width.h>

#include <catch2/catch.hpp>

TEST_CASE("random test", "[width]")
{
    // C0
    CHECK(unicode::width(0x07) == 0);

    // US-ASCII
    for (char32_t i = 0x20; i <= 0x7E; ++i)
        CHECK(unicode::width(i) == 1);

    CHECK(unicode::width(U'\u00A9') == 1); // Copyright symbol

    CHECK(unicode::width(U'\u200D') == 0); // ZWJ
    CHECK(unicode::width(U'\uFE0E') == 0); // emoji modifier
    CHECK(unicode::width(U'\uFE0F') == 0); // emoji modifier

    // emoji
    CHECK(unicode::width(U'\U0001F60A') == 2); // 😊 :blush:
    CHECK(unicode::width(U'\U0001F480') == 2); // 💀 :skull:
}
