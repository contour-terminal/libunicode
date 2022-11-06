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
#pragma once

#include <unicode/codepoint_properties.h>
#include <unicode/support/multistage_table_generator.h>

#include <vector>

namespace unicode
{

using codepoint_properties_table = support::multistage_table<codepoint_properties,
                                                             uint32_t,     // source type
                                                             uint8_t,      // stage 1
                                                             uint16_t,     // stage 2
                                                             256,          // block size
                                                             0x110'000 - 1 // max value
                                                             >;

codepoint_properties_table load_from_directory(std::string const& ucdDataDirectory,
                                               std::ostream* log = nullptr);

} // namespace unicode
