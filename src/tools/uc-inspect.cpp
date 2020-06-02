#include <unicode/width.h>
#include <unicode/ucd.h>
#include <unicode/ucd_ostream.h>
#include <unicode/utf8.h>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>

using namespace std;

// {{{ escape(...)
namespace {
    inline std::string escape(uint8_t ch)
    {
        switch (ch)
        {
            case '\\':
                return "\\\\";
            case 0x1B:
                return "\\033";
            case '\t':
                return "\\t";
            case '\r':
                return "\\r";
            case '\n':
                return "\\n";
            case '"':
                return "\\\"";
            default:
                if (ch <= 0xFF && std::isprint(static_cast<char>(ch)))
                    return fmt::format("{}", static_cast<char>(ch));
                else
                    return fmt::format("\\x{:02X}", static_cast<uint8_t>(ch) & 0xFF);
        }
    }

    template <typename T>
    inline std::string escape(T begin, T end)
    {
        return std::accumulate(begin, end, std::string{}, [](auto const& a, auto ch) { return a + escape(ch); });
        // auto result = std::string{};
        // for (T cur = begin; cur != end; ++cur)
        //     result += *cur;
        // return result;
    }

    inline std::string escape(std::string const& s)
    {
        return escape(begin(s), end(s));
    }

}
// }}}

bool isEmoji(char32_t ch)
{
    return unicode::emoji(ch) && !unicode::emoji_component(ch);
}

// TODO
// void grapheme_clusters(istream& _in)
// {
//     while (_in.good())
//     {
//     }
// }

void codepoints(istream& _in)
{
    size_t lastOffset = 0;
    size_t totalOffset = 0;
    unicode::utf8_decoder_state utf8_state{};

    while (_in.good())
    {
        uint8_t ch;
        _in.read((char*) &ch, sizeof(ch));
        totalOffset++;

        auto const convertResult = from_utf8(utf8_state, ch);
        if (holds_alternative<unicode::Success>(convertResult))
        {
            char32_t const wc = get<unicode::Success>(convertResult).value;
            int const width = unicode::width(wc);
            if (width >= 0)
            {
                string const u8 = escape(unicode::to_utf8(wc));

                cout << fmt::format("{:>3}: U+{:08X} [{}] [{:<10}] width:{} UTF8:{}\n",
                                    lastOffset,
                                    static_cast<uint32_t>(wc),
                                    isEmoji(wc) ? "EMOJI" : "TEXT ",
                                    unicode::script(wc),
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
}

int main([[maybe_unused]] int argc, char const* argv[])
{
    // TODO: hb-inspect --codepoints FILE           Inspects source by UTF-32 codepoints
    // TODO: hb-inspect --grapheme-clusters FILE    Inspects source by grapheme cluster
    // TODO: hb-inspect --script-clusters FILE      Inspects source by script cluster

    auto in = ifstream(argv[1], ios::binary);

    codepoints(in);

    return EXIT_SUCCESS;
}
