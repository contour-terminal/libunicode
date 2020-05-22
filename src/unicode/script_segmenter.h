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

#include <unicode/ucd.h>

#include <optional>
#include <string_view>
#include <vector>

namespace unicode {

namespace helper {
    template <typename T, std::size_t N>
    class FixedCapacityArray {
      public:
        using value_type = T;
        using array_type = std::array<value_type, N>;
        using iterator = typename array_type::iterator;
        using const_iterator = typename array_type::const_iterator;

        constexpr FixedCapacityArray() noexcept : values_{{}}, size_{0} {}

        constexpr void clear() noexcept {
            for (size_t i = 0; i < size_; ++i)
                values_[i].~T();
            size_ = 0;
        };

        constexpr size_t size() const noexcept { return size_; }
        constexpr bool empty() const noexcept { return size_ == 0; }

        constexpr bool push_back(T _value) noexcept
        {
            if (size_ == N)
                return false;
            values_[size_++] = std::move(_value);
            return true;
        }

        constexpr void resize(size_t n) noexcept
        {
            if (n < N)
                size_ = n;
        }

        constexpr T const& operator[](size_t i) const noexcept { return values_[i]; }
        constexpr T const& at(size_t i) const noexcept { return values_.at(i); }

        constexpr iterator begin() noexcept { return values_.begin(); }
        constexpr iterator end() noexcept { return values_.end(); }

        constexpr const_iterator begin() const noexcept { return values_.begin(); }
        constexpr const_iterator end() const noexcept { return values_.end(); }

      private:
        array_type values_;
        size_t size_;
    };
}

class script_segmenter {
  public:
    constexpr script_segmenter(char32_t const* _data, size_t _size) noexcept :
        data_{ _data },
        offset_{ 0 },
        size_{ _size }
    {
        currentScriptSet_.push_back(Script::Common);
    }

    struct result {
        Script script;
        size_t size;
    };

    std::optional<result> consume();

  private:
    using ScriptSet = helper::FixedCapacityArray<Script, 32>;

    /// Returnes all scripts that this @p _codepoint
    ScriptSet getScriptsFor(char32_t _codepoint);

    /// Intersects @p _nextSet into @p _currentSet.
    ///
    /// @retval true Intersection succeed, meaning that no boundary was found.
    /// @retval false The resulting intersection is empty, meaning, a script boundary was found.
    bool mergeSets(ScriptSet const& _nextSet, ScriptSet& _currentSet);

    /// Returns the resolved script.
    ///
    /// That is, if currentScriptSet is {Common}, then the preferred script for Common, otherwise
    /// whatever currentScriptSet's one and only element contains.
    constexpr Script resolveScript() const noexcept
    {
        Script const result = currentScriptSet_.at(0);
        return result == Script::Common ? commonPreferredScript_ : result;
    }

    constexpr char32_t currentChar() const noexcept
    {
        return data_[offset_];
    }

    constexpr bool advanceChar() noexcept
    {
        if (offset_ == size_)
            return false;

        offset_++;
        return true;
    }

    constexpr char32_t advanceAndGetChar() noexcept
    {
        return data_[++offset_];
    }

    constexpr std::optional<char32_t> peekAndAdvanceChar() noexcept
    {
        if (offset_ == size_)
            return std::nullopt;

        auto const currentChar = data_[offset_];
        ++offset_;

        return {currentChar};
    }

  private:
    char32_t const* data_;
    size_t offset_;
    size_t size_;

    ScriptSet currentScriptSet_{};
    Script commonPreferredScript_ = Script::Unknown;
};

} // end namespace
