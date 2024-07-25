/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2024 Christian Parpart <christian@parpart.family>
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
#include <libunicode/convert.h>
#include <libunicode/grapheme_line_segmenter.h>
#include <libunicode/utf8.h>

#include <fmt/format.h>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <string_view>
#include <variant>

#if 0 || defined(LIBUNICODE_TRACE)
    #include <format>
    #include <iostream>

    #define TRACE(...) std::cout << std::format(__VA_ARGS__)
#else
    #define TRACE(...) ((void) 0)
#endif

using namespace std::string_view_literals;
using namespace std::string_literals;
using std::pair;
using StopCondition = unicode::StopCondition;

struct expectation
{
    size_t offset;
    size_t size;

    unsigned width;

    auto operator<=>(expectation const&) const = default;
};

// {{{ operator<<(ostream, ...) overrides
namespace std
{

std::ostream& operator<<(std::ostream& os, expectation const& e)
{
    return os << "{ offset: " << e.offset << ", size: " << e.size << ", width: " << e.width << " }";
}

[[maybe_unused]] std::ostream& operator<<(std::ostream& os, std::pair<unicode::StopCondition, unsigned> const& v)
{
    return os << "{" << v.first << ", " << v.second << "}";
}

} // namespace std
// }}}

namespace fmt
{

template <>
struct formatter<expectation>: formatter<std::string_view>
{
    template <typename FormatContext>
    auto format(expectation const& e, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "{{ offset: {}, size: {}, width: {} }}", e.offset, e.size, e.width);
    }
};

template <>
struct formatter<std::pair<unicode::StopCondition, unsigned>>: formatter<std::string_view>
{
    template <typename FormatContext>
    auto format(std::pair<unicode::StopCondition, unsigned> const& v, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "{{{}, {}}}", v.first, v.second);
    }
};

} // namespace fmt

// {{{ helpers
namespace
{

using segmentation_result = unicode::grapheme_segmentation_result;
using unicode::grapheme_line_segmenter;

enum class NumericEscape
{
    Octal,
    Hex
};

std::string escape(uint8_t ch, NumericEscape numericEscape = NumericEscape::Hex)
{
    switch (ch)
    {
        case '\\': return "\\\\";
        case 0x1B: return "\\e";
        case '\t': return "\\t";
        case '\r': return "\\r";
        case '\n': return "\\n";
        case '"': return "\\\"";
        default:
            if (0x20 <= ch && ch < 0x7E)
                return fmt::format("{}", static_cast<char>(ch));
            else if (numericEscape == NumericEscape::Hex)
                return fmt::format("\\x{:02x}", static_cast<uint8_t>(ch) & 0xFF);
            else
                return fmt::format("\\{:03o}", static_cast<uint8_t>(ch) & 0xFF);
    }
}

inline std::string e(std::string_view s, NumericEscape numericEscape = NumericEscape::Hex)
{
    auto result = std::string {};
    for (char c: s)
        result += escape(static_cast<uint8_t>(c), numericEscape);
    return result;
}

struct invalid_sequence
{
    std::string_view value;
    auto operator<=>(invalid_sequence const&) const = default;
};

struct ascii_sequence
{
    std::string_view value;
    auto operator<=>(ascii_sequence const&) const = default;
};

struct complex_unicode_sequence
{
    std::string_view value;
    unsigned width;
    auto operator<=>(complex_unicode_sequence const&) const = default;
};

[[maybe_unused]] std::ostream& operator<<(std::ostream& os, complex_unicode_sequence const& seq)
{
    return os << "{ value: \"" << e(seq.value) << "\", width: " << seq.width << " }";
}

using Record = std::variant<invalid_sequence, ascii_sequence, complex_unicode_sequence>;

} // namespace

namespace fmt
{
template <>
struct formatter<Record>: formatter<std::string_view>
{
    template <typename FormatContext>
    auto format(Record const& r, FormatContext& ctx) const
    {
        if (std::holds_alternative<invalid_sequence>(r))
            return fmt::format_to(ctx.out(), "invalid_sequence {{ value: \"{}\" }}", std::get<invalid_sequence>(r).value);
        else if (std::holds_alternative<ascii_sequence>(r))
            return fmt::format_to(ctx.out(), "ascii_sequence {{ value: \"{}\" }}", std::get<ascii_sequence>(r).value);
        else
            return fmt::format_to(ctx.out(),
                                  "complex_unicode_sequence {{ value: \"{}\", width: {} }}",
                                  std::get<complex_unicode_sequence>(r).value,
                                  std::get<complex_unicode_sequence>(r).width);
    }
};

} // namespace fmt

