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
#include "ucd_parser.h"

#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>

namespace tablegen
{

namespace
{
    auto parseCodepoint(std::string_view s) -> char32_t
    {
        while (!s.empty() && s.front() == ' ')
            s.remove_prefix(1);
        while (!s.empty() && s.back() == ' ')
            s.remove_suffix(1);
        unsigned value = 0;
        std::from_chars(s.data(), s.data() + s.size(), value, 16);
        return static_cast<char32_t>(value);
    }

    auto parseCodepoints(std::string_view s) -> std::vector<char32_t>
    {
        std::vector<char32_t> result;
        while (!s.empty())
        {
            while (!s.empty() && s.front() == ' ')
                s.remove_prefix(1);
            if (s.empty())
                break;
            auto end = s.find(' ');
            if (end == std::string_view::npos)
                end = s.size();
            result.push_back(parseCodepoint(s.substr(0, end)));
            s.remove_prefix(end);
        }
        return result;
    }

    auto trim(std::string_view s) -> std::string_view
    {
        while (!s.empty() && (s.front() == ' ' || s.front() == '\t' || s.front() == '\r' || s.front() == '\n'))
            s.remove_prefix(1);
        while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n'))
            s.remove_suffix(1);
        return s;
    }

    auto openFile(std::string const& path) -> std::ifstream
    {
        auto f = std::ifstream(path);
        if (!f.is_open())
            throw std::runtime_error("Failed to open file: " + path);
        return f;
    }
    /// Parsed result from a standard UCD data line.
    struct ParsedLine
    {
        char32_t first {};
        char32_t last {};
        bool isRange {};
        std::string_view property; ///< First field after ';'
        std::string_view comment;  ///< Text after '#'
    };

    /// Parse a standard UCD line: HEX[..HEX] ; PROPERTY # comment
    /// Returns nullopt for comment-only or blank lines.
    auto parseStandardLine(std::string_view line) -> std::optional<ParsedLine>
    {
        // Strip trailing CR
        if (!line.empty() && line.back() == '\r')
            line.remove_suffix(1);
        if (line.empty() || line.front() == '#' || line.front() == '@')
            return std::nullopt;

        ParsedLine result {};

        // Split off comment
        auto hashPos = line.find('#');
        if (hashPos != std::string_view::npos)
        {
            result.comment = trim(line.substr(hashPos + 1));
            line = line.substr(0, hashPos);
        }

        // Split codepoint(s) from property at first ';'
        auto semiPos = line.find(';');
        if (semiPos == std::string_view::npos)
            return std::nullopt;

        auto cpPart = trim(line.substr(0, semiPos));
        result.property = trim(line.substr(semiPos + 1));

        // Check for range (..)
        auto dotdot = cpPart.find("..");
        if (dotdot != std::string_view::npos)
        {
            result.first = parseCodepoint(cpPart.substr(0, dotdot));
            result.last = parseCodepoint(cpPart.substr(dotdot + 2));
            result.isRange = true;
        }
        else
        {
            result.first = parseCodepoint(cpPart);
            result.last = result.first;
            result.isRange = false;
        }

        return result;
    }

    /// Extract just the first word from a property field (e.g., "Latin" from "Latin # ...")
    auto firstWord(std::string_view s) -> std::string_view
    {
        s = trim(s);
        auto end = s.find_first_of(" \t;");
        if (end != std::string_view::npos)
            return s.substr(0, end);
        return s;
    }

