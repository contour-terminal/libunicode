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
#include "case_norm_generator.h"

#include <algorithm>
#include <format>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "enum_utils.h"
#include "ucd_parser.h"

namespace tablegen
{

namespace
{

    void writeSimpleCaseTable(std::ofstream& out, std::map<char32_t, char32_t> const& mappings, std::string const& name)
    {
        auto sortedItems = std::vector<std::pair<char32_t, char32_t>>(mappings.begin(), mappings.end());
        std::sort(sortedItems.begin(), sortedItems.end());

        out << std::format("// Simple {} mappings: source -> target\n", name);
        out << std::format("// Total entries: {}\n", sortedItems.size());
        out << std::format(
            "inline constexpr std::array<std::pair<char32_t, char32_t>, {}> simple_{}_table {{{{\n", sortedItems.size(), name);

        for (size_t i = 0; i < sortedItems.size(); ++i)
        {
            auto const& [src, tgt] = sortedItems[i];
            auto comma = (i < sortedItems.size() - 1) ? "," : "";
            auto comment = (src >= 0x20 && src < 0x7F) ? std::string(1, static_cast<char>(src)) : std::string {};
            out << std::format(
                "    {{ 0x{:04X}, 0x{:04X} }}{} // {}\n", static_cast<unsigned>(src), static_cast<unsigned>(tgt), comma, comment);
        }
        out << "}};\n";
    }

    void writeFullCaseTable(std::ofstream& out,
                            std::map<char32_t, std::vector<char32_t>> const& mappings,
                            std::string const& name)
    {
        auto sortedItems = std::vector<std::pair<char32_t, std::vector<char32_t>>>(mappings.begin(), mappings.end());
        std::sort(sortedItems.begin(), sortedItems.end(), [](auto const& a, auto const& b) { return a.first < b.first; });

        out << std::format("// Full {} mappings: source -> [target1, target2, target3]\n", name);
        out << std::format("// Total entries: {}\n", sortedItems.size());
        out << std::format("struct full_{}_entry {{\n", name);
        out << "    char32_t source;\n";
        out << "    char32_t targets[3];\n";
        out << "    uint8_t length;\n";
        out << "};\n\n";
        out << std::format("inline constexpr std::array<full_{}_entry, {}> full_{}_table {{{{\n", name, sortedItems.size(), name);

        for (size_t i = 0; i < sortedItems.size(); ++i)
        {
            auto const& [src, targets] = sortedItems[i];
            auto comma = (i < sortedItems.size() - 1) ? "," : "";
            // Pad targets to 3
            auto padded = targets;
            while (padded.size() < 3)
                padded.push_back(char32_t(0));
            out << std::format("    {{ 0x{:04X}, {{ 0x{:04X}, 0x{:04X}, 0x{:04X} }}, {} }}{}\n",
                               static_cast<unsigned>(src),
                               static_cast<unsigned>(padded[0]),
                               static_cast<unsigned>(padded[1]),
                               static_cast<unsigned>(padded[2]),
                               targets.size(),
                               comma);
        }
        out << "}};\n";
    }

    void writeCccTable(std::ofstream& out, std::map<char32_t, int> const& cccMap)
    {
        auto sortedItems = std::vector<std::pair<char32_t, int>>(cccMap.begin(), cccMap.end());
        std::sort(sortedItems.begin(), sortedItems.end());

        out << "// Canonical Combining Class entries for non-zero CCC values\n";
        out << std::format("// Total entries: {}\n", sortedItems.size());
        out << std::format("inline constexpr std::array<std::pair<char32_t, uint8_t>, {}> ccc_table {{{{\n", sortedItems.size());

        for (size_t i = 0; i < sortedItems.size(); ++i)
        {
            auto const& [cp, ccc] = sortedItems[i];
            auto comma = (i < sortedItems.size() - 1) ? "," : "";
            out << std::format("    {{ 0x{:04X}, {} }}{}\n", static_cast<unsigned>(cp), ccc, comma);
        }
        out << "}};\n";
    }

