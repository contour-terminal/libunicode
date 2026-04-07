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

#include <algorithm>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace tablegen
{

/// Shared license header for all generated files.
constexpr std::string_view licenseHeader = R"(/**
* This file is part of the "libunicode" project
*   Copyright (c) 2020-2021 Christian Parpart <christian@parpart.family>
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
)";

/// Build name -> enum integer index map for a given enum, using the exact same
/// member ordering as the generated ucd_enums.h.
///
/// This must be called with the SAME member list that the enum generator uses,
/// so that integer values match the generated enum class values.
inline auto buildEnumIndexMap(std::vector<std::string> const& members) -> std::map<std::string, uint8_t>
{
    std::map<std::string, uint8_t> result;
    for (size_t i = 0; i < members.size(); ++i)
        result[members[i]] = static_cast<uint8_t>(i);
    return result;
}

inline auto sanitizeIdentifier(std::string const& id) -> std::string
{
    auto result = id;
    for (auto& ch: result)
        if (ch == ' ' || ch == '-')
            ch = '_';
    return result;
}

/// Build the Script enum members: Invalid, Unknown, Common, then sorted rest.
inline auto buildScriptMembers(std::vector<std::string> const& scriptProperties) -> std::vector<std::string>
{
    std::vector<std::string> result;
    result.push_back("Invalid");
    result.push_back("Unknown");
    result.push_back("Common");
    std::set<std::string> scriptSet;
    for (auto const& s: scriptProperties)
        scriptSet.insert(s);
    for (auto const& s: scriptSet)
        if (s != "Common")
            result.push_back(s);
    return result;
}

/// Build General_Category enum members: Unspecified, then sorted rest.
inline auto buildGeneralCategoryMembers(std::vector<std::string> const& gcProperties) -> std::vector<std::string>
{
    std::vector<std::string> result;
    result.push_back("Unspecified");
    std::set<std::string> cats;
    for (auto const& c: gcProperties)
        cats.insert(c);
    for (auto const& c: cats)
        result.push_back(c);
    return result;
}

/// Build PVA-based enum members: sorted values from PropertyValueAliases.
/// Optionally with a special first member (e.g., "Undefined" for Grapheme_Cluster_Break).
inline std::vector<std::string> buildPvaEnumMembers(std::map<std::string, std::string> const& aliases,
                                                    std::string const& firstMember = {})
{
    std::vector<std::string> result;
    if (!firstMember.empty())
        result.push_back(firstMember);
    std::vector<std::string> sorted;
    for (auto const& [k, v]: aliases)
        sorted.push_back(v);
    std::sort(sorted.begin(), sorted.end());
    for (auto const& v: sorted)
        result.push_back(v);
    return result;
}

/// Build EastAsianWidth enum members (custom order, not PVA-based).
inline auto buildEastAsianWidthMembers() -> std::vector<std::string>
{
    return { "Ambiguous", "FullWidth", "HalfWidth", "Neutral", "Narrow", "Wide", "Unspecified" };
}

} // namespace tablegen
