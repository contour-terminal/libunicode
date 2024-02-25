// SPDX-License-Identifier: Apache-2.0
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

namespace
{

std::string_view file_header = "// SPDX-License-Identifier: Apache-2.0\n"sv;

struct apigen_configuration
{
    std::string ucdDataDirectory;
    std::ofstream cxxUcdFile;
    std::ofstream hppUcdFile;
    std::ofstream hppUcdEnumsFile;
    std::ofstream hppUcdFmtFile;
    std::ofstream hppUcdOstreamFile;
};

struct plane_info
{
    uint8_t plane;
    uint32_t start;
    uint32_t end;
    std::string_view short_name;
    std::string_view full_name;
};

std::string sanitize_identifier(std::string_view s)
{
    std::string result;
    for (char const c: s)
        result += (c == '-' || c == ' ') ? '_' : c;
    return result;
}

std::string_view minimal_uint(uint64_t maxValue) noexcept
{
    if (maxValue <= 0xFF)
        return "uint8_t";
    if (maxValue <= 0xFFFF)
        return "uint16_t";
    if (maxValue <= 0xFFFFFFFF)
        return "uint32_t";
    return "uint64_t";
}

// {{{ enum_builder
class enum_builder
{
  public:
    virtual ~enum_builder() = default;

    virtual void begin(std::string_view enumClass, int firstValue) = 0;
    virtual void member(std::string_view name) = 0;
    virtual void end() = 0;
};
// }}}
// {{{ enum_builder_array
class enum_builder_array: public enum_builder
{
  public:
    template <typename... Builders>
    enum_builder_array(Builders&&... builders)
    {
        (_builders.emplace_back(std::forward<Builders>(builders)), ...);
    }

    void begin(std::string_view enumClass, int firstValue) override
    {
        for (auto& b: _builders)
            b->begin(enumClass, firstValue);
    }

    void member(std::string_view name) override
    {
        for (auto& b: _builders)
            b->member(name);
    }

    void end() override
    {
        for (auto& b: _builders)
            b->end();
    }

  private:
    std::vector<std::unique_ptr<enum_builder>> _builders;
};
// }}}
// {{{ enum_class_writer
class enum_class_writer: public enum_builder
{
  public:
    explicit enum_class_writer(std::ostream& os): _os { os }
    {
        _os << file_header;
        _os << "#pragma once\n";
        _os << "\n";
        _os << "#include <cstdint>\n";
        _os << "\n";
        _os << "namespace unicode\n{\n\n";
    }

    ~enum_class_writer() override { _os << "} // end namespace unicode\n"; }

    void begin(std::string_view enumClass, int firstValue) override
    {
        _enumClass = enumClass;
        _nextValue = firstValue;
    }

    void member(std::string_view name) override { _collectedMembers.emplace_back(name); }

    void end() override
    {
        _os << "enum class " << _enumClass << ": " << minimal_uint(_collectedMembers.size()) << " {\n";
        for (auto const& m: _collectedMembers)
            _os << "    " << m << " = " << _nextValue++ << ",\n";
        _os << "};\n\n";
    }

  private:
    std::ostream& _os;
    std::string _enumClass;
    std::vector<std::string> _collectedMembers;
    int _nextValue = 0;
};
// }}}
// {{{ enum_ostream_writer
class enum_ostream_writer: public enum_builder
{
  public:
    explicit enum_ostream_writer(std::ostream& os): _os { os }
    {
        _os << file_header;
        _os << "#pragma once\n";
        _os << "\n";
        _os << "#include <libunicode/ucd.h>\n";
        _os << "\n";
        _os << "#include <ostream>\n";
        _os << "\n";
        _os << "namespace unicode\n{\n\n";
    }

    ~enum_ostream_writer() override { _os << "} // end namespace unicode\n"; }

    void begin(std::string_view enumClass, int /*firstValue*/) override
    {
        _enumClass = enumClass;
        _os << "inline std::ostream& operator<<(std::ostream& os, " << _enumClass << " value) noexcept\n{\n";
        _os << "    // clang-format off\n";
        _os << "    switch (value)\n";
        _os << "    {\n";
    }