    void writeDecompositionTable(std::ofstream& out, std::map<char32_t, Decomposition> const& decomps)
    {
        // Filter to canonical only
        std::vector<std::pair<char32_t, Decomposition const*>> canonical;
        for (auto const& [cp, d]: decomps)
            if (d.type == "canonical")
                canonical.emplace_back(cp, &d);
        std::sort(canonical.begin(), canonical.end(), [](auto const& a, auto const& b) { return a.first < b.first; });

        // Find max length
        size_t maxLen = 4;
        for (auto const& [cp, d]: canonical)
            maxLen = std::max(maxLen, d->targets.size());

        out << "// Canonical decomposition mappings\n";
        out << std::format("// Total entries: {}, max length: {}\n", canonical.size(), maxLen);
        out << "struct canonical_decomposition_entry {\n";
        out << "    char32_t source;\n";
        out << std::format("    char32_t targets[{}];\n", maxLen);
        out << "    uint8_t length;\n";
        out << "};\n\n";
        out << std::format("inline constexpr std::array<canonical_decomposition_entry, {}> canonical_decomposition_table {{{{\n",
                           canonical.size());

        for (size_t i = 0; i < canonical.size(); ++i)
        {
            auto const& [cp, d] = canonical[i];
            auto comma = (i < canonical.size() - 1) ? "," : "";
            auto padded = d->targets;
            while (padded.size() < maxLen)
                padded.push_back(char32_t(0));
            out << std::format("    {{ 0x{:04X}, {{ ", static_cast<unsigned>(cp));
            for (size_t j = 0; j < padded.size(); ++j)
            {
                if (j > 0)
                    out << ", ";
                out << std::format("0x{:04X}", static_cast<unsigned>(padded[j]));
            }
            out << std::format(" }}, {} }}{}\n", d->targets.size(), comma);
        }
        out << "}};\n";
    }

    void writeCompositionTable(std::ofstream& out,
                               std::map<char32_t, Decomposition> const& decomps,
                               std::set<char32_t> const& exclusions)
    {
        // Only canonical decompositions of length 2 that are not excluded
        struct CompositionEntry
        {
            char32_t first;
            char32_t second;
            char32_t composed;
        };
        std::vector<CompositionEntry> compositions;
        for (auto const& [cp, d]: decomps)
        {
            if (d.type == "canonical" && d.targets.size() == 2 && exclusions.find(cp) == exclusions.end())
                compositions.push_back({ d.targets[0], d.targets[1], cp });
        }
        std::sort(compositions.begin(), compositions.end(), [](auto const& a, auto const& b) {
            if (a.first != b.first)
                return a.first < b.first;
            return a.second < b.second;
        });

        out << "// Canonical composition pairs (first + second -> composed)\n";
        out << std::format("// Total entries: {}\n", compositions.size());
        out << "struct composition_pair {\n";
        out << "    char32_t first;\n";
        out << "    char32_t second;\n";
        out << "    char32_t composed;\n";
        out << "};\n\n";
        out << std::format("inline constexpr std::array<composition_pair, {}> composition_table {{{{\n", compositions.size());

        for (size_t i = 0; i < compositions.size(); ++i)
        {
            auto const& c = compositions[i];
            auto comma = (i < compositions.size() - 1) ? "," : "";
            out << std::format("    {{ 0x{:04X}, 0x{:04X}, 0x{:04X} }}{}\n",
                               static_cast<unsigned>(c.first),
                               static_cast<unsigned>(c.second),
                               static_cast<unsigned>(c.composed),
                               comma);
        }
        out << "}};\n";
    }

    /// Maps decomposition type string from UnicodeData.txt to Decomposition_Type enum ordinal.
    [[nodiscard]] uint8_t decompositionTypeOrdinal(std::string const& type) noexcept
    {
        // clang-format off
        if (type == "circle")   return 1;
        if (type == "compat")   return 2;
        if (type == "final")    return 3;
        if (type == "font")     return 4;
        if (type == "fraction") return 5;
        if (type == "initial")  return 6;
        if (type == "isolated") return 7;
        if (type == "medial")   return 8;
        if (type == "narrow")   return 9;
        if (type == "noBreak")  return 10;
        if (type == "small")    return 12;
        if (type == "square")   return 13;
        if (type == "sub")      return 14;
        if (type == "super")    return 15;
        if (type == "vertical") return 16;
        if (type == "wide")     return 17;
        // clang-format on
        return 2; // Default to Compat
    }

