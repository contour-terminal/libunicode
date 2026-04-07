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
#include "enum_generator.h"

#include <algorithm>
#include <format>
#include <fstream>
#include <set>
#include <string>
#include <vector>

#include "enum_utils.h"
#include "ucd_parser.h"

namespace tablegen
{

namespace
{

    auto minimalUint(size_t maxValue) -> std::string
    {
        if (maxValue < (1u << 8))
            return "uint8_t";
        if (maxValue < (1u << 16))
            return "uint16_t";
        if (maxValue < (1ull << 32))
            return "uint32_t";
        return "uint64_t";
    }

    /// Represents a collected enum with its name and member list.
    struct EnumDef
    {
        std::string name;
        std::vector<std::string> members; // raw names (before sanitize_identifier)
    };

    void writeEnumClass(std::ofstream& out, EnumDef const& def)
    {
        out << std::format("enum class {}: {}\n{{\n", def.name, minimalUint(def.members.size()));
        for (size_t i = 0; i < def.members.size(); ++i)
            out << std::format("    {} = {},\n", sanitizeIdentifier(def.members[i]), i);
        out << "};\n\n";
    }

    void writeEnumOstream(std::ofstream& out, EnumDef const& def)
    {
        out << std::format("inline std::ostream& operator<<(std::ostream& os, {} value) noexcept\n{{\n", def.name);
        out << "    // clang-format off\n";
        out << "    switch (value)\n";
        out << "    {\n";
        for (auto const& member: def.members)
            out << std::format("    case {}::{}: return os << \"{}\";\n", def.name, sanitizeIdentifier(member), member);
        out << "    }\n";
        out << "    // clang-format on\n";
        out << "    return os << \"(\" << static_cast<unsigned>(value) << \")\";\n";
        out << "}\n\n";
    }

    void writeEnumFormatter(std::ofstream& out, EnumDef const& def)
    {
        auto qualifiedName = "unicode::" + def.name;
        out << "template <>\n";
        out << std::format("struct std::formatter<{}>: std::formatter<std::string_view>\n{{\n", qualifiedName);
        out << std::format("    auto format({} value, auto& ctx) const\n", qualifiedName);
        out << "    {\n";
        out << "        std::string_view name;\n";
        out << "        switch (value)\n";
        out << "        {\n";
        out << "            // clang-format off\n";
        for (auto const& member: def.members)
            out << std::format(
                "            case {}::{}: name = \"{}\"; break;\n", qualifiedName, sanitizeIdentifier(member), member);
        out << "            // clang-format off\n";
        out << "        }\n";
        out << "        return formatter<string_view>::format(name, ctx);\n";
        out << "    }\n";
        out << "};\n\n";
    }

