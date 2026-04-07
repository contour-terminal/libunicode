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

#include <libunicode/multistage_table_view.h>
#include <libunicode/scoped_timer.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iterator>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
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
    using view_type = multistage_table_view<T, SourceType, Stage1ElementType, Stage2ElementType, BlockSize, MaxValue>;

    std::vector<Stage1ElementType> stage1; // div
    std::vector<Stage2ElementType> stage2; // mod
    std::vector<T> stage3;                 // values

    auto to_view() const noexcept { return view_type { stage1.data(), stage2.data(), stage3.data() }; }

    T const& get(SourceType index) const noexcept { return to_view().get(index); }
};

template <typename T,
          typename SourceType,
          typename Stage1ElementType,
          typename Stage2ElementType,
          typename ValueHasher,
          SourceType BlockSize,
          SourceType MaxValue = std::numeric_limits<SourceType>::max()>
class multistage_table_generator
{
  public:
    T const* _input;
    size_t _inputSize;
    multistage_table<T, SourceType, Stage1ElementType, Stage2ElementType, BlockSize, MaxValue>& _output;
    ValueHasher _valueHasher;

    multistage_table_generator(T const* input,
                               size_t inputSize,
                               multistage_table<T, SourceType, Stage1ElementType, Stage2ElementType, BlockSize, MaxValue>& output,
                               ValueHasher hasher):
        _input(input), _inputSize(inputSize), _output(output), _valueHasher(std::move(hasher))
    {
    }

    void generate()
    {
        assert(_inputSize % BlockSize == 0);
        _output.stage1.resize(_inputSize / BlockSize);
        for (SourceType blockStart = 0; blockStart <= _inputSize - BlockSize; blockStart += BlockSize)
            _output.stage1[blockStart / BlockSize] = get_or_create_index_to_stage2_block(blockStart);
    }

    void verify() const
    {
        for (SourceType blockStart = 0; blockStart <= _inputSize - BlockSize; ++blockStart)
            verify_block(blockStart / BlockSize);
    }

  private:
    void verify_block(SourceType blockNumber) const
    {
        for (SourceType codepoint = blockNumber * BlockSize; codepoint < (blockNumber + 1) * BlockSize; ++codepoint)
        {
            auto const& a = _input[codepoint];
            auto const& b = _output.get(codepoint);
            if (a != b)
            {
                throw runtime_error((std::ostringstream()
                                     << "U+" << std::hex << unsigned(codepoint) << " mismatch in properties.\n"
                                     << "Expected : " << a << "\nActual   : " << b)
                                        .str());
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

    /// FNV-1a hash over a block of elements.
    size_t hashBlock(size_t blockStart) const noexcept
    {
        size_t hash = 14695981039346656037ULL;
        auto const* bytes = reinterpret_cast<char const*>(&_input[blockStart]);
        auto const byteCount = BlockSize * sizeof(T);
        for (size_t i = 0; i < byteCount; ++i)
        {
            hash ^= static_cast<size_t>(static_cast<unsigned char>(bytes[i]));
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    std::optional<size_t> find_same_block(size_t blockStart)
    {
        assert(blockStart % BlockSize == 0);
        assert(blockStart + BlockSize <= _inputSize);

        auto const h = hashBlock(blockStart);
        if (auto it = _blockHashMap.find(h); it != _blockHashMap.end())
        {
            for (auto otherStart: it->second)
                if (is_same_block(otherStart, blockStart))
                    return { otherStart / BlockSize };
        }
        _blockHashMap[h].push_back(blockStart);
        return std::nullopt;
    }

    bool is_same_block(size_t a, size_t b) const noexcept
    {
        return std::memcmp(&_input[a], &_input[b], BlockSize * sizeof(T)) == 0;
    }

    Stage2ElementType get_or_create_stage3_index(SourceType stage1Index)
    {
        auto const& value = _input[stage1Index];
        if (auto it = _stage3Map.find(value); it != _stage3Map.end())
            return it->second;

        auto const stage3Index = static_cast<Stage2ElementType>(_output.stage3.size());
        _output.stage3.emplace_back(value);
        assert(stage3Index < std::numeric_limits<Stage2ElementType>::max());
        _stage3Map[value] = stage3Index;
        return stage3Index;
    }

    std::unordered_map<size_t, std::vector<size_t>> _blockHashMap;
    std::unordered_map<T, Stage2ElementType, ValueHasher> _stage3Map;
};

template <typename T,
          typename SourceType,
          typename Stage1ElementType,
          typename Stage2ElementType,
          typename ValueHasher,
          SourceType BlockSize,
          SourceType MaxValue = std::numeric_limits<SourceType>::max()>
void generate(T const* input,
              size_t inputSize,
              multistage_table<T, SourceType, Stage1ElementType, Stage2ElementType, BlockSize, MaxValue>& output,
              ValueHasher&& hasher)
{
    auto builder = multistage_table_generator<T,
                                              SourceType,
                                              Stage1ElementType,
                                              Stage2ElementType,
                                              std::remove_reference_t<ValueHasher>,
                                              BlockSize,
                                              MaxValue>(input, inputSize, output, std::forward<ValueHasher>(hasher));
    builder.generate();
}

} // namespace support