    void writeCompatibilityDecompositionTable(std::ofstream& out, std::map<char32_t, Decomposition> const& decomps)
    {
        // Filter to compatibility only (type != "canonical")
        std::vector<std::pair<char32_t, Decomposition const*>> compat;
        for (auto const& [cp, d]: decomps)
            if (d.type != "canonical")
                compat.emplace_back(cp, &d);
        std::sort(compat.begin(), compat.end(), [](auto const& a, auto const& b) { return a.first < b.first; });

        // Find max length
        size_t maxLen = 4;
        for (auto const& [cp, d]: compat)
            maxLen = std::max(maxLen, d->targets.size());

        out << "// Compatibility decomposition mappings\n";
        out << std::format("// Total entries: {}, max length: {}\n", compat.size(), maxLen);
        out << "struct compatibility_decomposition_entry {\n";
        out << "    char32_t source;\n";
        out << std::format("    char32_t targets[{}];\n", maxLen);
        out << "    uint8_t length;\n";
        out << "    uint8_t decomp_type; // maps to Decomposition_Type enum ordinal\n";
        out << "};\n\n";
        out << std::format(
            "inline constexpr std::array<compatibility_decomposition_entry, {}> compatibility_decomposition_table {{{{\n",
            compat.size());

        for (size_t i = 0; i < compat.size(); ++i)
        {
            auto const& [cp, d] = compat[i];
            auto comma = (i < compat.size() - 1) ? "," : "";
            auto padded = d->targets;
            while (padded.size() < maxLen)
                padded.push_back(char32_t(0));
            out << std::format("    {{ 0x{:04X}, {{ ", static_cast<unsigned>(cp));
            for (size_t j = 0; j < padded.size(); ++j)
            {
                if (j > 0)
                    out << ", ";
                out << std::format("0x{:04X}", static_cast<unsigned>(padded[j]));
            }
            out << std::format(" }}, {}, {} }}{}\n", d->targets.size(), decompositionTypeOrdinal(d->type), comma);
        }
        out << "}};\n";
    }

    void writeQuickCheckTable(std::ofstream& out, std::set<char32_t> const& codepoints, std::string const& name)
    {
        auto sorted = std::vector<char32_t>(codepoints.begin(), codepoints.end());
        std::sort(sorted.begin(), sorted.end());

        out << std::format("// {} codepoints\n", name);
        out << std::format("// Total entries: {}\n", sorted.size());
        out << std::format("inline constexpr std::array<char32_t, {}> {}_table {{{{\n", sorted.size(), name);

        for (size_t i = 0; i < sorted.size(); i += 8)
        {
            out << "    ";
            for (size_t j = i; j < std::min(i + 8, sorted.size()); ++j)
            {
                if (j > i)
                    out << ", ";
                out << std::format("0x{:04X}", static_cast<unsigned>(sorted[j]));
            }
            if (i + 8 < sorted.size())
                out << ",";
            out << "\n";
        }
        out << "}};\n";
    }

} // anonymous namespace

void generateCaseNormFile(UcdParser const& parser, std::string const& outputDir)
{
    auto out = std::ofstream(outputDir + "/case_normalization_data.h", std::ios::binary);

    out << licenseHeader;
    out << R"(
#pragma once

#include <array>
#include <cstdint>
#include <utility>

namespace unicode::detail
{

// clang-format off

)";

    // Simple case mappings
    writeSimpleCaseTable(out, parser.simpleUppercase(), "uppercase");
    out << "\n";
    writeSimpleCaseTable(out, parser.simpleLowercase(), "lowercase");
    out << "\n";
    writeSimpleCaseTable(out, parser.simpleTitlecase(), "titlecase");
    out << "\n";
    writeSimpleCaseTable(out, parser.simpleCasefold(), "casefold");
    out << "\n";

    // Full case mappings
    writeFullCaseTable(out, parser.fullUppercase(), "uppercase");
    out << "\n";
    writeFullCaseTable(out, parser.fullLowercase(), "lowercase");
    out << "\n";
    writeFullCaseTable(out, parser.fullTitlecase(), "titlecase");
    out << "\n";
    writeFullCaseTable(out, parser.fullCasefold(), "casefold");
    out << "\n";

    // CCC
    writeCccTable(out, parser.ccc());
    out << "\n";

    // Canonical decomposition
    writeDecompositionTable(out, parser.decompositions());
    out << "\n";

    // Compatibility decomposition
    writeCompatibilityDecompositionTable(out, parser.decompositions());
    out << "\n";

    // Composition
    writeCompositionTable(out, parser.decompositions(), parser.compositionExclusions());
    out << "\n";

    // Quick check tables
    writeQuickCheckTable(out, parser.nfcQcNo(), "nfc_qc_no");
    out << "\n";
    writeQuickCheckTable(out, parser.nfcQcMaybe(), "nfc_qc_maybe");
    out << "\n";
    writeQuickCheckTable(out, parser.nfkcQcNo(), "nfkc_qc_no");
    out << "\n";
    writeQuickCheckTable(out, parser.nfkcQcMaybe(), "nfkc_qc_maybe");
    out << "\n";
    writeQuickCheckTable(out, parser.nfdQcNo(), "nfd_qc_no");
    out << "\n";
    writeQuickCheckTable(out, parser.nfkdQcNo(), "nfkd_qc_no");
    out << "\n";

    // Composition exclusions
    writeQuickCheckTable(out, parser.compositionExclusions(), "composition_exclusions");
    out << "\n";

    out << "// clang-format on\n";
    out << "\n";
    out << "} // namespace unicode::detail\n";
}

} // namespace tablegen
