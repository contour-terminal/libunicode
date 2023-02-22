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
#include <libunicode/codepoint_properties_data.h>

namespace unicode
{

codepoint_properties::tables_view codepoint_properties::configured_tables { precompiled::stage1.data(),
                                                                            precompiled::stage2.data(),
                                                                            precompiled::properties.data() };

codepoint_properties::names_view codepoint_properties::configured_names {
    precompiled::names_stage1.data(),
    precompiled::names_stage2.data(),
    precompiled::names_stage3.data(),
};

} // namespace unicode
