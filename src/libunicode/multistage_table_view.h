/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2022 Christian Parpart <christian@parpart.family>
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

#include <cstdint>
#include <limits>

namespace support
{

template <typename T,
          typename SourceType,
          typename Stage1ElementType,
          typename Stage2ElementType,
          SourceType BlockSize,
          SourceType MaxValue = std::numeric_limits<SourceType>::max()>
struct multistage_table_view
{
    using source_type = SourceType;
    using stage1_element_type = Stage1ElementType;
    using stage2_element_type = Stage2ElementType;
    using value_type = T;

    stage1_element_type const* stage1; // div
    stage2_element_type const* stage2; // mod
    value_type const* stage3;          // values

    static std::size_t constexpr block_size = BlockSize;

    // size_t size() const noexcept { return stage1.size(); }

    value_type const& get(source_type index, source_type fallback = source_type {}) const noexcept
    {
        return unsafe_get(index <= MaxValue ? index : fallback);
    }

    value_type const& unsafe_get(source_type index) const noexcept
    {
        auto const block_number = stage1[index / BlockSize];
        auto const block_start = block_number * BlockSize;
        auto const element_offset = index % BlockSize;
        auto const property_index = stage2[block_start + element_offset];
        return stage3[property_index];
    }
};

} // namespace support
