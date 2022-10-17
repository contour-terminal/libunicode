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

#include <vector>

namespace unicode
{

/**
 * Loads Unicode properties as two-stage lookup tables from an extracted UCD.zip location.
 */
struct codepoint_properties_table
{
    operator codepoint_properties::tables_view() const noexcept
    {
        return codepoint_properties::tables_view { stage1, stage2, properties };
    }

    [[nodiscard]] codepoint_properties const& operator[](char32_t codepoint) const noexcept
    {
        return codepoint_properties::tables_view(*this).get(codepoint);
    }

    std::vector<codepoint_properties::tables_view::stage1_element_type> stage1 {}; // codepoint -> block
    std::vector<codepoint_properties::tables_view::stage2_element_type> stage2 {}; // block items -> property
    std::vector<codepoint_properties> properties {};

    static codepoint_properties_table load_from_directory(std::string const& ucdDataDirectory,
                                                          std::ostream* log = nullptr);
};

} // namespace unicode
