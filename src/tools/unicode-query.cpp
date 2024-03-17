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
#include <libunicode/run_segmenter.h>
#include <libunicode/ucd.h>
#include <libunicode/ucd_enums.h>
#include <libunicode/ucd_fmt.h>
#include <libunicode/ucd_ostream.h>
#include <libunicode/utf8_grapheme_segmenter.h>

#include <cassert>
#include <charconv>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#if !defined(_WIN32)
    #include <unistd.h>
#endif

using namespace std;

namespace
{

std::string escapeControlCodes(std::string const& text)
{
    auto result = stringstream {};
    for (char const ch: text)
    {
        if (ch < 0x20)
            result << "\\x" << setw(2) << std::hex << (unsigned(ch) & 0xFF);
        else
            result << ch;
    }
    return result.str();
}

std::string escaped(std::string const& text)
{
    auto result = stringstream {};
    for (char const ch: text)
    {
        if (std::isprint(ch) && ch != '"')
            result << ch;
        else
            result << "\\x" << setw(2) << std::hex << (unsigned(ch) & 0xFF);
    }
    return result.str();
}

std::string quotedAndEscaped(std::string const& text)
{
    return '"' + escaped(text) + '"';
}

int printUsage(int exitCode)
{
    cout << "unicode-query [properties] U+XXXX [...]\n"
         << "              gc [-e] [--] \"Text string\"\n"
         << "              runs [-e] [--] \"Text string\"\n";
    return exitCode;
}

std::string_view seq(std::string_view const& text)
{
    static const bool isTTY = []() {
#if !defined(_WIN32)
        auto const isPTY = isatty(STDOUT_FILENO);
        return isPTY;
#else
        return false;
#endif
    }();
    if (isTTY)
        return text;
    else
        return {};
}

// {{{ properties
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
    // clang-format on
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
// }}}

// {{{ grapheme clusters
int showGraphemeClusters(int argc, char const* argv[])
{
    int i = 0;
    bool escapeText = false;
    for (; i < argc; ++i)
    {
        auto const arg = string_view(argv[i]);
        if (arg == "-e")
            escapeText = true;
        else if (arg == "--")
        {
            ++i;
            break;
        }
        else if (arg.starts_with('-'))
            return printUsage(EXIT_FAILURE);
        else
            break;
    }
    for (; i < argc; ++i)
    {
        auto const text = string_view(argv[i]);
        auto const gcs = unicode::utf8_grapheme_segmenter(text);
        for (auto const& gc: gcs)
        {
            auto const text32 = std::u32string_view(gc);
            auto const text8 = unicode::convert_to<char>(text32);
            std::cout << (escapeText ? escaped(text8) : escapeControlCodes(text8)) << "\n";
        }
    }
    return EXIT_SUCCESS;
}
// }}}

// {{{ runs
int showRuns(istream& in, bool escapeRunText)
{
    string bytes((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    u32string const codepoints = unicode::convert_to<char32_t>(string_view(bytes));

    unicode::run_segmenter rs(codepoints);
    unicode::run_segmenter::range run;

    while (rs.consume(unicode::out(run)))
    {
        auto const script = get<unicode::Script>(run.properties);
        auto const presentationStyle = get<unicode::PresentationStyle>(run.properties);

        auto const text32 = u32string_view(codepoints.data() + run.start, run.end - run.start);
        auto const text8 = unicode::convert_to<char>(text32);
        auto const textEscaped = escapeRunText ? escaped(text8) : escapeControlCodes(text8);

        cout << run.start << "-" << run.end - 1 << " (" << run.end - run.start << "): " << script << " " << presentationStyle
             << "\n"
             << '"' << seq("\033[32m") << textEscaped << seq("\033[m") << "\"\n\n";
    }

    return EXIT_SUCCESS;
}

int showRuns(int argc, char const* argv[])
{
    // [-e]
    int i = 0;
    bool escaped = false;
    for (; i < argc; ++i)
    {
        auto const arg = string_view(argv[i]);
        if (arg == "-e")
            escaped = true;
        else if (arg == "--")
        {
            ++i;
            break;
        }
        else if (arg.starts_with('-'))
            return printUsage(EXIT_FAILURE);
        else
            break;
    }

    for (; i < argc; ++i)
    {
        auto in = std::istringstream(argv[i]);
        showRuns(in, escaped);
    }

    return EXIT_SUCCESS;
}
// }}}
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

    if (string_view(argv[argIndex]) == "runs")
    {
        ++argIndex;
        return showRuns(argc - argIndex, argv + argIndex);
    }

    if (string_view(argv[argIndex]) == "gc")
    {
        ++argIndex;
        return showGraphemeClusters(argc - argIndex, argv + argIndex);
    }

    if (string_view(argv[argIndex]) == "properties")
        ++argIndex;

    return showCodepointProperties(argc - argIndex, argv + argIndex);
}
