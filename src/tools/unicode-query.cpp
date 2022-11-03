/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2022 Christian Parpart <christian@parpart.family>
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

#include <unicode/codepoint_properties_loader.h>
#include <unicode/convert.h>
#include <unicode/grapheme_segmenter.h>
#include <unicode/ucd.h>
#include <unicode/ucd_enums.h>
#include <unicode/ucd_fmt.h>
#include <unicode/utf8_grapheme_segmenter.h>

#include <fmt/format.h>

#include <charconv>
#include <iostream>
#include <optional>
#include <string>

using namespace std;

namespace
{

std::string quotedAndEscaped(std::string const& text)
{
    auto result = "\""s;
    for (char const ch: text)
    {
        if (std::isprint(ch) && ch != '"')
            result += ch;
        else
            result += fmt::format("\\x{:02X}", uint8_t(ch));
    }
    result += "\"";
    return result;
}

int printUsage(int exitCode)
{
    cout << "unicode-query [properties] U+XXXX [...]\n";
    return exitCode;
}

optional<char32_t> parseChar(std::string_view text)
{
    if (text.size() >= 3 && text[0] == 'U' && text[1] == '+')
        text.remove_prefix(2);

    auto value = uint32_t {};
    auto const result = std::from_chars(text.data(), text.data() + text.size(), value, 16);
    if (result.ptr != text.data() + text.size())
        return nullopt;
    return { static_cast<char32_t>(value) };
}

vector<char32_t> parseChars(std::string_view text)
{
    if (text.size() >= 3 && text[0] == 'U' && text[1] == '+')
    {
        text.remove_prefix(2);
        if (auto const parsedChar = parseChar(text); parsedChar.has_value())
            return { parsedChar.value() };
        else
            return {}; // error
    }

    auto parsedChars = vector<char32_t> {};

    for (char32_t const ch: unicode::from_utf8(text))
        parsedChars.emplace_back(ch);

    return parsedChars;
}

void showCodepointProperties(char32_t codepoint)
{
    auto const properties = unicode::codepoint_properties::get(codepoint);

    // clang-format off
    cout << fmt::format("Codepoint                   : U+{:X}\n", uint32_t(codepoint));
    cout << fmt::format("UTF-8                       : {}\n", quotedAndEscaped(unicode::convert_to<char>(codepoint)));
    if (properties.general_category != unicode::General_Category::Control)
        cout << fmt::format("Display                     : {}\n", unicode::convert_to<char>(codepoint));
    cout << fmt::format("Plane                       : {}\n", unicode::plane(codepoint));
    cout << fmt::format("Block                       : {}\n", unicode::block(codepoint));
    cout << fmt::format("Script                      : {}\n", unicode::script(codepoint));
    cout << fmt::format("General Category            : {}\n", properties.general_category);
    cout << fmt::format("East Asian Width            : {}\n", properties.east_asian_width);
    cout << fmt::format("Character width             : {}\n", properties.char_width);
    cout << fmt::format("Emoji Segmentation Category : {}\n", properties.emoji_segmentation_category);
    cout << fmt::format("Grapheme Cluster Break      : {}\n", properties.grapheme_cluster_break);
    cout << "\n";
    // clang-format off
}

int showCodepointProperties(int argc, char const* argv[])
{
    int arg = 0;
    while (arg < argc)
    {
        auto const codepoints = parseChars(argv[arg]);
        if (codepoints.empty())
            cerr << "Failed to parse codepoint " << argv[arg] << "\n";
        else
            for (auto const codepoint: codepoints)
                showCodepointProperties(codepoint);
        ++arg;
    }
    return EXIT_SUCCESS;
}

} // namespace

// Example usage:
//
// unicode-query [properties] U+1234 [U+5678 ...]
//
// unicode-query analyze "Text string"
//
//     Analyzes the given input string for common Unicode properties
//     and prints out each segments with the gathered information.
int main(int argc, char const* argv[])
{
    if (argc == 1)
        return printUsage(EXIT_FAILURE);

    int argIndex = 1;
    if (string_view(argv[argIndex]) == "help")
        return printUsage(EXIT_SUCCESS);

    if (string_view(argv[argIndex]) == "properties")
        ++argIndex;

    return showCodepointProperties(argc - argIndex, argv + argIndex);
}
