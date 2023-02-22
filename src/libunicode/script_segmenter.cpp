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
#include <libunicode/script_segmenter.h>
#include <libunicode/ucd.h>

#include <algorithm>

using namespace std;

namespace unicode
{

namespace
{
    bool constexpr isPreferred(Script script) noexcept
    {
        switch (script)
        {
            case Script::Invalid:
            case Script::Common:
            case Script::Inherited: return false;
            default: return true;
        }
    }
} // namespace

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
            auto const res = result { resolveScript(), offset_ };
            currentScriptSet_ = nextScriptSet;
            return res;
        }

        offset_++;
    }

    auto const res = result { resolveScript(), offset_ };
    currentScriptSet_.clear();
    return res;
}

bool script_segmenter::mergeSets(ScriptSet const& nextSet, ScriptSet& currentSet)
{
    if (nextSet.empty() || currentSet.empty())
        return false;

    auto currentSetIter = currentSet.begin();
    auto const currentSetEnd = currentSet.end();

    Script priorityScript = *currentSetIter++;

    if (!isPreferred(nextSet.at(0)))
    {
        if (nextSet.size() == 2 && !isPreferred(priorityScript) && commonPreferredScript_ == Script::Common)
            commonPreferredScript_ = nextSet.at(1);
        return true;
    }

    // If priorityScript is either Common or Inherited then take nextScriptSet
    if (!isPreferred(priorityScript))
    {
        currentSet = nextSet;
        return true;
    }

    auto nextSetIter = nextSet.begin();
    auto const nextSetEnd = nextSet.end();

    if (currentSetIter == currentSetEnd)
        return std::find(nextSetIter, nextSetEnd, priorityScript) != nextSetEnd;

    // See if we have a priority script, and if not, get it from the nextScriptSet
    bool hasPriorityScript = find(nextSetIter, nextSetEnd, priorityScript) != nextSetEnd;
    if (!hasPriorityScript)
    {
        priorityScript = *nextSetIter++;
        hasPriorityScript = find(currentSetIter, currentSetEnd, priorityScript) != currentSetEnd;
    }

    auto currentWriteIter = currentSet.begin();
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
    auto const writeCount = static_cast<size_t>(distance(currentSet.begin(), currentWriteIter));
    if (writeCount == 0)
        return false;

    currentSet.resize(writeCount);
    return true;
}

script_segmenter::ScriptSet script_segmenter::getScriptsFor(char32_t codepoint)
{
    ScriptSet scriptSet;

    // Collect all script(/-extensions) for @p _codepoint into scriptSet.
    size_t const sceCount = script_extensions(codepoint, scriptSet.data(), scriptSet.capacity());
    scriptSet.resize(sceCount);

    // Get the script for @p _codepoint.
    Script const sc = script(codepoint);

    // If the script of @p _codepoint is also in scriptSet,
    // then move it to the front of the set,
    // otherwise append it to the back of scriptSet.
    if (auto i = find(scriptSet.begin(), scriptSet.end(), sc); i != scriptSet.end())
        swap(*i, *scriptSet.begin());
    else
        scriptSet.push_back(sc);

    return scriptSet;
}

} // namespace unicode