    /// Split a string_view by a delimiter, returning a vector of trimmed parts.
    auto splitSemicolons(std::string_view s) -> std::vector<std::string_view>
    {
        std::vector<std::string_view> result;
        while (!s.empty())
        {
            auto pos = s.find(';');
            if (pos == std::string_view::npos)
            {
                result.push_back(trim(s));
                break;
            }
            result.push_back(trim(s.substr(0, pos)));
            s.remove_prefix(pos + 1);
        }
        return result;
    }

} // namespace

UcdParser::UcdParser(std::string ucdDirectory): _ucdDir(std::move(ucdDirectory))
{
    // Initialize plane definitions (same as Python PLANES constant)
    _planes = {
        { 0, 0x0000, 0x0FFFF, "BMP", "Basic Multilingual Plane" },
        { 1, 0x10000, 0x1FFFF, "SMP", "Supplementary Multilingual Plane" },
        { 2, 0x20000, 0x2FFFF, "SIP", "Supplementary Ideographic Plane" },
        { 3, 0x30000, 0x3FFFF, "TIP", "Tertiary Ideographic Plane" },
        { 4, 0x40000, 0x4FFFF, "", "Unassigned" },
        { 5, 0x50000, 0x5FFFF, "", "Unassigned" },
        { 6, 0x60000, 0x6FFFF, "", "Unassigned" },
        { 7, 0x70000, 0x7FFFF, "", "Unassigned" },
        { 8, 0x80000, 0x8FFFF, "", "Unassigned" },
        { 9, 0x90000, 0x9FFFF, "", "Unassigned" },
        { 10, 0xA0000, 0xAFFFF, "", "Unassigned" },
        { 11, 0xB0000, 0xBFFFF, "", "Unassigned" },
        { 12, 0xC0000, 0xCFFFF, "", "Unassigned" },
        { 13, 0xD0000, 0xDFFFF, "", "Unassigned" },
        { 14, 0xE0000, 0xEFFFF, "SSP", "Supplementary Special-purpose Plane" },
        { 15, 0xF0000, 0xFFFFF, "SPUA-A", "Supplementary Private Use Area Plane" },
        { 16, 0x100000, 0x10FFFF, "SPUA-B", "Supplementary Private Use Area Plane" },
    };
}

void UcdParser::parseAll()
{
    // Phase 1: Enum-related files
    loadPropertyValueAliases();
    loadGeneralCategory();
    loadCoreProperties();
    loadScripts();
    loadScriptExtensions();
    loadBlocks();

    // Phase 2: UCD API files
    loadGraphemeBreakProps();
    loadEastAsianWidths();
    loadHangulSyllableType();
    loadEmojiProps();
    loadBidiMirrored();
    loadBidiMirroringGlyph();

    // Phase 3: Case/normalization
    loadUnicodeData();
    loadCaseFolding();
    loadSpecialCasing();
    loadCompositionExclusions();
    loadDerivedNormalizationProps();

    // Phase 4: Multistage tables
    loadDerivedAge();
    loadDerivedNames();
}

// ---- Generic helpers ----

auto UcdParser::loadGenericProperties(std::string const& filename) const -> std::vector<PropertyRange>
{
    std::vector<PropertyRange> result;
    auto f = openFile(filename);
    std::string line;
    while (std::getline(f, line))
    {
        auto parsed = parseStandardLine(line);
        if (!parsed)
            continue;
        auto prop = firstWord(parsed->property);
        result.push_back({ parsed->first, parsed->last, std::string(prop), std::string(parsed->comment) });
    }
    std::sort(result.begin(), result.end(), [](auto const& a, auto const& b) { return a.first < b.first; });
    return result;
}

std::map<std::string, std::vector<PropertyRange>> UcdParser::loadGroupedProperties(std::string const& filename,
                                                                                   std::string const& headerKey) const
{
    // Header pattern: "# <headerKey>: <value>"
    auto const headerPrefix = std::string("# ") + headerKey + ": ";

    std::map<std::string, std::vector<PropertyRange>> result;
    auto f = openFile(filename);
    std::string line;
    std::string currentGroup;
    while (std::getline(f, line))
    {
        // Check for section header
        auto trimmed = trim(line);
        if (trimmed.starts_with("#"))
        {
            // Try to match "# Property: VALUE" pattern
            auto sv = std::string_view(line);
            if (auto pos = sv.find(headerKey + ":"); pos != std::string_view::npos && sv[0] == '#')
            {
                auto valueStart = pos + headerKey.size() + 1;
                auto value = trim(sv.substr(valueStart));
                auto word = firstWord(value);
                currentGroup = std::string(word);
                if (result.find(currentGroup) == result.end())
                    result[currentGroup] = {};
            }
            continue;
        }

        auto parsed = parseStandardLine(line);
        if (!parsed)
            continue;
        auto prop = firstWord(parsed->property);
        result[currentGroup].push_back({ parsed->first, parsed->last, std::string(prop), std::string(parsed->comment) });
    }
    for (auto& [key, ranges]: result)
        std::sort(ranges.begin(), ranges.end(), [](auto const& a, auto const& b) { return a.first < b.first; });
    return result;
}

// ---- Property Value Aliases ----

void UcdParser::loadPropertyValueAliases()
{
    std::set<std::string> blackList = { "Block" };

    auto f = openFile(_ucdDir + "/PropertyValueAliases.txt");
    std::string line;
    std::string currentProperty;
    while (std::getline(f, line))
    {
        auto sv = trim(std::string_view(line));
        if (sv.empty())
            continue;

        // Header: "# PropertyName (abbrev)"
        if (sv.starts_with("# "))
        {
            auto content = sv.substr(2);
            auto parenPos = content.find(" (");
            if (parenPos != std::string_view::npos && content.back() == ')')
            {
                currentProperty = std::string(content.substr(0, parenPos));
                if (!blackList.count(currentProperty))
                    _propertyValues[currentProperty] = {};
            }
            continue;
        }
        if (sv.front() == '#')
            continue;

        // Data line: "propAbbrev ; valueAbbrev ; valueFull ..."
        auto parts = splitSemicolons(sv);
        if (parts.size() < 3)
            continue;
        if (blackList.count(currentProperty))
            continue;

        auto valueAbbrev = std::string(firstWord(parts[1]));
        auto valueFull = std::string(firstWord(parts[2]));
        _propertyValues[currentProperty][valueAbbrev] = valueFull;
    }
}

// ---- General Category ----

void UcdParser::loadGeneralCategory()
{
    auto const& propertyValues = _propertyValues.at("General_Category");
    auto const headerPrefix = std::string_view("# General_Category=");

    auto f = openFile(_ucdDir + "/extracted/DerivedGeneralCategory.txt");
    std::string line;
    std::string catName;
    while (std::getline(f, line))
    {
        auto sv = std::string_view(line);
        // Check for "# General_Category=XX" header
        if (sv.starts_with(headerPrefix))
        {
            catName = std::string(trim(sv.substr(headerPrefix.size())));
            if (_generalCategoryMap.find(catName) == _generalCategoryMap.end())
                _generalCategoryMap[catName] = {};
            continue;
        }

        auto parsed = parseStandardLine(line);
        if (!parsed)
            continue;

        auto abbrev = std::string(firstWord(parsed->property));
        auto const& prop = propertyValues.at(abbrev);
        auto comment = std::string(parsed->comment);
        _generalCategoryMap[catName].push_back({ parsed->first, parsed->last, prop, comment });
        _generalCategory.push_back({ parsed->first, parsed->last, prop, comment });
    }
    std::sort(_generalCategory.begin(), _generalCategory.end(), [](auto const& a, auto const& b) { return a.first < b.first; });
}

// ---- Core Properties ----

void UcdParser::loadCoreProperties()
{
    auto f = openFile(_ucdDir + "/DerivedCoreProperties.txt");
    std::string line;
    while (std::getline(f, line))
    {
        auto parsed = parseStandardLine(line);
        if (!parsed)
            continue;
        // Handle multi-value entries (e.g., "InCB; Consonant")
        auto const semicolonPos = parsed->property.find(';');
        if (semicolonPos != std::string_view::npos)
        {
            auto const propName = std::string(trim(parsed->property.substr(0, semicolonPos)));
            auto const propValue = std::string(trim(parsed->property.substr(semicolonPos + 1)));
            if (propName == "InCB")
                _indicConjunctBreak[propValue].push_back(
                    { parsed->first, parsed->last, propValue, std::string(parsed->comment) });
            continue;
        }
        auto prop = std::string(firstWord(parsed->property));
        _coreProperties[prop].push_back({ parsed->first, parsed->last, "", std::string(parsed->comment) });
    }
    for (auto& [key, ranges]: _coreProperties)
        std::sort(ranges.begin(), ranges.end(), [](auto const& a, auto const& b) { return a.first < b.first; });
    for (auto& [key, ranges]: _indicConjunctBreak)
        std::sort(ranges.begin(), ranges.end(), [](auto const& a, auto const& b) { return a.first < b.first; });
}

// ---- Scripts ----

void UcdParser::loadScripts()
{
    _scripts = loadGenericProperties(_ucdDir + "/Scripts.txt");
}

// ---- Script Extensions ----

void UcdParser::loadScriptExtensions()
{
    auto f = openFile(_ucdDir + "/ScriptExtensions.txt");
    std::string line;
    while (std::getline(f, line))
    {
        auto parsed = parseStandardLine(line);
        if (!parsed)
            continue;

        // Property field contains space-separated script abbreviations
        auto propStr = std::string(trim(parsed->property));
        std::vector<std::string> props;
        std::istringstream iss(propStr);
        std::string token;
        while (iss >> token)
            props.push_back(token);
        std::sort(props.begin(), props.end());

        _scriptExtensions.push_back({ parsed->first, parsed->last, std::move(props), std::string(parsed->comment) });
    }
    std::sort(_scriptExtensions.begin(), _scriptExtensions.end(), [](auto const& a, auto const& b) { return a.first < b.first; });
}

// ---- Blocks ----

void UcdParser::loadBlocks()
{
    auto f = openFile(_ucdDir + "/Blocks.txt");
    std::string line;
    while (std::getline(f, line))
    {
        auto parsed = parseStandardLine(line);
        if (!parsed || !parsed->isRange)
            continue;
        auto title = std::string(trim(parsed->property));
        _blocks.push_back({ parsed->first, parsed->last, title });
    }
}

// ---- Grapheme Break Properties ----

void UcdParser::loadGraphemeBreakProps()
{
    _graphemeBreakProps = loadGroupedProperties(_ucdDir + "/auxiliary/GraphemeBreakProperty.txt", "Property");
}

// ---- East Asian Widths ----

void UcdParser::loadEastAsianWidths()
{
    _eastAsianWidths = loadGenericProperties(_ucdDir + "/EastAsianWidth.txt");
}

// ---- Hangul Syllable Type ----

void UcdParser::loadHangulSyllableType()
{
    _hangulSyllableType = loadGenericProperties(_ucdDir + "/HangulSyllableType.txt");
}

// ---- Emoji Properties ----

void UcdParser::loadEmojiProps()
{
    auto f = openFile(_ucdDir + "/emoji/emoji-data.txt");
    std::string line;
    while (std::getline(f, line))
    {
        auto parsed = parseStandardLine(line);
        if (!parsed)
            continue;
        auto name = std::string(firstWord(parsed->property));
        _emojiProps[name].push_back({ parsed->first, parsed->last, name, std::string(parsed->comment) });
    }
    for (auto& [key, ranges]: _emojiProps)
        std::sort(ranges.begin(), ranges.end(), [](auto const& a, auto const& b) { return a.first < b.first; });
}

// ---- Bidi Mirrored ----

void UcdParser::loadBidiMirrored()
{
    auto f = openFile(_ucdDir + "/UnicodeData.txt");
    std::string line;
    std::vector<char32_t> mirroredCps;
    while (std::getline(f, line))
    {
        auto trimmed = std::string(trim(line));
        if (trimmed.empty() || trimmed[0] == '#')
            continue;

        // Split by semicolons, check field 9
        size_t fieldIdx = 0;
        size_t pos = 0;
        std::string_view sv(trimmed);
        char32_t cp = 0;
        for (size_t i = 0; i <= sv.size(); ++i)
        {
            if (i == sv.size() || sv[i] == ';')
            {
                auto field = sv.substr(pos, i - pos);
                if (fieldIdx == 0)
                    cp = parseCodepoint(field);
                else if (fieldIdx == 9)
                {
                    if (trim(field) == "Y")
                        mirroredCps.push_back(cp);
                    break;
                }
                pos = i + 1;
                ++fieldIdx;
            }
        }
    }
    std::sort(mirroredCps.begin(), mirroredCps.end());

    // Compress into intervals
    for (auto cp: mirroredCps)
    {
        if (!_bidiMirroredIntervals.empty() && _bidiMirroredIntervals.back().second + 1 == cp)
            _bidiMirroredIntervals.back().second = cp;
        else
            _bidiMirroredIntervals.emplace_back(cp, cp);
    }
}

// ---- Bidi Mirroring Glyph ----

void UcdParser::loadBidiMirroringGlyph()
{
    auto f = openFile(_ucdDir + "/BidiMirroring.txt");
    std::string line;
    while (std::getline(f, line))
    {
        auto parsed = parseStandardLine(line);
        if (!parsed)
            continue;
        // Property field is the target codepoint
        auto target = parseCodepoint(parsed->property);
        _bidiMirroringGlyphPairs.emplace_back(parsed->first, target);
    }
    std::sort(_bidiMirroringGlyphPairs.begin(), _bidiMirroringGlyphPairs.end());
}

// ---- UnicodeData.txt (case mappings, CCC, decompositions) ----

void UcdParser::loadUnicodeData()
{
    auto f = openFile(_ucdDir + "/UnicodeData.txt");
    std::string line;
    while (std::getline(f, line))
    {
        auto trimmed = std::string(trim(line));
        if (trimmed.empty() || trimmed[0] == '#')
            continue;

        // Split by semicolons
        std::vector<std::string_view> fields;
        std::string_view sv(trimmed);
        size_t pos = 0;
        for (size_t i = 0; i <= sv.size(); ++i)
        {
            if (i == sv.size() || sv[i] == ';')
            {
                fields.push_back(sv.substr(pos, i - pos));
                pos = i + 1;
            }
        }
        if (fields.size() < 15)
            continue;

        auto cp = parseCodepoint(fields[0]);

        // Field 3: Canonical Combining Class
        auto cccField = trim(fields[3]);
        if (!cccField.empty())
        {
            int cccVal = 0;
            std::from_chars(cccField.data(), cccField.data() + cccField.size(), cccVal);
            if (cccVal != 0)
                _ccc[cp] = cccVal;
        }

        // Field 5: Decomposition mapping
        auto decompField = trim(fields[5]);
        if (!decompField.empty())
        {
            std::string decompType = "canonical";
            std::string_view decompStr = decompField;
            if (decompStr.front() == '<')
            {
                auto endAngle = decompStr.find('>');
                decompType = std::string(decompStr.substr(1, endAngle - 1));
                decompStr = trim(decompStr.substr(endAngle + 1));
            }
            if (!decompStr.empty())
            {
                auto targets = parseCodepoints(decompStr);
                _decompositions[cp] = { cp, targets, decompType };
            }
        }

        // Field 12: Simple Uppercase Mapping
        auto upperField = trim(fields[12]);
        if (!upperField.empty())
            _simpleUppercase[cp] = parseCodepoint(upperField);

        // Field 13: Simple Lowercase Mapping
        auto lowerField = trim(fields[13]);
        if (!lowerField.empty())
            _simpleLowercase[cp] = parseCodepoint(lowerField);

        // Field 14: Simple Titlecase Mapping
        auto titleField = trim(fields[14]);
        if (!titleField.empty())
            _simpleTitlecase[cp] = parseCodepoint(titleField);
    }
}

// ---- Case Folding ----

void UcdParser::loadCaseFolding()
{
    auto f = openFile(_ucdDir + "/CaseFolding.txt");
    std::string line;
    while (std::getline(f, line))
    {
        auto trimmed = std::string(trim(line));
        if (trimmed.empty() || trimmed[0] == '#')
            continue;

        // Format: <code>; <status>; <mapping>; # <name>
        std::vector<std::string_view> parts;
        std::string_view sv(trimmed);
        size_t pos = 0;
        for (size_t i = 0; i <= sv.size(); ++i)
        {
            if (i == sv.size() || sv[i] == ';')
            {
                parts.push_back(sv.substr(pos, i - pos));
                pos = i + 1;
            }
        }
        if (parts.size() < 3)
            continue;

        auto cp = parseCodepoint(parts[0]);
        auto status = trim(parts[1]);
        auto mapping = parseCodepoints(parts[2]);

        if (status == "C")
        {
            if (mapping.size() == 1)
                _simpleCasefold[cp] = mapping[0];
            _fullCasefold[cp] = mapping;
        }
        else if (status == "S")
        {
            if (mapping.size() == 1)
                _simpleCasefold[cp] = mapping[0];
        }
        else if (status == "F")
        {
            _fullCasefold[cp] = mapping;
        }
        // 'T' (Turkic) is ignored
    }
}

// ---- Special Casing ----

void UcdParser::loadSpecialCasing()
{
    auto f = openFile(_ucdDir + "/SpecialCasing.txt");
    std::string line;
    while (std::getline(f, line))
    {
        auto trimmed = std::string(trim(line));
        if (trimmed.empty() || trimmed[0] == '#')
            continue;

        // Format: <code>; <lower>; <title>; <upper>; (<condition_list>;)? # <comment>
        std::vector<std::string_view> parts;
        std::string_view sv(trimmed);
        size_t pos = 0;
        for (size_t i = 0; i <= sv.size(); ++i)
        {
            if (i == sv.size() || sv[i] == ';')
            {
                parts.push_back(sv.substr(pos, i - pos));
                pos = i + 1;
            }
        }
        if (parts.size() < 4)
            continue;

        // Check for conditional mappings: skip them (have > 4 fields before '#')
        auto commentIdx = trimmed.find('#');
        if (commentIdx != std::string::npos)
        {
            auto beforeComment = std::string_view(trimmed).substr(0, commentIdx);
            auto fieldCount = std::count(beforeComment.begin(), beforeComment.end(), ';');
            if (fieldCount > 4)
                continue;
        }

        auto cp = parseCodepoint(parts[0]);
        auto lower = parseCodepoints(parts[1]);
        auto title = parseCodepoints(parts[2]);
        auto upper = parseCodepoints(parts[3]);

        // Only store non-trivial mappings
        auto expectedLower = _simpleLowercase.count(cp) ? _simpleLowercase.at(cp) : cp;
        auto expectedTitle = _simpleTitlecase.count(cp) ? _simpleTitlecase.at(cp) : cp;
        auto expectedUpper = _simpleUppercase.count(cp) ? _simpleUppercase.at(cp) : cp;
        if (lower.size() > 1 || (lower.size() == 1 && lower[0] != expectedLower))
            _fullLowercase[cp] = lower;
        if (title.size() > 1 || (title.size() == 1 && title[0] != expectedTitle))
            _fullTitlecase[cp] = title;
        if (upper.size() > 1 || (upper.size() == 1 && upper[0] != expectedUpper))
            _fullUppercase[cp] = upper;
    }
}

// ---- Composition Exclusions ----

void UcdParser::loadCompositionExclusions()
{
    auto f = openFile(_ucdDir + "/CompositionExclusions.txt");
    std::string line;
    while (std::getline(f, line))
    {
        auto trimmed = std::string(trim(line));
        if (trimmed.empty() || trimmed[0] == '#')
            continue;

        // Format: <codepoint> # <comment>
        auto commentPos = trimmed.find('#');
        auto cpPart =
            trim(commentPos != std::string::npos ? std::string_view(trimmed).substr(0, commentPos) : std::string_view(trimmed));
        if (cpPart.empty())
            continue;

        if (auto dotdot = cpPart.find(".."); dotdot != std::string_view::npos)
        {
            auto start = parseCodepoint(cpPart.substr(0, dotdot));
            auto end = parseCodepoint(cpPart.substr(dotdot + 2));
            for (auto cp = start; cp <= end; ++cp)
                _compositionExclusions.insert(cp);
        }
        else
        {
            _compositionExclusions.insert(parseCodepoint(cpPart));
        }
    }
}

// ---- Derived Normalization Properties ----

void UcdParser::loadDerivedNormalizationProps()
{
    auto f = openFile(_ucdDir + "/DerivedNormalizationProps.txt");
    std::string line;
    while (std::getline(f, line))
    {
        auto trimmed = std::string(trim(line));
        if (trimmed.empty() || trimmed[0] == '#')
            continue;

        // Format: <codepoint(s)> ; <property> ; <value>? # <comment>
        std::vector<std::string_view> parts;
        std::string_view sv(trimmed);
        size_t pos = 0;
        for (size_t i = 0; i <= sv.size(); ++i)
        {
            if (i == sv.size() || sv[i] == ';')
            {
                parts.push_back(sv.substr(pos, i - pos));
                pos = i + 1;
            }
        }
        if (parts.size() < 2)
            continue;

        auto cpPart = trim(parts[0]);
        auto prop = std::string(trim(parts[1]));

        // Strip inline comments (everything from '#' onward) from the value field
        auto rawValue = parts.size() > 2 ? trim(parts[2]) : std::string_view {};
        if (auto hashPos = rawValue.find('#'); hashPos != std::string_view::npos)
            rawValue = trim(rawValue.substr(0, hashPos));
        auto value = std::string(rawValue);

        // Parse codepoint range
        std::vector<char32_t> codepoints;
        if (auto dotdot = cpPart.find(".."); dotdot != std::string_view::npos)
        {
            auto start = parseCodepoint(cpPart.substr(0, dotdot));
            auto end = parseCodepoint(cpPart.substr(dotdot + 2));
            for (auto cp = start; cp <= end; ++cp)
                codepoints.push_back(cp);
        }
        else
        {
            codepoints.push_back(parseCodepoint(cpPart));
        }

        for (auto cp: codepoints)
        {
            if (prop == "NFC_QC")
            {
                if (value == "N")
                    _nfcQcNo.insert(cp);
                else if (value == "M")
                    _nfcQcMaybe.insert(cp);
            }
            else if (prop == "NFKC_QC")
            {
                if (value == "N")
                    _nfkcQcNo.insert(cp);
                else if (value == "M")
                    _nfkcQcMaybe.insert(cp);
            }
            else if (prop == "NFD_QC")
            {
                if (value == "N")
                    _nfdQcNo.insert(cp);
            }
            else if (prop == "NFKD_QC")
            {
                if (value == "N")
                    _nfkdQcNo.insert(cp);
            }
            else if (prop == "Full_Composition_Exclusion")
            {
                _compositionExclusions.insert(cp);
            }
        }
    }
}

// ---- Derived Age ----

void UcdParser::loadDerivedAge()
{
    _ageRanges = loadGenericProperties(_ucdDir + "/DerivedAge.txt");
}

// ---- Derived Names ----

void UcdParser::loadDerivedNames()
{
    auto f = openFile(_ucdDir + "/extracted/DerivedName.txt");
    std::string line;
    while (std::getline(f, line))
    {
        auto parsed = parseStandardLine(line);
        if (!parsed)
            continue;
        // Property field is the full name (may contain spaces, so use the entire field)
        auto name = std::string(trim(parsed->property));
        for (auto cp = parsed->first; cp <= parsed->last; ++cp)
            _derivedNames.emplace_back(cp, name);
    }
    std::sort(_derivedNames.begin(), _derivedNames.end(), [](auto const& a, auto const& b) { return a.first < b.first; });
}

} // namespace tablegen
