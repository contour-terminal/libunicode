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
#include <libunicode/width.h>

namespace unicode
{

int width(char32_t codepoint) noexcept
{
    return codepoint_properties::get(codepoint).char_width;
}

} // namespace unicode
