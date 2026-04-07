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
#include <libunicode/codepoint_properties.h>
#include <libunicode/ucd.h>

#include <catch2/catch_test_macros.hpp>

using namespace unicode;

// Verify that Age enum values are ordered by version number.
static_assert(Age::Unassigned < Age::V1_1);
static_assert(Age::V1_1 < Age::V2_0);
static_assert(Age::V2_0 < Age::V2_1);
static_assert(Age::V2_1 < Age::V3_0);
static_assert(Age::V3_0 < Age::V3_1);
static_assert(Age::V3_1 < Age::V3_2);
static_assert(Age::V3_2 < Age::V4_0);
static_assert(Age::V4_0 < Age::V4_1);
static_assert(Age::V4_1 < Age::V5_0);
static_assert(Age::V5_0 < Age::V5_1);
static_assert(Age::V5_1 < Age::V5_2);
static_assert(Age::V5_2 < Age::V6_0);
static_assert(Age::V6_0 < Age::V6_1);
static_assert(Age::V6_1 < Age::V6_2);
static_assert(Age::V6_2 < Age::V6_3);
static_assert(Age::V6_3 < Age::V7_0);
static_assert(Age::V7_0 < Age::V8_0);
static_assert(Age::V8_0 < Age::V9_0);
static_assert(Age::V9_0 < Age::V10_0);
static_assert(Age::V10_0 < Age::V11_0);
static_assert(Age::V11_0 < Age::V12_0);
static_assert(Age::V12_0 < Age::V12_1);
static_assert(Age::V12_1 < Age::V13_0);
static_assert(Age::V13_0 < Age::V14_0);
static_assert(Age::V14_0 < Age::V15_0);
static_assert(Age::V15_0 < Age::V15_1);
static_assert(Age::V15_1 < Age::V16_0);
static_assert(Age::V16_0 < Age::V17_0);

TEST_CASE("age.make_age", "[age]")
{
    // Happy-path lookups
    CHECK(make_age("1.1") == Age::V1_1);
    CHECK(make_age("2.0") == Age::V2_0);
    CHECK(make_age("2.1") == Age::V2_1);
    CHECK(make_age("3.0") == Age::V3_0);
    CHECK(make_age("9.0") == Age::V9_0);
    CHECK(make_age("10.0") == Age::V10_0);
    CHECK(make_age("15.1") == Age::V15_1);
    CHECK(make_age("16.0") == Age::V16_0);
    CHECK(make_age("17.0") == Age::V17_0);

    // Invalid inputs return nullopt
    CHECK_FALSE(make_age("").has_value());
    CHECK_FALSE(make_age("99.9").has_value());
    CHECK_FALSE(make_age("NA").has_value());
    CHECK_FALSE(make_age("invalid").has_value());
}

TEST_CASE("age.codepoint_properties", "[age]")
{
    // U+0041 'A' has been assigned since Unicode 1.1
    CHECK(codepoint_properties::get(U'A').age == Age::V1_1);

    // U+20AC EURO SIGN was added in Unicode 2.1
    CHECK(codepoint_properties::get(U'\u20AC').age == Age::V2_1);
}
