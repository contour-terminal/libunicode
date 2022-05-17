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

#include <string_view>
#include <tuple>

namespace unicode
{

struct scan_result
{
    // Number of columns scanned.
    // One column equals a single narrow-width codepoint.
    // Codepoints with property East Asian Width Wide are treated as two columns.
    size_t count;

    // pointer to one byte after the last scanned codepoint.
    char const* next;
};

size_t scan_for_text_ascii(std::string_view text, size_t maxColumnCount) noexcept;
scan_result scan_for_text_nonascii(std::string_view text, size_t maxColumnCount) noexcept;
scan_result scan_for_text(std::string_view text, size_t maxColumnCount) noexcept;

} // namespace unicode
