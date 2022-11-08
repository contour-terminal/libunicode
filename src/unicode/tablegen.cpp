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
#include <unicode/codepoint_properties.h>
#include <unicode/codepoint_properties_loader.h>
#include <unicode/support/scoped_timer.h>
#include <unicode/ucd_fmt.h>

#include <fmt/format.h>

#include <iostream>
#include <string>

using namespace std::string_literals;

namespace
{

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

    header << fmt::format("extern std::array<{}, {}> const {};\n", elementTypeName, table.size(), name);
    implementation << fmt::format("std::array<{}, {}> const {} {{", elementTypeName, table.size(), name);
    for (size_t i = 0; i < table.size(); ++i)
    {
        if (i % ColumnCount == 0)
            implementation << "\n    ";

        if (commentOnBlock && i % unicode::codepoint_properties::tables_view::block_size == 0)
            implementation << fmt::format("// block number: {}\n    ",
                                          i / unicode::codepoint_properties::tables_view::block_size);

        implementation << fmt::format("{:>4},", table[i]);
    }
    implementation << "\n";
    implementation << fmt::format("}};\n\n");
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
                       << (!properties.flags ? "0"s : fmt::format("0b{:08b}", properties.flags)) << ", "
                       << fmt::format("Script::{}, ", properties.script)
                       << fmt::format("Grapheme_Cluster_Break::{}, ", properties.grapheme_cluster_break)
                       << fmt::format("East_Asian_Width::{}, ", properties.east_asian_width)
                       << fmt::format("General_Category::{}, ", properties.general_category)
                       << fmt::format("EmojiSegmentationCategory::{}, ", properties.emoji_segmentation_category)
                       << fmt::format("Age::{} ", properties.age)
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
        implementation << fmt::format("    \"{}\"sv,\n", propertiesTable[i]);
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

    auto const disclaimer = fmt::format("// This file was auto-generated using {}.\n", __FILE__);

    header << disclaimer;
    header << "#pragma once\n";
    header << "\n";
    header << "#include <unicode/codepoint_properties.h>\n";
    header << "\n";
    header << "#include <array>\n";
    header << "#include <cstdint>\n";
    header << "\n";
    header << "namespace " << namespaceName << "\n";
    header << "{\n\n";

    implementation << disclaimer;
    implementation << "#include <unicode/codepoint_properties.h>\n";
    implementation << "#include <unicode/codepoint_properties_data.h>\n";
    implementation << "#include <unicode/emoji_segmenter.h>\n";
    implementation << "#include <unicode/ucd_enums.h>\n";
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
    namesFile << "#include <unicode/codepoint_properties_data.h>\n";
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

char const* consumeParamterOrDefault(int& i, int argc, char const* argv[], char const* defaultValue) noexcept
{
    if (argc > i)
        return argv[i++];
    else
        return defaultValue;
}

} // namespace

// Usage: unicode_tablgen UCD_directory CPP_OUTPUTFILE NAMESPACE
int main(int argc, char const* argv[])
{
    // clang-format off
    int i = 1;
    auto const ucdDataDirectory = consumeParamterOrDefault(i, argc, argv, "_ucd/ucd-15.0.0");
    auto const cxxHeaderFileName = consumeParamterOrDefault(i, argc, argv, "codepoint_properties_data.h");
    auto const cxxImplementationFileName = consumeParamterOrDefault(i, argc, argv, "codepoint_properties_data.cpp");
    auto const cxxNamesFileName = consumeParamterOrDefault(i, argc, argv, "codepoint_names_data.cpp");
    auto const namespaceName = consumeParamterOrDefault(i, argc, argv, "unicode::precompiled");
    // clang-format on

    auto headerFile = std::ofstream(cxxHeaderFileName);
    auto implementationFile = std::ofstream(cxxImplementationFileName);
    auto namesFile = std::ofstream(cxxNamesFileName);
    auto const [props, names] = unicode::load_from_directory(ucdDataDirectory, &std::cout);

    write_cxx_tables(props, names, headerFile, implementationFile, namesFile, namespaceName);

    return EXIT_SUCCESS;
}
