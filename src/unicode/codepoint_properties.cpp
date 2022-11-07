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
#include <unicode/codepoint_properties.h>
#include <unicode/codepoint_properties_data.h>

namespace unicode
{

using table_view = codepoint_properties::tables_view;

codepoint_properties::tables_view codepoint_properties::configured_tables {
    gsl::span<table_view::stage1_element_type const>(precompiled::stage1.data(), precompiled::stage1.size()),
    gsl::span<table_view::stage2_element_type const>(precompiled::stage2.data(), precompiled::stage2.size()),
    gsl::span<codepoint_properties const>(precompiled::properties.data(), precompiled::properties.size())
};

codepoint_properties::names_view codepoint_properties::configured_names {
    gsl::span<table_view::stage1_element_type const>(precompiled::names_stage1.data(),
                                                     precompiled::names_stage1.size()),
    gsl::span<table_view::stage2_element_type const>(precompiled::names_stage2.data(),
                                                     precompiled::names_stage2.size()),
    gsl::span<std::string_view const>(precompiled::names_stage3.data(), precompiled::names_stage3.size())
};

} // namespace unicode