namespace
{

auto constexpr FamilyEmoji = U"\U0001F468\u200D\U0001F469\u200D\U0001F467\u200D\U0001F466"sv;
auto constexpr SmileyEmoji = U"\U0001F600"sv;
auto constexpr CopyrightSign = U"\u00A9"sv;

template <typename T>
auto u8(T text)
{
    return unicode::convert_to<char>(text);
}

class event_logger final: public unicode::grapheme_segmentation_listener
{
  public:
    static event_logger& get()
    {
        static event_logger instance;
        return instance;
    }

    void on_invalid(std::string_view invalid) noexcept
    {
        std::cout << std::format("[event_logger] on_invalid: {}\n", e(invalid));
        UNSCOPED_INFO(fmt::format("[event_logger] on_invalid: {}\n", e(invalid)));
    }

    void on_ascii(std::string_view text) noexcept
    {
        std::cout << std::format("[event_logger] on_ascii: {}\n", text);
        UNSCOPED_INFO(fmt::format("[event_logger] on_ascii: {}\n", text));
    }

    void on_grapheme_cluster(std::string_view text, unsigned width) noexcept
    {
        std::cout << std::format("[event_logger] on_grapheme_cluster: {} (width: {})\n", text, width);
        UNSCOPED_INFO(fmt::format("[event_logger] on_grapheme_cluster: {} (width: {})\n", text, width));
    }
};

class event_recorder final: public unicode::grapheme_segmentation_listener
{
  public:
    explicit event_recorder(std::string_view text): _text(text) {}

    void reset(std::string_view text) noexcept
    {
        _text = text;
        _records.clear();
    }

    std::string_view text() const noexcept { return _text; }

    void on_invalid(std::string_view sequence) noexcept
    {
        UNSCOPED_INFO(fmt::format("[event_logger] on_invalid: {}\n", e(sequence)));
        _records.emplace_back(invalid_sequence { sequence });
    }

    void on_ascii(std::string_view sequence) noexcept
    {
        UNSCOPED_INFO(fmt::format("[event_logger] on_ascii: {}\n", sequence));
        _records.emplace_back(ascii_sequence { sequence });
    }

    void on_grapheme_cluster(std::string_view cluster, unsigned width) noexcept
    {
        UNSCOPED_INFO(fmt::format("[event_recorder] grapheme cluster {}+{} '{}' (width: {}, u8: {})\n",
                                  std::distance(_text.data(), cluster.data()),
                                  cluster.size(),
                                  cluster,
                                  width,
                                  e(cluster)));
        _records.emplace_back(complex_unicode_sequence { cluster, width });
    }

    std::vector<Record> const& records() const noexcept { return _records; }
    size_t size() const noexcept { return _records.size(); }

    complex_unicode_sequence const& cluster(size_t i) const noexcept
    {
        return std::get<complex_unicode_sequence>(_records.at(i));
    }

    expectation at(size_t i) const noexcept
    {
        auto const& cu = cluster(i);
        return expectation { .offset = (size_t) std::distance(_text.data(), cu.value.data()),
                             .size = cu.value.size(),
                             .width = cu.width };
    }

  private:
    std::string_view _text;
    std::vector<Record> _records;
};

segmentation_result scan_text(std::string_view text, unsigned width)
{
    auto recorder = event_recorder { text };
    auto segmenter = grapheme_line_segmenter { recorder, text };
    UNSCOPED_INFO(fmt::format("Processing {} bytes @{}: \"{}\"\n", text.size(), (void*) text.data(), e(text)));
    auto const main = segmenter.process(width);
    if (main.width == width)
        return main;
    auto const fin = segmenter.flush(width - main.width);
    if (fin.text.empty())
        return main;
    return segmentation_result {
        .text = std::string_view { text.data(), fin.text.data() + fin.text.size() },
        .width = main.width + fin.width,
        .stop_condition = main.stop_condition,
    };
}

} // namespace
// }}}

