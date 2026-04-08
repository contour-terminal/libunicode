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

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace tablegen
{

/// A single codepoint or codepoint range with an associated property value and comment.
struct PropertyRange
{
    char32_t first {};
    char32_t last {};
    std::string property;
    std::string comment;
};

/// A codepoint range with multiple property values (e.g., script extensions).
struct MultiPropertyRange
{
    char32_t first {};
    char32_t last {};
    std::vector<std::string> properties;
    std::string comment;
};

/// A Unicode block range with a human-readable title.
struct BlockRange
{
    char32_t first {};
    char32_t last {};
    std::string title;
};

/// A simple 1:1 case mapping.
struct SimpleCaseMapping
{
    char32_t source {};
    char32_t target {};
};

/// A full case mapping (1:many, max 3).
struct FullCaseMapping
{
    char32_t source {};
    std::vector<char32_t> targets;
};

/// A Unicode decomposition mapping.
struct Decomposition
{
    char32_t source {};
    std::vector<char32_t> targets;
    std::string type; // "canonical" or compatibility type
};

/// Plane definition.
struct PlaneInfo
{
    int number {};
    char32_t start {};
    char32_t end {};
    std::string shortName;
    std::string name;
};

/// Self-contained UCD parser that uses only strings/integers internally.
/// Has zero dependency on generated code.
class UcdParser
{
  public:
    explicit UcdParser(std::string ucdDirectory);

    /// Parse all UCD files.
    void parseAll();

    // ---- Parsed data accessors ----

    /// Property value aliases: property_name -> { abbreviation -> full_name }
    [[nodiscard]] auto const& propertyValueAliases() const noexcept { return _propertyValues; }

    /// General category ranges, sorted by start codepoint.
    [[nodiscard]] auto const& generalCategories() const noexcept { return _generalCategory; }

    /// General category ranges grouped by category name.
    [[nodiscard]] auto const& generalCategoryMap() const noexcept { return _generalCategoryMap; }

    /// Core properties grouped by property name.
    [[nodiscard]] auto const& coreProperties() const noexcept { return _coreProperties; }

    /// Indic Conjunct Break (InCB) properties grouped by value (Consonant, Linker, Extend).
    [[nodiscard]] auto const& indicConjunctBreak() const noexcept { return _indicConjunctBreak; }

    /// Script property ranges, sorted by start codepoint.
    [[nodiscard]] auto const& scripts() const noexcept { return _scripts; }

    /// Script extensions, sorted by start codepoint.
    [[nodiscard]] auto const& scriptExtensions() const noexcept { return _scriptExtensions; }

    /// Unicode blocks.
    [[nodiscard]] auto const& blocks() const noexcept { return _blocks; }

    /// Grapheme cluster break properties grouped by property name.
    [[nodiscard]] auto const& graphemeBreakProps() const noexcept { return _graphemeBreakProps; }

    /// East Asian Width ranges, sorted by start codepoint.
    [[nodiscard]] auto const& eastAsianWidths() const noexcept { return _eastAsianWidths; }

    /// Emoji properties grouped by property name.
    [[nodiscard]] auto const& emojiProps() const noexcept { return _emojiProps; }

    /// Bidi mirrored intervals (compressed).
    [[nodiscard]] auto const& bidiMirroredIntervals() const noexcept { return _bidiMirroredIntervals; }

    /// Bidi mirroring glyph (source, target) pairs.
    [[nodiscard]] auto const& bidiMirroringGlyphPairs() const noexcept { return _bidiMirroringGlyphPairs; }

    /// Plane definitions.
    [[nodiscard]] auto const& planes() const noexcept { return _planes; }

    // ---- Case / normalization data ----

    [[nodiscard]] auto const& simpleUppercase() const noexcept { return _simpleUppercase; }
    [[nodiscard]] auto const& simpleLowercase() const noexcept { return _simpleLowercase; }
    [[nodiscard]] auto const& simpleTitlecase() const noexcept { return _simpleTitlecase; }
    [[nodiscard]] auto const& simpleCasefold() const noexcept { return _simpleCasefold; }
    [[nodiscard]] auto const& fullUppercase() const noexcept { return _fullUppercase; }
    [[nodiscard]] auto const& fullLowercase() const noexcept { return _fullLowercase; }
    [[nodiscard]] auto const& fullTitlecase() const noexcept { return _fullTitlecase; }
    [[nodiscard]] auto const& fullCasefold() const noexcept { return _fullCasefold; }
    [[nodiscard]] auto const& ccc() const noexcept { return _ccc; }
    [[nodiscard]] auto const& decompositions() const noexcept { return _decompositions; }
    [[nodiscard]] auto const& compositionExclusions() const noexcept { return _compositionExclusions; }
    [[nodiscard]] auto const& nfcQcNo() const noexcept { return _nfcQcNo; }
    [[nodiscard]] auto const& nfcQcMaybe() const noexcept { return _nfcQcMaybe; }
    [[nodiscard]] auto const& nfkcQcNo() const noexcept { return _nfkcQcNo; }
    [[nodiscard]] auto const& nfkcQcMaybe() const noexcept { return _nfkcQcMaybe; }
    [[nodiscard]] auto const& nfdQcNo() const noexcept { return _nfdQcNo; }
    [[nodiscard]] auto const& nfkdQcNo() const noexcept { return _nfkdQcNo; }

    // ---- For multistage table generation ----

    /// DerivedAge ranges.
    [[nodiscard]] auto const& ageRanges() const noexcept { return _ageRanges; }

    /// DerivedCoreProperties ranges (for grapheme_extend flag).
    [[nodiscard]] auto const& derivedCorePropertiesRanges() const noexcept { return _derivedCorePropertiesRanges; }

    /// DerivedName entries (codepoint -> name).
    [[nodiscard]] auto const& derivedNames() const noexcept { return _derivedNames; }

  private:
    void loadPropertyValueAliases();
    void loadGeneralCategory();
    void loadCoreProperties();
    void loadScripts();
    void loadScriptExtensions();
    void loadBlocks();
    void loadGraphemeBreakProps();
    void loadEastAsianWidths();
    void loadEmojiProps();
    void loadBidiMirrored();
    void loadBidiMirroringGlyph();

    // Case / normalization
    void loadUnicodeData();
    void loadCaseFolding();
    void loadSpecialCasing();
    void loadCompositionExclusions();
    void loadDerivedNormalizationProps();

    // For multistage tables
    void loadDerivedAge();
    void loadDerivedNames();

    // Helper: parse a standard UCD file into PropertyRange list
    [[nodiscard]] std::vector<PropertyRange> loadGenericProperties(std::string const& filename) const;

    // Helper: parse a UCD file grouped by section header
    [[nodiscard]] std::map<std::string, std::vector<PropertyRange>> loadGroupedProperties(std::string const& filename,
                                                                                          std::string const& headerKey) const;

    std::string _ucdDir;

    // Planes
    std::vector<PlaneInfo> _planes;

    // Property value aliases
    std::map<std::string, std::map<std::string, std::string>> _propertyValues;

    // General category
    std::vector<PropertyRange> _generalCategory;
    std::map<std::string, std::vector<PropertyRange>> _generalCategoryMap;

    // Core properties
    std::map<std::string, std::vector<PropertyRange>> _coreProperties;

    // Indic Conjunct Break (InCB) properties
    std::map<std::string, std::vector<PropertyRange>> _indicConjunctBreak;

    // Scripts
    std::vector<PropertyRange> _scripts;

    // Script extensions
    std::vector<MultiPropertyRange> _scriptExtensions;

    // Blocks
    std::vector<BlockRange> _blocks;

    // Grapheme break props
    std::map<std::string, std::vector<PropertyRange>> _graphemeBreakProps;

    // East Asian Width
    std::vector<PropertyRange> _eastAsianWidths;

    // Emoji
    std::map<std::string, std::vector<PropertyRange>> _emojiProps;

    // Bidi mirroring
    std::vector<std::pair<char32_t, char32_t>> _bidiMirroredIntervals;
    std::vector<std::pair<char32_t, char32_t>> _bidiMirroringGlyphPairs;

    // Case mappings
    std::map<char32_t, char32_t> _simpleUppercase;
    std::map<char32_t, char32_t> _simpleLowercase;
    std::map<char32_t, char32_t> _simpleTitlecase;
    std::map<char32_t, char32_t> _simpleCasefold;
    std::map<char32_t, std::vector<char32_t>> _fullUppercase;
    std::map<char32_t, std::vector<char32_t>> _fullLowercase;
    std::map<char32_t, std::vector<char32_t>> _fullTitlecase;
    std::map<char32_t, std::vector<char32_t>> _fullCasefold;

    // Normalization
    std::map<char32_t, int> _ccc;
    std::map<char32_t, Decomposition> _decompositions;
    std::set<char32_t> _compositionExclusions;
    std::set<char32_t> _nfcQcNo;
    std::set<char32_t> _nfcQcMaybe;
    std::set<char32_t> _nfkcQcNo;
    std::set<char32_t> _nfkcQcMaybe;
    std::set<char32_t> _nfdQcNo;
    std::set<char32_t> _nfkdQcNo;

    // For multistage tables
    std::vector<PropertyRange> _ageRanges;
    std::vector<PropertyRange> _derivedCorePropertiesRanges;
    std::vector<std::pair<char32_t, std::string>> _derivedNames;
};

} // namespace tablegen
