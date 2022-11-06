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

#include <unicode/support/multistage_table_view.h>
#include <unicode/support/scoped_timer.h>

#include <fmt/format.h>

#include <gsl/span>
#include <gsl/span_ext>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

namespace support
{

template <typename T,
          typename SourceType,
          typename Stage1ElementType,
          typename Stage2ElementType,
          SourceType BlockSize,
          SourceType MaxValue = std::numeric_limits<SourceType>::max()>
struct multistage_table
{
    using view_type =
        multistage_table_view<T, SourceType, Stage1ElementType, Stage2ElementType, BlockSize, MaxValue>;

    std::vector<Stage1ElementType> stage1; // div
    std::vector<Stage2ElementType> stage2; // mod
    std::vector<T> stage3;                 // values

    auto to_view() const noexcept
    {
        return view_type { gsl::span<Stage1ElementType const>(stage1.data(), stage1.size()),
                           gsl::span<Stage2ElementType const>(stage2.data(), stage2.size()),
                           gsl::span<T const>(stage3.data(), stage3.size()) };
    }

    T const& get(SourceType index) const noexcept { return to_view().get(index); }
};

template <typename T,
          typename SourceType,
          typename Stage1ElementType,
          typename Stage2ElementType,
          SourceType BlockSize,
          SourceType MaxValue = std::numeric_limits<SourceType>::max()>
class multistage_table_generator
{
  public:
    gsl::span<T const> _input;
    multistage_table<T, SourceType, Stage1ElementType, Stage2ElementType, BlockSize, MaxValue>& _output;

    void generate()
    {
        assert(_input.size() % BlockSize == 0);
        _output.stage1.resize(_input.size());
        for (SourceType blockStart = 0; blockStart <= _input.size() - BlockSize; blockStart += BlockSize)
            _output.stage1[blockStart / BlockSize] = get_or_create_index_to_stage2_block(blockStart);
    }

    void verify() const
    {
        for (SourceType blockStart = 0; blockStart <= _input.size() - BlockSize; ++blockStart)
            verify_block(blockStart / BlockSize);
    }

  private:
    void verify_block(SourceType blockNumber) const
    {
        for (SourceType codepoint = blockNumber * BlockSize; codepoint < (blockNumber + 1) * BlockSize;
             ++codepoint)
        {
            auto const& a = _input[codepoint];
            auto const& b = _output.get(codepoint);
            if (a != b)
            {
                throw runtime_error(fmt::format("U+{:X} mismatch in properties. "
                                                "Expected : {}; "
                                                "Actual   : {}",
                                                (unsigned) codepoint,
                                                a,
                                                b));
            }
        }
    }

    Stage1ElementType get_or_create_index_to_stage2_block(SourceType blockStart)
    {
        if (auto other_block = find_same_block(static_cast<size_t>(blockStart)))
            return _output.stage1[other_block.value()];

        // Block has not been seen yet. Create a new block.
        auto const stage2Index = _output.stage2.size() / BlockSize;
        assert(stage2Index < std::numeric_limits<Stage2ElementType>::max());

        for (SourceType index = blockStart; index < blockStart + BlockSize; ++index)
            _output.stage2.emplace_back(get_or_create_stage3_index(index));

        assert(_output.stage2.size() % BlockSize == 0);

        return static_cast<Stage1ElementType>(stage2Index);
    }

    std::optional<size_t> find_same_block(size_t blockStart) const noexcept
    {
        assert(blockStart % BlockSize == 0);
        assert(blockStart + BlockSize <= _input.size());

        for (size_t otherBlockStart = 0; otherBlockStart < blockStart; otherBlockStart += BlockSize)
            if (is_same_block(otherBlockStart, blockStart))
                return { otherBlockStart / BlockSize };

        return std::nullopt;
    }

    /// Tests if two given blocks are equivalent.
    /// @p a and @p b are both absolute offsets to the start of each block.
    bool is_same_block(size_t a, size_t b) const noexcept
    {
        assert(a % BlockSize == 0);
        assert(b % BlockSize == 0);
        assert(a + BlockSize <= _input.size());
        assert(b + BlockSize <= _input.size());

        for (size_t i = 0; i < BlockSize; ++i)
            if (_input[a + i] != _input[b + i])
                return false;

        return true;
    }

    Stage2ElementType get_or_create_stage3_index(SourceType stage1Index)
    {
        auto& properties = _output.stage3;
        auto const propertyIterator = find(properties.begin(), properties.end(), _input[stage1Index]);
        if (propertyIterator != properties.end())
            return static_cast<Stage2ElementType>(distance(properties.begin(), propertyIterator));

        auto const stage3Index = properties.size();
        properties.emplace_back(_input[stage1Index]);
        assert(stage3Index < std::numeric_limits<Stage2ElementType>::max());
        return static_cast<Stage2ElementType>(stage3Index);
    }
};

template <typename T,
          typename SourceType,
          typename Stage1ElementType,
          typename Stage2ElementType,
          SourceType BlockSize,
          SourceType MaxValue = std::numeric_limits<SourceType>::max()>
void generate(
    gsl::span<T const> input,
    multistage_table<T, SourceType, Stage1ElementType, Stage2ElementType, BlockSize, MaxValue>& output)
{
    auto builder =
        multistage_table_generator<T, SourceType, Stage1ElementType, Stage2ElementType, BlockSize, MaxValue> {
            input, output
        };
    builder.generate();
}

} // namespace support