// {{{ helper method tests for ASCII-only
inline auto process_only_ascii(std::string_view text)
{
    auto const result = unicode::detail::process_only_ascii(text);
    return pair { result.first, static_cast<int>(std::distance(text.data(), result.second)) };
}

TEST_CASE("grapheme_line_segmenter.process_only_ascii")
{
    // clang-format off
    // ensure SIMD-enabled processing stops at control characters and complex Unicode characters at the beginning
    CHECK(process_only_ascii("\rABCD") == pair { StopCondition::UnexpectedInput, 0 });
    CHECK(process_only_ascii("\nABCD") == pair { StopCondition::UnexpectedInput, 0 });
    CHECK(process_only_ascii("\033ABCD") == pair { StopCondition::UnexpectedInput, 0 });
    CHECK(process_only_ascii(u8(CopyrightSign)) == pair { StopCondition::UnexpectedInput, 0 });
    CHECK(process_only_ascii(u8(SmileyEmoji)) == pair { StopCondition::UnexpectedInput, 0 });

    // ensure SIMD-enabled processing stops at control characters
    CHECK(process_only_ascii("1234\033") == pair { StopCondition::UnexpectedInput, 4 });
    CHECK(process_only_ascii("12345678\033") == pair { StopCondition::UnexpectedInput, 8 });
    CHECK(process_only_ascii("0123456789ABCDEF\033") == pair { StopCondition::UnexpectedInput, 16 });
    CHECK(process_only_ascii("0123456789ABCDEF1\033") == pair { StopCondition::UnexpectedInput, 17 });
    CHECK(process_only_ascii("0123456789ABCDEF1" + u8(SmileyEmoji)) == pair { StopCondition::UnexpectedInput, 17 });
    CHECK(process_only_ascii("0123456789ABCDEF0123456789ABCDE\033") == pair { StopCondition::UnexpectedInput, 31 });

    // ensure SIMD-enabled processing stops at complex Unicode
    CHECK(process_only_ascii("0123456789ABCDEF0123456789ABCDE\x80") == pair { StopCondition::UnexpectedInput, 31 });
    CHECK(process_only_ascii("0123456789ABCDEF0123456789ABCDE\x81") == pair { StopCondition::UnexpectedInput, 31 });
    CHECK(process_only_ascii("0123456789ABCDEF0123456789ABCDE\xFF") == pair { StopCondition::UnexpectedInput, 31 });

    // test minimal input
    CHECK(process_only_ascii("") == pair { StopCondition::EndOfInput, 0 });
    CHECK(process_only_ascii("0") == pair { StopCondition::EndOfInput, 1 });

    // test at and around SIMD (SSE2) boundary
    CHECK(process_only_ascii("0123456789ABCDE") == pair { StopCondition::EndOfInput, 15 });
    CHECK(process_only_ascii("0123456789ABCDEF") == pair { StopCondition::EndOfInput, 16 });
    CHECK(process_only_ascii("0123456789ABCDEF1") == pair { StopCondition::EndOfInput, 17 });
    // clang-format on
}
// }}}

// {{{ helper method tests for complex unicode only
TEST_CASE("grapheme_line_segmenter.process_only_complex_unicode.0")
{
    auto const text = ""s;
    auto recorder = event_recorder { text };
    auto state = unicode::detail::unicode_process_state {};
    auto const result = unicode::detail::process_only_complex_unicode(recorder, state, text, 80);
    CHECK(result.end == text.data());
    CHECK(result.totalWidth == 0);
    CHECK(recorder.size() == 0);

    unicode::detail::flush_complex_unicode(recorder, state, 80);
    CHECK(recorder.size() == 0);
}

TEST_CASE("grapheme_line_segmenter.process_only_complex_unicode.1")
{
    // 1 complex grapheme cluster and 1 simple grapheme cluster
    auto const text = u8(SmileyEmoji) + "."s;

    auto recorder = event_recorder { text };
    auto state = unicode::detail::unicode_process_state {};
    auto const result = unicode::detail::process_only_complex_unicode(recorder, state, text, 80);
    CHECK(result.end == text.data() + 4);
    CHECK(result.totalWidth == 2);
    CHECK(recorder.size() == 1);
    CHECK(recorder.at(0) == expectation { .offset = 0, .size = 4, .width = 2 });
}

