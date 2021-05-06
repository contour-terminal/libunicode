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
#include <unicode/script_segmenter.h>
#include <unicode/ucd.h>
#include <algorithm>

using namespace std;

namespace unicode {

namespace
{
    bool constexpr isPreferred(Script _script) noexcept
    {
        switch (_script)
        {
            case Script::Invalid:
            case Script::Common:
            case Script::Inherited:
                return false;
            default:
                return true;
        }
    }
}

optional<script_segmenter::result> script_segmenter::consume()
{
    if (offset_ >= size_)
        return nullopt;

    while (offset_ < size_)
    {
        ScriptSet const nextScriptSet = getScriptsFor(currentChar());

        if (!mergeSets(nextScriptSet, currentScriptSet_))
        {
            // If merging failed, then we have found a script segmeent boundary.
            auto const res = result{ resolveScript(), offset_ };
            currentScriptSet_ = nextScriptSet;
            return res;
        }

        offset_++;
    }

    auto const res = result{ resolveScript(), offset_ };
    currentScriptSet_.clear();
    return res;
}

bool script_segmenter::mergeSets(ScriptSet const& _nextSet, ScriptSet& _currentSet)
{
    if (_nextSet.empty() || _currentSet.empty())
        return false;

    auto currentSetIter = _currentSet.begin();
    auto const currentSetEnd = _currentSet.end();

    Script priorityScript = *currentSetIter++;

    if (!isPreferred(_nextSet.at(0)))
    {
        if (_nextSet.size() == 2 && !isPreferred(priorityScript) && commonPreferredScript_ == Script::Common)
            commonPreferredScript_ = _nextSet.at(1);
        return true;
    }

    // If priorityScript is either Common or Inherited then take nextScriptSet
    if (!isPreferred(priorityScript))
    {
        _currentSet = _nextSet;
        return true;
    }

    auto nextSetIter = _nextSet.begin();
    auto const nextSetEnd = _nextSet.end();

    if (currentSetIter == currentSetEnd)
        return std::find(nextSetIter, nextSetEnd, priorityScript) != nextSetEnd;

    // See if we have a priority script, and if not, get it from the nextScriptSet
    bool hasPriorityScript = find(nextSetIter, nextSetEnd, priorityScript) != nextSetEnd;
    if (!hasPriorityScript)
    {
        priorityScript = *nextSetIter++;
        hasPriorityScript = find(currentSetIter, currentSetEnd, priorityScript) != currentSetEnd;
    }

    auto currentWriteIter = _currentSet.begin();
    if (hasPriorityScript)
        *currentWriteIter++ = priorityScript;

    // Intersect the remaining nextScriptSet into the currentSetIter.
    if (nextSetIter != nextSetEnd)
    {
        while (currentSetIter != currentSetEnd)
        {
            auto const sc = *currentSetIter++;
            if (find(nextSetIter, nextSetEnd, sc) != nextSetEnd)
                *currentWriteIter++ = sc;
        }
    }

    // NB: first is always smaller than second, so it is save to cast to unsigned.
    auto const writeCount = static_cast<size_t>(distance(_currentSet.begin(), currentWriteIter));
    if (writeCount == 0)
        return false;

    _currentSet.resize(writeCount);
    return true;
}

script_segmenter::ScriptSet script_segmenter::getScriptsFor(char32_t _codepoint)
{
    ScriptSet scriptSet;

    // Collect all script(/-extensions) for @p _codepoint into scriptSet.
    size_t const sceCount = script_extensions(_codepoint, scriptSet.data(), scriptSet.capacity());
    scriptSet.resize(sceCount);

    // Get the script for @p _codepoint.
    Script const sc = script(_codepoint);

    // If the script of @p _codepoint is also in scriptSet,
    // then move it to the front of the set,
    // otherwise append it to the back of scriptSet.
    if (auto i = find(scriptSet.begin(), scriptSet.end(), sc); i != scriptSet.end())
        swap(*i, *scriptSet.begin());
    else
        scriptSet.push_back(sc);

    return scriptSet;
}


} // end namespace
