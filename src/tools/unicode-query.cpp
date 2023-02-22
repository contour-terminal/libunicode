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

#include <libunicode/codepoint_properties_loader.h>
#include <libunicode/convert.h>
#include <libunicode/grapheme_segmenter.h>
#include <libunicode/ucd.h>
#include <libunicode/ucd_enums.h>
#include <libunicode/ucd_ostream.h>
#include <libunicode/utf8_grapheme_segmenter.h>

#include <cassert>
#include <charconv>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

using namespace std;

namespace
{

std::string quotedAndEscaped(std::string const& text)
{
    auto result = stringstream {};
    result << '"';
    for (char const ch: text)
    {
        if (std::isprint(ch) && ch != '"')
            result << ch;
        else
            result << "\\x" << setw(2) << std::hex << (unsigned(ch) & 0xFF);
    }
    result << "\"";
    return result.str();
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

string prettyAge(unicode::Age age)
{
    // clang-format off
    string str = [=]() { auto s = ostringstream(); s << age; return s.str(); }();
    // clang-format on
    assert(str.at(0) == 'V');
    str = str.substr(1);
    replace(str.begin(), str.end(), '_', '.');
    return str;
}

void showCodepointProperties(char32_t codepoint)
{
    auto const properties = unicode::codepoint_properties::get(codepoint);

    // clang-format off
    cout << "Name                        : " << unicode::codepoint_properties::name(codepoint) << '\n';
    cout << "Unicode Version             : " << prettyAge(properties.age) << '\n';
    cout << "Codepoint                   : U+" << hex << uint32_t(codepoint) << '\n';
    cout << "UTF-8                       : " << quotedAndEscaped(unicode::convert_to<char>(codepoint)) << '\n';
    if (properties.general_category != unicode::General_Category::Control)
        cout << "Display                     : " << unicode::convert_to<char>(codepoint) << '\n';
    cout << "Plane                       : " << unicode::plane(codepoint) << '\n';
    cout << "Block                       : " << unicode::block(codepoint) << '\n';
    cout << "Script                      : " << unicode::script(codepoint) << '\n';
    cout << "General Category            : " << properties.general_category << '\n';
    cout << "East Asian Width            : " << properties.east_asian_width << '\n';
    cout << "Character width             : " << unsigned(properties.char_width) << '\n';
    cout << "Emoji Segmentation Category : " << properties.emoji_segmentation_category << '\n';
    cout << "Grapheme Cluster Break      : " << properties.grapheme_cluster_break << '\n';
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