    void member(std::string_view name) override
    {
        _os << "    case " << _enumClass << "::" << sanitize_identifier(name) << ": return os << \"" << name
            << "\";\n";
    }

    void end() override
    {
        _os << "    }\n";
        _os << "    // clang-format on\n";
        _os << "    return os << \"(\" << static_cast<unsigned>(value) << \")\";\n";
    }

  private:
    std::ostream& _os;
    std::string _enumClass;
};
// }}}
// {{{ enum_fmt_writer
class enum_fmt_writer: public enum_builder
{
  public:
    explicit enum_fmt_writer(std::ostream& os): _os { &os }
    {
        *_os << file_header;
        *_os << "#pragma once\n";
        *_os << "\n";
        *_os << "#include <libunicode/ucd_enums.h>\n";
        *_os << "\n";
        *_os << "#include <fmt/format.h>\n";
        *_os << "\n";
        *_os << "namespace fmt\n{\n\n";
    }

    ~enum_fmt_writer() override { *_os << "} // end namespace fmt\n"; }

    void begin(std::string_view enumClass, int /*firstValue*/) override
    {
        _enumClass = "unicode::" + std::string(enumClass);
        *_os << "template <>\n";
        *_os << "struct formatter<" << _enumClass << ">\n{\n";
        *_os << "    template <typename ParseContext>\n";
        *_os << "    constexpr auto parse(ParseContext& ctx)\n";
        *_os << "    {\n";
        *_os << "        return ctx.begin();\n";
        *_os << "    }\n";
        *_os << "    template <typename FormatContext>\n";
        *_os << "    auto format(" << _enumClass << " value, FormatContext& ctx)\n";
        *_os << "    {\n";
        *_os << "        switch (value)\n";
        *_os << "        {\n";
        *_os << "            // clang-format off\n";
    }

    void member(std::string_view name) override
    {
        *_os << "            // case " << _enumClass << "::" << sanitize_identifier(name)
             << ": return fmt::format_to(ctx.out(), \"" << name << "\");\n";
    }

    void end() override
    {
        *_os << "            // clang-format on\n";
        *_os << "        }\n";
        *_os << "        return fmt::format_to(ctx.out(), \"({})\", unsigned(value));\n";
        *_os << "    }\n";
        *_os << "};\n\n";
    }

  private:
    std::ostream* _os;
    std::string _enumClass;
};
// }}}
// {{{ range_table_generator
struct merged_range
{
    uint32_t range_from;
    uint32_t range_to;
    uint32_t value;
    std::vector<std::string> comments;
    size_t count;
};

class range_table_generator
{
  public:
    void append(uint32_t from, uint32_t to, uint32_t value, std::string_view comment)
    {
        if (_current.range_to + 1 == from && _current.value == value)
        {
            // This is a direct upper neighbor of the current range.
            _current.range_to = to;
            _current.count += 1;
            _current.comments.emplace_back(comment);
        }
        else
        {
            flush();
            _current = { .range_from = from,
                         .range_to = to,
                         .value = value,
                         .comments = { std::string(comment) },
                         .count = 1 };
        }
    }

    std::vector<merged_range> const& flush()
    {
        if (_current.value)
            _result.emplace_back(_current);
        _current = {};
        return _result;
    }

  private:
    merged_range _current;
    std::vector<merged_range> _result;
};
// }}}

class ucd_generator
{
  public:
    ucd_generator(apigen_configuration& config):
        _enumBuilders(std::make_unique<enum_class_writer>(config.hppUcdEnumsFile),
                      std::make_unique<enum_ostream_writer>(config.hppUcdOstreamFile),
                      std::make_unique<enum_fmt_writer>(config.hppUcdFmtFile))
    {
    }

  private:
    enum_builder_array _enumBuilders;
};

} // namespace

int main(int argc, char const* argv[])
{
    (void) argc;
    (void) argv;
    return EXIT_SUCCESS; // TODO(pr)
}
