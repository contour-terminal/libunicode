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
#include <libunicode/codepoint_properties.h>
#include <libunicode/codepoint_properties_loader.h>
#include <libunicode/scoped_timer.h>
#include <libunicode/ucd_ostream.h>

#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <string>

using namespace std::string_literals;

namespace
{

struct tablegen_configuration
{
    std::string ucdDataDirectory;
    std::string namespaceName;
    std::ofstream cxxHeaderFile;
    std::ofstream cxxImplementationFile;
    std::ofstream cxxNamesFile;
};

std::string binstr(uint32_t n)
{
    std::string ss;
    ss += "0b";
    int const highestOrderBit = n <= 0xFF ? 7 : n <= 0xFFFF ? 15 : 31;
    for (int i = highestOrderBit; i >= 0; --i)
        ss += ((n & (1u << i)) != 0) ? '1' : '0';
    return ss;
}

template <typename T>
std::string_view minimum_uint_t(std::vector<T> const& values)
{
    auto const v = std::max_element(values.begin(), values.end());
    if (*v <= 0xFF)
        return "uint8_t";
    if (*v <= 0xFFFF)
        return "uint16_t";
    if (*v <= 0xFFFFFFFF)
        return "uint32_t";
    return "uint64_t";
}

template <typename T>
void write_cxx_table(std::ostream& header,
                     std::ostream& implementation,
                     std::vector<T> const& table,
                     std::string_view name,
                     bool commentOnBlock)
{
    auto constexpr ColumnCount = 16;

    auto const elementTypeName = minimum_uint_t(table);

    header << "extern std::array<" << elementTypeName << ", " << table.size() << "> const " << name << ";\n";
    implementation << "std::array<" << elementTypeName << ", " << table.size() << "> const " << name << " {";
    for (size_t i = 0; i < table.size(); ++i)
    {
        if (i % ColumnCount == 0)
            implementation << "\n    ";

        if (commentOnBlock && i % unicode::codepoint_properties::tables_view::block_size == 0)
            implementation << "// block number: "
                           << (i / unicode::codepoint_properties::tables_view::block_size) << "\n    ";

        implementation << std::right << std::setw(4) << unsigned(table[i]) << ',';
    }
    implementation << "\n};\n\n";
}

void write_cxx_properties_table(std::ostream& header,
                                std::ostream& implementation,
                                std::vector<unicode::codepoint_properties> const& propertiesTable,
                                std::string_view tableName)
{
    using namespace unicode;
    header << "extern std::array<codepoint_properties, " << propertiesTable.size() << "> const " << tableName
           << ";\n";
    implementation << "std::array<codepoint_properties, " << propertiesTable.size() << "> const " << tableName
                   << "{{\n";
    for (size_t i = 0; i < propertiesTable.size(); ++i)
    {
        // clang-format off
        auto const& properties = propertiesTable[i];
        implementation << "    {"
                       << static_cast<unsigned>(properties.char_width) << ", "
                       << (!properties.flags ? "0"s : binstr(properties.flags)) << ", "
                       << "Script::" << properties.script << ", "
                       << "Grapheme_Cluster_Break::" << properties.grapheme_cluster_break << ", "
                       << "East_Asian_Width::" << properties.east_asian_width << ", "
                       << "General_Category::" << properties.general_category << ", "
                       << "EmojiSegmentationCategory::" << properties.emoji_segmentation_category << ", "
                       << "Age::" << properties.age
                       << "},\n";
        // clang-format on
    }
    implementation << "}};\n\n";
}

void write_cxx_properties_table(std::ostream& header,
                                std::ostream& implementation,
                                std::vector<std::string> const& propertiesTable,
                                std::string_view tableName)
{
    using namespace unicode;
    header << "extern std::array<std::string_view, " << propertiesTable.size() << "> const " << tableName
           << ";\n";
    implementation << "std::array<std::string_view, " << propertiesTable.size() << "> const " << tableName
                   << "{{\n";
    for (size_t i = 0; i < propertiesTable.size(); ++i)
        implementation << "    \"" << propertiesTable[i] << "\"sv,\n";
    implementation << "}};\n\n";
}

void write_cxx_tables(unicode::codepoint_properties_table const& tables,
                      unicode::codepoint_names_table const& namesTables,
                      std::ostream& header,
                      std::ostream& implementation,
                      std::ostream& namesFile,
                      std::string_view namespaceName)
{
    auto const _ = support::scoped_timer(&std::cout, "Writing C++ table files");

    auto const disclaimer = "// This file was auto-generated using " __FILE__ ".\n";

    header << disclaimer;
    header << "#pragma once\n";
    header << "\n";
    header << "#include <libunicode/codepoint_properties.h>\n";
    header << "\n";
    header << "#include <array>\n";
    header << "#include <cstdint>\n";
    header << "\n";
    header << "namespace " << namespaceName << "\n";
    header << "{\n\n";

    implementation << disclaimer;
    implementation << "#include <libunicode/codepoint_properties.h>\n";
    implementation << "#include <libunicode/codepoint_properties_data.h>\n";
    implementation << "#include <libunicode/emoji_segmenter.h>\n";
    implementation << "#include <libunicode/ucd_enums.h>\n";
    implementation << "\n";
    implementation << "#include <array>\n";
    implementation << "#include <cstdint>\n";
    implementation << "#include <string_view>\n";
    implementation << "\n";
    implementation << "using namespace unicode;\n";
    implementation << "\n";
    implementation << "namespace " << namespaceName << "\n";
    implementation << "{\n\n";
    write_cxx_table(header, implementation, tables.stage1, "stage1", false);
    write_cxx_table(header, implementation, tables.stage2, "stage2", true);
    write_cxx_properties_table(header, implementation, tables.stage3, "properties");
    implementation << "} // end namespace " << namespaceName << "\n";

    namesFile << disclaimer;
    namesFile << "#include <libunicode/codepoint_properties_data.h>\n";
    namesFile << "\n";
    namesFile << "#include <array>\n";
    namesFile << "#include <string_view>\n";
    namesFile << "#include <cstdint>\n";
    namesFile << "\n";
    namesFile << "using namespace unicode;\n";
    namesFile << "using namespace std::string_view_literals;\n";
    namesFile << "\n";
    namesFile << "namespace " << namespaceName << "\n";
    namesFile << "{\n\n";
    write_cxx_table(header, namesFile, namesTables.stage1, "names_stage1", false);
    write_cxx_table(header, namesFile, namesTables.stage2, "names_stage2", true);
    write_cxx_properties_table(header, namesFile, namesTables.stage3, "names_stage3");
    namesFile << "} // end namespace " << namespaceName << "\n";

    header << "\n} // end namespace " << namespaceName << "\n";
}

} // namespace

int main(int argc, char const* argv[])
{
    if (argc != 5)
    {
        std::cerr << "Usage: " << argv[0]         // should be: libunicode_tablegen
                  << " <ucd-data-directory>"      // e.g. _ucd/ucd-15.0.0
                  << " <cxx-header-file>"         // e.g. codepoint_properties_data.h
                  << " <cxx-implementation-file>" // e.g. codepoint_properties_data.cpp
                  << " <cxx-names-file>"          // e.g. codepoint_names_data.cpp
                  << "\n";
        return EXIT_FAILURE;
    }

    auto config = tablegen_configuration {
        .ucdDataDirectory = argv[1],
        .namespaceName = "unicode::precompiled",
        .cxxHeaderFile = std::ofstream(argv[2]),
        .cxxImplementationFile = std::ofstream(argv[3]),
        .cxxNamesFile = std::ofstream(argv[4]),
    };

    auto const [props, names] = unicode::load_from_directory(config.ucdDataDirectory, &std::clog);

    write_cxx_tables(props,
                     names,
                     config.cxxHeaderFile,
                     config.cxxImplementationFile,
                     config.cxxNamesFile,
                     config.namespaceName);

    return EXIT_SUCCESS;
}
