/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2021 Christian Parpart <christian@parpart.family>
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

#include <unicode/convert.h>
#include <unicode/grapheme_segmenter.h>
#include <unicode/run_segmenter.h>
#include <unicode/ucd.h>
#include <unicode/ucd_ostream.h>
#include <unicode/utf8.h>
#include <unicode/width.h>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>

using namespace std::string_literals;
using namespace std::string_view_literals;

using std::array;
using std::basic_string;
using std::basic_string_view;
using std::cerr;
using std::cin;
using std::cout;
using std::get;
using std::holds_alternative;
using std::ifstream;
using std::ios;
using std::istream;
using std::istreambuf_iterator;
using std::make_unique;
using std::optional;
using std::pair;
using std::string;
using std::string_view;
using std::u32string;
using std::u32string_view;
using std::unique_ptr;
using std::variant;

// {{{ escape(...)
namespace
{
bool isEmoji(char32_t ch)
{
    return unicode::emoji(ch) && !unicode::emoji_component(ch);
}

template <typename T>
basic_string<T> replaceAll(basic_string_view<T> what, basic_string_view<T> with, basic_string_view<T> text)
{
    basic_string<T> s;
    size_t a = 0;
    size_t b = text.find(what);
    while (b != basic_string_view<T>::npos)
    {
        s += text.substr(a, b - a);
        s += with;
        a = b + what.size();
        b = text.find(what, a + 1);
    }
    s += text.substr(a);
    return s;
}

inline string escape(uint8_t ch)
{
    switch (ch)
    {
    case '\\': return "\\\\";
    case 0x1B: return "\\033";
    case '\t': return "\\t";
    case '\r': return "\\r";
    case '\n': return "\\n";
    case '"': return "\\\"";
    default:
        if (std::isprint(static_cast<char>(ch)))
            return fmt::format("{}", static_cast<char>(ch));
        else
            return fmt::format("\\x{:02X}", static_cast<uint8_t>(ch) & 0xFF);
    }
}

template <typename T>
inline string escape(T begin, T end)
{
    return std::accumulate(
        begin, end, string {}, [](auto const& a, auto ch) { return a + escape(static_cast<uint8_t>(ch)); });
    // auto result = string{};
    // for (T cur = begin; cur != end; ++cur)
    //     result += *cur;
    // return result;
}

inline string escape(string const& s)
{
    return escape(begin(s), end(s));
}

} // namespace
// }}}

// TODO
// void grapheme_clusters(istream& _in)
// {
//     while (_in.good())
//     {
//     }
// }

void codepoints(istream& in) // {{{
{
    auto lastOffset = 0;
    auto totalOffset = 0;
    auto utf8_state = unicode::utf8_decoder_state {};
    auto last_wc = char32_t {};

    for (;;)
    {
        uint8_t ch {};
        in.read((char*) &ch, sizeof(ch));
        if (!in.good())
            break;

        totalOffset++;

        auto const convertResult = from_utf8(utf8_state, ch);

        if (holds_alternative<unicode::Success>(convertResult))
        {
            char32_t const wc = get<unicode::Success>(convertResult).value;
            int const width = unicode::width(wc);
            bool breakable = !last_wc || unicode::grapheme_segmenter::breakable(last_wc, wc);
            last_wc = wc;
            // TODO: breakable buggy lol what?
            if (width >= 0)
            {
                string const u8 = escape(unicode::to_utf8(wc));

                cout << fmt::format("{:>3}: U+{:08X} [{}] [{:<10}] {} width:{} UTF8:{}\n",
                                    lastOffset,
                                    static_cast<uint32_t>(wc),
                                    isEmoji(wc) ? "EMOJI" : "TEXT ",
                                    unicode::script(wc),
                                    breakable ? "[breakable  ]" : "[unbreakable]",
                                    width,
                                    u8);
            }
            lastOffset = totalOffset;
        }
        else if (holds_alternative<unicode::Invalid>(convertResult))
        {
            lastOffset = totalOffset;
        }
    }
} // }}}

using unicode::convert_to;
using unicode::out;
using unicode::run_segmenter;

string scriptExtensionsString(char32_t codepoint)
{
    array<unicode::Script, 32> scripts {};
    size_t count = unicode::script_extensions(codepoint, scripts.data(), scripts.size());
    std::stringstream sstr;
    for (size_t i = 0; i < count; ++i)
    {
        if (i)
            sstr << ", ";
        sstr << fmt::format("{}", scripts[i]);
    }
    return sstr.str();
}