TEST_CASE("grapheme_line_segmenter.process_only_complex_unicode.2")
{
    // 2 complex grapheme cluster and 1 simple grapheme cluster
    // NB: We append the trailing dot to force the grapheme line segmenter to process the last grapheme cluster.
    auto const text = u8(SmileyEmoji) + u8(SmileyEmoji) + "."s;

    auto recorder = event_recorder { text };
    auto state = unicode::detail::unicode_process_state {};
    auto const result = unicode::detail::process_only_complex_unicode(recorder, state, text, 80);
    CHECK(result.end == text.data() + 8);
    CHECK(result.totalWidth == 4);
    CHECK(recorder.size() == 2);
    CHECK(recorder.at(0) == expectation { .offset = 0, .size = 4, .width = 2 });
    CHECK(recorder.at(1) == expectation { .offset = 4, .size = 4, .width = 2 });

    unicode::detail::flush_complex_unicode(recorder, state, 80);
    CHECK(recorder.size() == 2);
}

TEST_CASE("grapheme_line_segmenter.process_only_complex_unicode.3")
{
    // 3 simple grapheme clusters (and have no bytes next to them)
    auto const text = u8(SmileyEmoji) + u8(SmileyEmoji) + u8(SmileyEmoji);

    auto recorder = event_recorder { text };
    auto state = unicode::detail::unicode_process_state {};
    auto const result = unicode::detail::process_only_complex_unicode(recorder, state, text, 80);
    CHECK(std::distance(text.data(), result.end) == 3 * 4lu);
    CHECK(result.totalWidth == 4);
    CHECK(recorder.size() == 2);
    CHECK(recorder.at(0) == expectation { .offset = 0, .size = 4, .width = 2 });
    CHECK(recorder.at(1) == expectation { .offset = 4, .size = 4, .width = 2 });
    REQUIRE(long(std::distance((char const*) text.data(), state.utf8DecodeNext)) == 12);

    unicode::detail::flush_complex_unicode(recorder, state, 80);
    CHECK(recorder.size() == 3);
    CHECK(recorder.at(2) == expectation { .offset = 8, .size = 4, .width = 2 });
}

TEST_CASE("grapheme_line_segmenter.process_only_complex_unicode.invalid.1")
{
    auto const text = "\xFF"sv;
    auto recorder = event_recorder { text };
    auto state = unicode::detail::unicode_process_state {};
    auto const result = unicode::detail::process_only_complex_unicode(recorder, state, text, 80);
    CHECK(result.end == text.data() + 1);
    CHECK(result.totalWidth == 1);
    REQUIRE(recorder.size() == 1);
    REQUIRE(std::holds_alternative<invalid_sequence>(recorder.records().at(0)));
    CHECK(std::get<invalid_sequence>(recorder.records().at(0)).value == "\xFF"sv);

    unicode::detail::flush_complex_unicode(recorder, state, 80);
    CHECK(recorder.size() == 1);
}

TEST_CASE("grapheme_line_segmenter.process_only_complex_unicode.invalid.2")
{
    auto const text = "\xFF\xFFx"sv;
    auto recorder = event_recorder { text };
    auto state = unicode::detail::unicode_process_state {};
    auto const result = unicode::detail::process_only_complex_unicode(recorder, state, text, 80);
    CHECK(result.end == text.data() + 2);
    CHECK(result.totalWidth == 2);
    CHECK(recorder.size() == 2);

    unicode::detail::flush_complex_unicode(recorder, state, 80);
    CHECK(recorder.size() == 2);
}

