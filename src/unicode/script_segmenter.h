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

#include <unicode/support.h>
#include <unicode/ucd.h>

#include <optional>
#include <string_view>
#include <vector>

namespace unicode
{

class script_segmenter
{
  public:
    constexpr script_segmenter() noexcept = default;
    constexpr script_segmenter& operator=(script_segmenter const&) noexcept = default;
    constexpr script_segmenter& operator=(script_segmenter&&) noexcept = default;
    constexpr script_segmenter(script_segmenter const&) noexcept = default;
    constexpr script_segmenter(script_segmenter&&) noexcept = default;

    constexpr explicit script_segmenter(char32_t const* data) noexcept:
        script_segmenter { data, getStringLength(data) }
    {
    }

    constexpr script_segmenter(char32_t const* data, size_t size) noexcept:
        data_ { data }, offset_ { 0 }, size_ { size }
    {
        currentScriptSet_.push_back(Script::Common);
    }

    constexpr script_segmenter(std::u32string_view data) noexcept:
        data_ { data.data() }, offset_ { 0 }, size_ { data.size() }
    {
        currentScriptSet_.push_back(Script::Common);
    }

    struct result
    {
        Script script;
        size_t size;
    };

    std::optional<result> consume();

    using property_type = Script;

    bool consume(out<size_t> size, out<Script> script)
    {
        if (auto const p = consume(); p.has_value())
        {
            *size = p.value().size;
            *script = p.value().script;
            return true;
        }
        return false;
    }

  private:
    using ScriptSet = fs_array<Script, 32>;

    /// constexpr-version of strlen for UTF-32 strings
    constexpr size_t getStringLength(char32_t const* data) noexcept
    {
        size_t n = 0;
        while (data && *data)
        {
            ++data;
            ++n;
        }
        return n;
    }

    /// Returnes all scripts that this @p _codepoint is associated with.
    ScriptSet getScriptsFor(char32_t codepoint);

    /// Intersects @p _nextSet into @p _currentSet.
    ///
    /// @retval true Intersection succeed, meaning that no boundary was found.
    /// @retval false The resulting intersection is empty, meaning, a script boundary was found.
    bool mergeSets(ScriptSet const& nextSet, ScriptSet& currentSet);

    /// Returns the resolved script.
    ///
    /// That is, if currentScriptSet is {Common}, then the preferred script for Common, otherwise
    /// whatever currentScriptSet's one and only element contains.
    constexpr Script resolveScript() const noexcept
    {
        Script const result = currentScriptSet_.at(0);
        return result == Script::Common ? commonPreferredScript_ : result;
    }

    constexpr char32_t currentChar() const noexcept { return data_[offset_]; }

    // private data

    char32_t const* data_ = U"";
    size_t offset_ = 0;
    size_t size_ = 0;

    ScriptSet currentScriptSet_ {};
    Script commonPreferredScript_ = Script::Common;
};

} // namespace unicode