    /// Build the list of all enums in the exact order mktables.py produces them.
    auto collectEnums(UcdParser const& parser) -> std::vector<EnumDef>
    {
        std::vector<EnumDef> enums;

        // 1. Plane enum (sort raw names, sanitize in output)
        {
            std::set<std::string> names;
            for (auto const& p: parser.planes())
                names.insert(p.name);
            EnumDef def;
            def.name = "Plane";
            for (auto const& n: names) // set is sorted on raw names
                def.members.push_back(sanitizeIdentifier(n));
            enums.push_back(std::move(def));
        }

        // 2. Property value alias enums (sorted by property name)
        {
            auto const& pva = parser.propertyValueAliases();
            for (auto const& [propName, values]: pva)
            {
                if (values.empty())
                    continue;

                // Skip boolean properties (Yes/No only)
                if (values.size() == 2)
                {
                    std::set<std::string> vals;
                    for (auto const& [k, v]: values)
                        vals.insert(v);
                    if (vals.count("Yes") && vals.count("No"))
                        continue;
                }

                // Skip properties generated elsewhere
                if (propName == "Script" || propName == "General_Category" || propName == "EastAsianWidth")
                    continue;

                EnumDef def;
                def.name = propName;

                // Special first member
                if (propName == "Grapheme_Cluster_Break")
                    def.members.push_back("Undefined");

                // Collect and sort values
                std::vector<std::string> sortedValues;
                for (auto const& [k, v]: values)
                    sortedValues.push_back(v);
                std::sort(sortedValues.begin(), sortedValues.end());
                for (auto const& v: sortedValues)
                    def.members.push_back(v);

                enums.push_back(std::move(def));
            }
        }

        // 3. Core_Property enum
        {
            EnumDef def;
            def.name = "Core_Property";
            auto const& props = parser.coreProperties();
            for (auto const& [name, ranges]: props) // map is sorted
                def.members.push_back(name);
            enums.push_back(std::move(def));
        }

        // 4. General_Category enum
        {
            EnumDef def;
            def.name = "General_Category";
            def.members.push_back("Unspecified");
            std::set<std::string> cats;
            for (auto const& r: parser.generalCategories())
                cats.insert(r.property);
            for (auto const& c: cats) // set is sorted
                def.members.push_back(c);
            enums.push_back(std::move(def));
        }

        // 5. Script enum
        {
            EnumDef def;
            def.name = "Script";
            def.members.push_back("Invalid");
            def.members.push_back("Unknown");
            def.members.push_back("Common");
            std::set<std::string> scriptSet;
            for (auto const& r: parser.scripts())
                scriptSet.insert(r.property);
            for (auto const& s: scriptSet) // set is sorted
            {
                if (s != "Common")
                    def.members.push_back(s);
            }
            enums.push_back(std::move(def));
        }

        // 6. Block enum (sort raw titles, sanitize in output)
        {
            EnumDef def;
            def.name = "Block";
            def.members.push_back("Unspecified");
            std::set<std::string> titles;
            for (auto const& b: parser.blocks())
                titles.insert(b.title);
            for (auto const& t: titles) // set is sorted on raw titles
                def.members.push_back(sanitizeIdentifier(t));
            enums.push_back(std::move(def));
        }

        // 7. EastAsianWidth enum
        {
            EnumDef def;
            def.name = "EastAsianWidth";
            // These are in the exact order from the Python WIDTH_NAMES dict
            def.members = { "Ambiguous", "FullWidth", "HalfWidth", "Neutral", "Narrow", "Wide", "Unspecified" };
            enums.push_back(std::move(def));
        }

        return enums;
    }

} // anonymous namespace

void generateEnumFiles(UcdParser const& parser, std::string const& outputDir)
{
    auto enums = collectEnums(parser);

    // ---- ucd_enums.h ----
    {
        auto out = std::ofstream(outputDir + "/ucd_enums.h", std::ios::binary);
        out << licenseHeader;
        out << "#pragma once\n";
        out << "\n";
        out << "#include <cstdint>\n";
        out << "\n";
        out << "namespace unicode\n{\n\n";
        for (auto const& def: enums)
            writeEnumClass(out, def);
        out << "} // namespace unicode\n";
    }

    // ---- ucd_ostream.h ----
    {
        auto out = std::ofstream(outputDir + "/ucd_ostream.h", std::ios::binary);
        out << licenseHeader;
        out << "#pragma once\n";
        out << "\n";
        out << "#include <libunicode/ucd.h>\n";
        out << "\n";
        out << "#include <ostream>\n";
        out << "\n";
        out << "namespace unicode\n{\n\n";
        for (auto const& def: enums)
            writeEnumOstream(out, def);
        out << "} // namespace unicode\n";
    }

    // ---- ucd_fmt.h ----
    {
        auto out = std::ofstream(outputDir + "/ucd_fmt.h", std::ios::binary);
        out << licenseHeader;
        out << "#pragma once\n";
        out << "\n";
        out << "#include <libunicode/ucd_enums.h>\n";
        out << "\n";
        out << "#include <format>\n";
        out << "\n";
        for (auto const& def: enums)
            writeEnumFormatter(out, def);
    }
}

} // namespace tablegen