TEST_CASE("grapheme_line_segmenter.process_only_complex_unicode.sliced_calls")
{
    auto constexpr text = "\xF0\x9F\x98\x80\033\\0123456789ABCDEF"sv; // U+1F600
    auto constexpr splitOffset = 3;
    auto constexpr chunk1 = text.substr(0, splitOffset);

    auto recorder = event_recorder { text };
    auto state = unicode::detail::unicode_process_state {};
    auto const r1 = unicode::detail::process_only_complex_unicode(recorder, state, chunk1, 80);
    REQUIRE(state.utf8.expectedLength == 4);
    REQUIRE(state.utf8.currentLength == 3);
    // We must not have emitted any grapheme cluster yet.
    CHECK(std::distance(chunk1.data(), r1.end) == 3);
    CHECK(r1.totalWidth == 0);

    auto constexpr chunk2 = text.substr(splitOffset);
    auto const r2 = unicode::detail::process_only_complex_unicode(recorder, state, chunk2, 80);
    CHECK(r2.end == chunk2.data() + 1);
    CHECK(r2.totalWidth == 2);
    CHECK(state.utf8.expectedLength == 0);
    CHECK(state.utf8.currentLength == 0);
}

// }}}

TEST_CASE("grapheme_line_segmenter.nocallbacks")
{
    auto segmenter = unicode::grapheme_line_segmenter {};
    segmenter.reset("Hello\033[m"sv);
    auto const result = segmenter.process(80);
    CHECK(result.text == "Hello"sv);
    CHECK(result.width == 5);
    CHECK(result.stop_condition == StopCondition::UnexpectedInput);
}

// {{{ test ASCII-only
TEST_CASE("grapheme_line_segmenter.ascii.empty")
{
    CHECK(scan_text(""sv, 4) == segmentation_result { "", 0, StopCondition::EndOfInput });
}

TEST_CASE("grapheme_line_segmenter.ascii.32")
{
    auto const text = "0123456789ABCDEF0123456789ABCDEF"sv;

    auto const shortStr = [&](size_t len) {
        return text.substr(0, len);
    };

    // clang-format off
    CHECK(scan_text(text, 32) == segmentation_result { .text = text, .width = 32, .stop_condition = StopCondition::EndOfInput });
    CHECK(scan_text(text, 16) == segmentation_result { .text = shortStr(16), .width = 16, .stop_condition = StopCondition::EndOfWidth });
    CHECK(scan_text(text, 8) == segmentation_result { .text = shortStr(8), .width = 8, .stop_condition = StopCondition::EndOfWidth });
    CHECK(scan_text(text, 1) == segmentation_result { .text = shortStr(1), .width = 1, .stop_condition = StopCondition::EndOfWidth });
    CHECK(scan_text(text, 0) == segmentation_result { .text = shortStr(0), .width = 0, .stop_condition = StopCondition::EndOfWidth });
    // clang-format on
}

TEST_CASE("grapheme_line_segmenter.ascii.mixed_with_controls")
{
    // clang-format off
    CHECK(scan_text("\0331234", 80) == segmentation_result { "", 0, StopCondition::UnexpectedInput });
    CHECK(scan_text("1234\033", 80) == segmentation_result { "1234", 4, StopCondition::UnexpectedInput });
    CHECK(scan_text("12345678\033", 80) == segmentation_result { "12345678", 8, StopCondition::UnexpectedInput });
    CHECK(scan_text("0123456789ABCDEF\033", 80) == segmentation_result { "0123456789ABCDEF", 16, StopCondition::UnexpectedInput });
    CHECK(scan_text("0123456789ABCDEF1\033", 80) == segmentation_result { "0123456789ABCDEF1", 17, StopCondition::UnexpectedInput });
    CHECK(scan_text("0123456789ABCDEF0123456789ABCD\033F", 80) == segmentation_result { "0123456789ABCDEF0123456789ABCD", 30, StopCondition::UnexpectedInput });
    // clang-format on
}
// }}}

// {{{ test complex unicode only
TEST_CASE("grapheme_line_segmenter.complex.grapheme_cluster.1")
{
    auto constexpr OUmlaut = "\xC3\xB6"sv;
    CHECK(scan_text(OUmlaut, 2) == segmentation_result { OUmlaut, 1, StopCondition::EndOfInput });

    auto const familyEmoji8 = u8(FamilyEmoji);
    auto const result = scan_text(familyEmoji8, 80);
    CHECK(result == segmentation_result { familyEmoji8, 2, StopCondition::EndOfInput });
}

TEST_CASE("grapheme_line_segmenter.complex.grapheme_cluster.2")
{
    auto const familyEmoji8 = u8(FamilyEmoji) + u8(FamilyEmoji);
    auto const result = scan_text(familyEmoji8, 80);
    CHECK(result == segmentation_result { .text = familyEmoji8, .width = 4, .stop_condition = StopCondition::EndOfInput });
}
// }}}