int scripts(istream& in) // {{{
{
    string bytes((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    u32string const codepoints = convert_to<char32_t>(string_view(bytes));

    unicode::script_segmenter segmenter(codepoints);

    size_t frontPosition {};
    size_t nextPosition {};
    unicode::Script script {};

    cout << "   INDEX     CODEPOINT    TEXT  WIDTH   SCRIPT          SCRIPT EXTS\n";
    while (segmenter.consume(out(nextPosition), out(script)))
    {
        cout << fmt::format("{}-{}: {}\n", frontPosition, nextPosition - 1, script);
        for (size_t i = frontPosition; i < nextPosition; ++i)
        {
            auto const cp = codepoints[i];
            cout << fmt::format("    {:>04}:    U+{:08X}   {}\t∆ {}\t{:<12}\t({})\n",
                                i,
                                unsigned(cp),
                                convert_to<char>(cp),
                                unicode::width(cp),
                                unicode::script(cp),
                                scriptExtensionsString(cp));
        }
        frontPosition = nextPosition;
    }

    return EXIT_SUCCESS;
} // }}}

int runs(istream& in) // {{{
{
    string bytes((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    u32string const codepoints = convert_to<char32_t>(string_view(bytes));

    run_segmenter rs(codepoints);
    run_segmenter::range run;

    while (rs.consume(out(run)))
    {
        auto const script = get<unicode::Script>(run.properties);
        auto const presentationStyle = get<unicode::PresentationStyle>(run.properties);

        cout << fmt::format(
            "{}-{} ({}): {} {}\n", run.start, run.end - 1, run.end - run.start, script, presentationStyle);
        auto const text32 = u32string_view(codepoints.data() + run.start, run.end - run.start);
        auto const text8 = convert_to<char>(text32);
        auto const textEscaped = replaceAll("\033"sv, "\\033"sv, string_view(text8));
        cout << '"' << "\033[32m" << textEscaped << "\033[m"
             << "\"\n\n";
    }

    return EXIT_SUCCESS;
} // }}}

enum class Cmd
{
    Codepoints,
    Runs,
    Scripts,
};

using InputStream = variant<istream*, unique_ptr<istream>>;

auto parseArgs(int argc, char const* argv[]) -> optional<pair<Cmd, InputStream>>
{
    auto constexpr DefaultCmd = Cmd::Codepoints;

    if (argc != 3)
        return { { DefaultCmd, &cin } };

    string_view const arg = argv[1];

    static auto constexpr mappings =
        array<pair<string_view, Cmd>, 4> { pair { "codepoints"sv, Cmd::Codepoints },
                                           pair { "cp"sv, Cmd::Codepoints },
                                           pair { "runs"sv, Cmd::Runs },
                                           pair { "scripts"sv, Cmd::Scripts } };

    for (auto const& mapping: mappings)
        if (mapping.first == arg)
            return { { mapping.second, make_unique<ifstream>(argv[2], ios::binary) } };

    return { { DefaultCmd, make_unique<ifstream>(argv[2], ios::binary) } };
}

int run(int argc, char const* argv[])
{
    // TODO: hb-inspect codepoints FILE           Inspects source by UTF-32 codepoints
    // TODO: hb-inspect grapheme-clusters FILE    Inspects source by grapheme cluster
    // TODO: hb-inspect script-clusters FILE      Inspects source by script cluster

    auto args = parseArgs(argc, argv);
    if (!args.has_value())
    {
        cerr << "Usage error.\n"
             << "Usage:\n"
             << "    uc-inspect codepoints FILE\n"
             << "    uc-inspect runs FILE\n"
             << "    uc-inspect scripts FILE\n";
        return EXIT_FAILURE;
    }

    auto& [cmd, inputStreamVar] = *args;
    istream& in = holds_alternative<istream*>(inputStreamVar) ? *get<istream*>(inputStreamVar)
                                                              : *get<unique_ptr<istream>>(inputStreamVar);

    switch (cmd)
    {
    case Cmd::Codepoints: codepoints(in); break;
    case Cmd::Runs: runs(in); break;
    case Cmd::Scripts: scripts(in); break;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char const* argv[])
{
    try
    {
        return run(argc, argv);
    }
    catch (std::exception const& e)
    {
        std::cerr << fmt::format("Unhandled exception caught ({}). {}\n", typeid(e).name(), e.what());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "Unhandled exception caught.\n";
        return EXIT_FAILURE;
    }
}