// {{{ test invalid UTF-8 sequences
TEST_CASE("grapheme_line_segmenter.invalid_char.1")
{
    auto const text = "1234\x80"sv;
    auto recorder = event_recorder { text };
    auto segmenter = grapheme_line_segmenter { recorder, text };
    auto const result = segmenter.process(80);
    UNSCOPED_INFO("result: " << result);
    REQUIRE(recorder.size() == 2);
    REQUIRE(std::holds_alternative<ascii_sequence>(recorder.records().at(0)));
    REQUIRE(std::holds_alternative<invalid_sequence>(recorder.records().at(1)));
    REQUIRE(std::get<ascii_sequence>(recorder.records().at(0)).value == "1234"sv);
    REQUIRE(std::get<invalid_sequence>(recorder.records().at(1)).value == "\x80"sv);
}
// }}}

// {{{ mixed primitive ASCII and complex unicode
TEST_CASE("grapheme_line_segmenter.mixed.1")
{
    auto const text = "0123456789{\xE2\x94\x80}ABCDEF"sv; // \xE2 \x94 \x80 == U+2500
    auto recorder = event_recorder { text };
    auto segmenter = grapheme_line_segmenter { recorder, text };
    auto const result = segmenter.process(80);
    segmenter.flush(80);

    UNSCOPED_INFO("result: " << result);
    CHECK(e(result.text) == e(text));
    CHECK(result.width == 19);

    CHECK(recorder.size() == 3);
    auto const& records = recorder.records();
    CHECK(std::get<ascii_sequence>(records.at(0)) == ascii_sequence { "0123456789{"sv });
    CHECK(std::get<complex_unicode_sequence>(records.at(1)) == complex_unicode_sequence { "\xE2\x94\x80"sv, 1 });
    CHECK(std::get<ascii_sequence>(records.at(2)) == ascii_sequence { "}ABCDEF"sv });
}

TEST_CASE("grapheme_line_segmenter.mixed.2")
{
    auto const text = u8(FamilyEmoji);
    auto segmenter = grapheme_line_segmenter { event_logger::get(), text };
    auto const main = segmenter.process(80);
    auto const fini = segmenter.flush(80);
    CHECK(static_cast<size_t>(std::distance(text.data(), segmenter.next())) == text.size());
    CHECK(main == segmentation_result { .text = "", .width = 0, .stop_condition = StopCondition::EndOfInput });
    CHECK(fini == segmentation_result { .text = text, .width = 2, .stop_condition = StopCondition::EndOfInput });
}

TEST_CASE("grapheme_line_segmenter.mixed.3")
{
    auto const text = u8(FamilyEmoji) + "ABC"s;
    auto segmenter = grapheme_line_segmenter { event_logger::get(), text };

    auto const main = segmenter.process(80);
    CHECK(static_cast<size_t>(std::distance(text.data(), segmenter.next())) == text.size());
    CHECK(main == segmentation_result { .text = text, .width = 5, .stop_condition = StopCondition::EndOfInput });

    // Because we've scanned it all already in the main call, the flush() call will return an empty result.
    auto const fini = segmenter.flush(80);
    CHECK(fini == segmentation_result { .text = {}, .width = 0, .stop_condition = StopCondition::EndOfInput });
}

TEST_CASE("grapheme_line_segmenter.mixed.4")
{
    auto const text = u8(FamilyEmoji) + "ABC"s + u8(FamilyEmoji);
    auto const result = scan_text(text, 80);
    CHECK(result == segmentation_result { .text = text, .width = 7, .stop_condition = StopCondition::EndOfInput });
}
// }}}

TEST_CASE("grapheme_line_segmenter.complex.half-overflowing")
{
    auto const oneEmoji = u8(SmileyEmoji);
    auto const textStr = oneEmoji + oneEmoji + oneEmoji;
    auto const text = std::string_view(textStr);

    INFO("match at boundary of first grapheme cluster");
    CHECK(scan_text(text, 2) == segmentation_result { text.substr(0, 1 * oneEmoji.size()), 2, StopCondition::EndOfWidth });

    INFO("match at boundary of second grapheme cluster");
    CHECK(scan_text(text, 4) == segmentation_result { text.substr(0, 2 * oneEmoji.size()), 4, StopCondition::EndOfWidth });

    INFO("second grapheme cluster is half overflowing");
    CHECK(scan_text(text, 3) == segmentation_result { text.substr(0, 1 * oneEmoji.size()), 2, StopCondition::EndOfWidth });

    INFO("third grapheme cluster is half overflowing");
    CHECK(scan_text(text, 5) == segmentation_result { text.substr(0, 2 * oneEmoji.size()), 4, StopCondition::EndOfInput });
}

TEST_CASE("grapheme_line_segmenter.complex.half-overflowing.and-resume.1")
{
    auto const oneEmoji = u8(SmileyEmoji);
    auto const textStr = oneEmoji + oneEmoji + oneEmoji;
    auto const text = std::string_view(textStr);
    auto recorder = event_recorder { text };
    auto segmenter = grapheme_line_segmenter { recorder, text };

    auto const one = segmenter.process(3); // first emoji (second is overlapping)
    CHECK(one.text == text.substr(0, 4));
    CHECK(one.width == 2);
    CHECK((void*) one.text.data() == (void*) (text.data() + 0));
    CHECK(recorder.size() == 1);
    CHECK(recorder.at(0) == expectation { .offset = 0, .size = 4, .width = 2 });

    auto const two = segmenter.process(2); // resume second emoji
    CHECK(two.text == text.substr(4, 4));
    CHECK(two.width == 2);
    CHECK((void*) two.text.data() == (void*) (text.data() + 4));
    CHECK(recorder.size() == 2);
    CHECK(recorder.at(1) == expectation { .offset = 4, .size = 4, .width = 2 });
}

TEST_CASE("grapheme_line_segmenter.complex.half-overflowing.and-resume.2")
{
    auto const oneEmoji = u8(SmileyEmoji);
    auto const textStr = oneEmoji + oneEmoji + oneEmoji;
    auto const text = std::string_view(textStr);
    auto recorder = event_recorder { text };
    auto segmenter = grapheme_line_segmenter { recorder, text };
    INFO(fmt::format("oneEmoji: {} ({}), text: {}", e(oneEmoji), oneEmoji.size(), e(text)));

    // Process first two emoji (thrid is overlapping)
    auto const one = segmenter.process(5);
    INFO("one: " << one);
    CHECK(one.text == text.substr(0, 8));
    CHECK(one.width == 4);
    CHECK((void*) one.text.data() == (void*) (text.data() + 0));
    CHECK(recorder.size() == 2);
    CHECK(recorder.at(0) == expectation { .offset = 0, .size = 4, .width = 2 });
    CHECK(recorder.at(1) == expectation { .offset = 4, .size = 4, .width = 2 });

    // Resume processing third emoji
    // This emoji cannot be emitted yet, because it is not known yet whether it's a full emoji or not
    // (e.g. due to variation selectors).
    auto const two = segmenter.process(2); // resume second emoji
    INFO("two: " << two);
    CHECK(two.text.empty());
    CHECK(two.width == 0);
    CHECK(recorder.size() == 2);

    // Inform the segmenter that we've explicitly reached the end of the input.
    // Thus, the segmenter must emit the third emoji now.
    auto const fin = segmenter.flush(2);
    CHECK(fin.text == text.substr(8, 4));
    CHECK(fin.width == 2);
    CHECK((void*) fin.text.data() == (void*) (text.data() + 8));
    CHECK(recorder.size() == 3);
    CHECK(recorder.at(2) == expectation { .offset = 8, .size = 4, .width = 2 });
}

TEST_CASE("grapheme_line_segmenter.complex.sliced_calls")
{
    // auto const text = u8(SmileyEmoji) + "\033\\0123456789ABCDEF"s; // U+1F600
    auto constexpr text = "\xF0\x9F\x98\x80\033\\0123456789ABCDEF"sv; // U+1F600
    auto constexpr splitOffset = 3;
    auto constexpr chunkOne = text.substr(0, splitOffset);
    auto constexpr chunkTwo = text.substr(splitOffset);

    auto recorder = event_recorder { text };
    auto segmenter = grapheme_line_segmenter { recorder, chunkOne };
    auto const result1 = segmenter.process(3);

    // For the 4-byte sequence, we've only read 3 bytes, so we expect the next call to continue reading the 4th byte.
    REQUIRE(segmenter.utf8_state().expectedLength == 4);
    REQUIRE(segmenter.utf8_state().currentLength == 3);
    CHECK(result1.width == 0); // We must not have emitted any grapheme cluster yet.
    CHECK(result1.text == "");
    CHECK(result1.stop_condition == StopCondition::EndOfInput);

    segmenter.expand_buffer_by(chunkTwo.size());
    auto const result2 = segmenter.process(80);

    REQUIRE(segmenter.utf8_state().expectedLength == 0);
    CHECK(result2.width == 2);
    CHECK(result2.stop_condition == StopCondition::UnexpectedInput); // control character \033
    REQUIRE(e(result2.text) == e(u8(SmileyEmoji)));
}

TEST_CASE("grapheme_utf8.0")
{
    auto constexpr text = "\xC3\xB6"sv; // 'ö'

    const auto* input = text.data();
    const auto* const end = text.data() + text.size();

    auto recorder = event_recorder { "single_utf8" };
    auto segmenter = grapheme_line_segmenter { recorder, ""sv };

    auto const chunk = std::string_view(input, end);
    TRACE("Processing {}...{} ({})\n", (void*) input, (void*) end, std::distance(input, end));
    segmenter.reset(chunk);

    auto const result = segmenter.process(10);
    TRACE("result: [text: \"{}\", width: {}, stop: {}]\n", result.text, result.width, [](auto val) {
        switch (val)
        {
            case unicode::StopCondition::UnexpectedInput: return "UnexpectedInput";
            case unicode::StopCondition::EndOfWidth: return "EndOfWidth";
            case unicode::StopCondition::EndOfInput: return "EndOfInput";
        }
        return "Unknown";
    }(result.stop_condition));

    CHECK(result.text == text);
    CHECK(result.width == 0);
}

TEST_CASE("grapheme_utf8.1")
{
    auto constexpr text = "\xC3\xB6 "sv; // 'ö '

    const auto* input = text.data();
    const auto* const end = text.data() + text.size();

    auto recorder = event_recorder { "single_utf8" };
    auto segmenter = grapheme_line_segmenter { recorder, ""sv };

    auto const chunk = std::string_view(input, end);
    TRACE("Processing {}...{} ({})\n", (void*) input, (void*) end, std::distance(input, end));
    segmenter.reset(chunk);

    auto const result = segmenter.process(10);
    TRACE("result: [text: \"{}\", width: {}, stop: {}]\n", result.text, result.width, [](auto val) {
        switch (val)
        {
            case unicode::StopCondition::UnexpectedInput: return "UnexpectedInput";
            case unicode::StopCondition::EndOfWidth: return "EndOfWidth";
            case unicode::StopCondition::EndOfInput: return "EndOfInput";
        }
        return "Unknown";
    }(result.stop_condition));

    CHECK(result.text == text);
    CHECK(result.width == 2);
}

TEST_CASE("grapheme_utf8.2")
{
    auto constexpr text = "a\xC3\xB6a"sv; // 'aöa'

    const auto* input = text.data();
    const auto* const end = text.data() + text.size();

    auto recorder = event_recorder { "single_utf8" };
    auto segmenter = grapheme_line_segmenter { recorder, ""sv };

    auto const chunk = std::string_view(input, end);
    TRACE("Processing {}...{} ({})\n", (void*) input, (void*) end, std::distance(input, end));
    segmenter.reset(chunk);

    auto const result = segmenter.process(10);
    TRACE("result: [text: \"{}\", width: {}, stop: {}]\n", result.text, result.width, [](auto val) {
        switch (val)
        {
            case unicode::StopCondition::UnexpectedInput: return "UnexpectedInput";
            case unicode::StopCondition::EndOfWidth: return "EndOfWidth";
            case unicode::StopCondition::EndOfInput: return "EndOfInput";
        }
        return "Unknown";
    }(result.stop_condition));

    CHECK(result.text == text);
    CHECK(result.width == 3);
}
