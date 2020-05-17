#! /usr/bin/env python3
"""/**
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
"""

import re

PROJECT_ROOT = '/home/trapni/projects/libunicode'

# unicode database (extracted zip file): https://www.unicode.org/Public/UCD/latest/ucd/
UCD_DIR = PROJECT_ROOT + '/docs/ucd'

FOLD_OPEN = '{{{'
FOLD_CLOSE = '}}}'

class UCDGenerator:
    def __init__(self, _ucd_dir, _header_file, _impl_file):
        self.ucd_dir = _ucd_dir
        self.header = open(_header_file, 'w')
        self.impl = open(_impl_file, 'w')
        self.singleValueRE = re.compile('([0-9A-F]+)\s*;\s*(\w+)\s*#\s*(.*)$')
        self.rangeValueRE = re.compile('([0-9A-F]+)\.\.([0-9A-F]+)\s*;\s*(\w+)\s*#\s*(.*)$')
        self.general_category_map = dict()
        self.general_category = list()

    def close(self):
        self.header.close()
        self.impl.close()

    def generate(self):
        self.load_property_value_aliases()
        self.load_general_category()
        self.load_core_properties()

        self.file_header()
        self.write_properties()
        self.write_core_properties()
        self.write_general_categories()

        self.process_grapheme_break_props()
        self.process_east_asian_width()
        self.process_emoji_props()

        self.file_footer()

    def load_property_value_aliases(self): # {{{
        with open(self.ucd_dir + '/PropertyValueAliases.txt') as f:
            # gc ; C   ; Other    # Cc | Cf | Cn | Co | Cs
            headerRE = re.compile('^#\s*(\w+) \((\w+)\)$')
            lineRE = re.compile('^(\w+)\s*;\s*([a-zA-Z0-9_\.]+)\s*;\s*([a-zA-Z0-9_]+).*$')
            property_values = dict()

            while True:
                line = f.readline()
                if not line:
                    break
                m = headerRE.match(line)
                if m:
                    name = m.group(1)
                    name_abbrev = m.group(2)
                    property_values[name] = dict()
                    continue
                # TODO: treat Canonical_Combining_Class differently (see .txt file)
                m = lineRE.match(line)
                if m:
                    a = m.group(1)
                    value_abbrev = m.group(2)
                    value = m.group(3)
                    property_values[name][value_abbrev] = value

            self.property_values = property_values
        # }}}

    def write_properties(self): # {{{
        self.header.write('// {} General property enum classes\n'.format(FOLD_OPEN))
        for name in sorted(self.property_values.keys()):
            if len(self.property_values[name].keys()) == 0:
                continue
            if (len(self.property_values[name].keys()) == 2
                    and ('Yes' in self.property_values[name].values())
                    and ('No'  in self.property_values[name].values())):
                continue
            if name in ['General_Category', 'Grapheme_Cluster_Break']:
                # XXX those properties are generated specifically
                continue
            self.header.write('enum class {} {{\n'.format(name))
            for value in sorted(self.property_values[name].keys()):
                self.header.write('    {},\n'.format(self.property_values[name][value]))
            self.header.write('};\n\n')
        self.header.write("// {}\n\n".format(FOLD_CLOSE))
        # }}}

    def load_general_category(self): # {{{
        with open(self.ucd_dir + '/extracted/DerivedGeneralCategory.txt', 'r') as f:
            headerRE = re.compile('^#\s*General_Category=(\w+)$')
            property_values = self.property_values['General_Category']
            cat_name = ''
            cats_grouped = dict()
            cats_all = []
            while True:
                line = f.readline()
                if not line:
                    break
                m = headerRE.match(line)
                if m:
                    cat_name = m.group(1)
                    if not (cat_name in cats_grouped):
                        cats_grouped[cat_name] = []
                if len(line) == 0 or line[0] == '#':
                    continue
                m = self.singleValueRE.match(line)
                if m:
                    code = int(m.group(1), 16)
                    prop = property_values[m.group(2)]
                    comment = m.group(3)
                    cats_grouped[cat_name].append({'start': code, 'end': code, 'property': prop, 'comment': comment})
                    cats_all.append({'start': code, 'end': code, 'category': cat_name, 'property': prop, 'comment': comment})
                m = self.rangeValueRE.match(line)
                if m:
                    start = int(m.group(1), 16)
                    end = int(m.group(2), 16)
                    prop = property_values[m.group(3)]
                    comment = m.group(4)
                    cats_grouped[cat_name].append({'start': start, 'end': end, 'property': prop, 'comment': comment})
                    cats_all.append({'start': start, 'end': end, 'category': cat_name, 'property': prop, 'comment': comment})
            self.general_category_map = cats_grouped

            cats_all.sort(key = lambda a: a['start'])
            self.general_category = cats_all
        # }}}

    def file_header(self): # {{{
        self.header.write(globals()['__doc__'])
        self.header.write("""#pragma once

#include <array>
#include <optional>
#include <string>
#include <utility>

namespace unicode {

""")

        self.impl.write(globals()['__doc__'])
        self.impl.write("""
#include <unicode/ucd.h>
#include <unicode/ucd_private.h>

#include <array>
#include <optional>
#include <string>

namespace unicode {

""") # }}}

    def file_footer(self): # {{{
        self.header.write("} // end namespace\n")
        self.impl.write("} // end namespace\n")
# }}}

    def load_core_properties(self): # {{{
        with open(self.ucd_dir + '/DerivedCoreProperties.txt', 'r') as f:
            # collect
            props = dict()
            while True:
                line = f.readline()
                if not line:
                    break
                if len(line) == 0 or line[0] == '#':
                    continue
                m = self.singleValueRE.match(line)
                if m:
                    code = int(m.group(1), 16)
                    prop = m.group(2)
                    comment = m.group(3)
                    if not (prop in props):
                        props[prop] = []
                    props[prop].append({'start': code, 'end': code, 'comment': comment})
                m = self.rangeValueRE.match(line)
                if m:
                    start = int(m.group(1), 16)
                    end = int(m.group(2), 16)
                    prop = m.group(3)
                    comment = m.group(4)
                    if not (prop in props):
                        props[prop] = []
                    props[prop].append({'start': start, 'end': end, 'comment': comment})
            # sort table
            for prop_key in props.keys():
                props[prop_key].sort(key = lambda a: a['start'])
        self.core_properties = props
        # }}}

    def write_core_properties(self): # {{{
        props = self.core_properties

        # write range tables
        self.impl.write("namespace tables {\n")
        for name in sorted(props.keys()):
            self.impl.write("auto constexpr {} = std::array{{ // {}\n".format(name, FOLD_OPEN))
            for propRange in props[name]:
                self.impl.write("    Interval{{ 0x{:>04X}, 0x{:>04X} }}, // {}\n".format(propRange['start'], propRange['end'], propRange['comment']))
            self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
        self.impl.write("} // end namespace tables\n\n")

        # write out test function
        self.impl.write("bool contains(Core_Property _prop, char32_t _codepoint) noexcept {\n")
        self.impl.write("    switch (_prop) {\n")
        for name in sorted(props.keys()):
            self.impl.write("        case Core_Property::{0:}: return contains(tables::{0:}, _codepoint);\n".format(name))
        self.impl.write("    }\n")
        self.impl.write("    return false;\n")
        self.impl.write("}\n\n")

        # API: write enum and tester
        self.header.write("enum class Core_Property {\n")
        for name in sorted(props.keys()):
            self.header.write("    {},\n".format(name))
        self.header.write("};\n\n")
        self.header.write("bool contains(Core_Property _prop, char32_t _codepoint) noexcept;\n\n")
        # }}}

    def process_props(self, filename, prop_key): # {{{
        with open(filename, 'r') as f:
            headerRE = re.compile('^#\s*{}:\s*(\w+)$'.format(prop_key))

            # collect
            props_name = ''
            props = dict()
            while True:
                line = f.readline()
                if not line:
                    break
                m = headerRE.match(line)
                if m:
                    props_name = m.group(1)
                    if not (props_name in props):
                        props[props_name] = []
                m = self.singleValueRE.match(line)
                if m:
                    code = int(m.group(1), 16)
                    prop = m.group(2)
                    comment = m.group(3)
                    props[props_name].append({'start': code, 'end': code, 'property': prop, 'comment': comment})
                m = self.rangeValueRE.match(line)
                if m:
                    start = int(m.group(1), 16)
                    end = int(m.group(2), 16)
                    prop = m.group(3)
                    comment = m.group(4)
                    props[props_name].append({'start': start, 'end': end, 'property': prop, 'comment': comment})

            # sort table
            for prop_key in props.keys():
                props[prop_key].sort(key = lambda a: a['start'])

            # write range tables
            self.impl.write("namespace tables {\n")
            for name in sorted(props.keys()):
                self.impl.write("auto constexpr {} = std::array{{ // {}\n".format(name, FOLD_OPEN))
                for propRange in props[name]:
                    self.impl.write("    Prop<::unicode::{}>{{ {{ 0x{:>04X}, 0x{:>04X} }}, ::unicode::{}::{} }}, // {}\n".format(
                               name,
                               propRange['start'],
                               propRange['end'],
                               name,
                               propRange['property'],
                               propRange['comment']))
                self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
            self.impl.write("} // end namespace tables\n\n")

            # write enums / signature
            for name in sorted(props.keys()):
                self.header.write('enum class {} {{\n'.format(name))
                enum_set = set()
                for enum in props[name]:
                    enum_set.add(enum['property'])
                for enum in sorted(enum_set):
                    self.header.write("    {},\n".format(enum))
                self.header.write("};\n\n")

            for name in sorted(props.keys()):
                self.impl.write('namespace {} {{\n'.format(name.lower()))
                self.header.write('namespace {} {{\n'.format(name.lower()))
                enum_set = set()
                for enum in props[name]:
                    enum_set.add(enum['property'])
                for enum in sorted(enum_set):
                    self.header.write('    bool {}(char32_t _codepoint) noexcept;\n'.format(enum.lower()))
                    self.impl.write('    bool {}(char32_t _codepoint) noexcept {{\n'.format(enum.lower()))
                    self.impl.write("        if (auto p = search(tables::{}, _codepoint); p.has_value())\n".format(name))
                    self.impl.write('            return p.value() == {}::{};\n'.format(name, enum))
                    self.impl.write('        return false;\n')
                    self.impl.write('    }\n\n')
                self.header.write('}\n')
                self.impl.write('}\n')
            self.header.write('\n')
            self.impl.write('\n')
        # }}}

    def process_grapheme_break_props(self):
        self.process_props(self.ucd_dir + '/auxiliary/GraphemeBreakProperty.txt', 'Property')

    def parse_range(self, line): # {{{
        m = self.singleValueRE.match(line)
        if m:
            code = int(m.group(1), 16)
            prop = m.group(2)
            comment = m.group(3)
            return {'start': code, 'end': code, 'property': prop, 'comment': comment}
        m = self.rangeValueRE.match(line)
        if m:
            start = int(m.group(1), 16)
            end = int(m.group(2), 16)
            prop = m.group(3)
            comment = m.group(4)
            return {'start': start, 'end': end, 'property': prop, 'comment': comment}
        return None
        # }}}

    def process_emoji_props(self): # {{{
        with open(self.ucd_dir + '/emoji/emoji-data.txt', 'r') as f:
            # collect
            props_name = ''
            props = dict()
            while True:
                line = f.readline()
                if not line:
                    break
                r = self.parse_range(line)
                if r != None:
                    name = r['property']
                    if not name in props:
                        props[name] = []
                    props[name].append(r)

            # sort table
            for prop_key in props.keys():
                    props[prop_key].sort(key = lambda a: a['start'])

            # write range tables
            self.impl.write("namespace tables {\n")
            for name in sorted(props.keys()):
                self.impl.write("auto constexpr {} = std::array{{ // {}\n".format(name, FOLD_OPEN))
                for propRange in props[name]:
                    self.impl.write("    Interval{{ 0x{:>04X}, 0x{:>04X} }}, // {}\n".format(
                               propRange['start'],
                               propRange['end'],
                               propRange['comment']))
                self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
            self.impl.write("} // end namespace tables\n\n")

            # write out test function
            for name in sorted(props.keys()):
                self.impl.write('bool {}(char32_t _codepoint) noexcept {{\n'.format(name.lower()))
                self.impl.write("    return contains(tables::{0:}, _codepoint);\n".format(name))
                self.impl.write("}\n\n")

            # write enums / signature
            for name in sorted(props.keys()):
                self.header.write('bool {}(char32_t _codepoint) noexcept;\n'.format(name.lower()))
            self.header.write('\n')
        # }}}

    def write_general_categories(self): # {{{
        gcats = self.general_category
        self.impl.write("namespace tables {\n")
        type_name = "General_Category"
        fqdn_type_name = "::unicode::{}".format(type_name)
        self.impl.write("auto const {} = std::array{{\n".format(type_name))
        for cat in gcats:
            self.impl.write(
                "    Prop<{}>{{ {{ 0x{:>04X}, 0x{:>04X} }}, {}::{} }}, // {}\n".format(
                fqdn_type_name,
                cat['start'],
                cat['end'],
                fqdn_type_name,
                cat['property'],
                cat['comment']))
        self.impl.write("};\n")
        self.impl.write("} // end namespace tables\n\n")

        self.impl.write("namespace {} {{\n".format(type_name.lower()))
        self.impl.write("    {} get(char32_t _value) noexcept {{\n".format(type_name))
        self.impl.write("        if (auto const p = search(tables::{}, _value); p.has_value())\n".format(type_name))
        self.impl.write('            return p.value();\n')
        self.impl.write('        return {}::Unspecified;\n'.format(type_name))
        self.impl.write("    }\n")
        self.impl.write("}\n\n")
        # -----------------------------------------------------------------------------------------------

        cats = self.general_category_map

        # write range tables
        self.impl.write("namespace tables {\n")
        for name in sorted(cats.keys()):
            self.impl.write("auto constexpr {} = std::array{{ // {}\n".format(name, FOLD_OPEN))
            for propRange in cats[name]:
                self.impl.write("    Interval{{ 0x{:>04X}, 0x{:>04X} }}, // {}\n".format(propRange['start'], propRange['end'], propRange['comment']))
            self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
        self.impl.write("} // end namespace tables\n\n")

        # write out test function
        self.impl.write("bool contains(General_Category _cat, char32_t _codepoint) noexcept {\n")
        self.impl.write("    switch (_cat) {\n")
        for name in sorted(cats.keys()):
            self.impl.write("        case General_Category::{0:}: return contains(tables::{0:}, _codepoint);\n".format(name))
        self.impl.write("        case General_Category::{0:}: return false;\n".format('Unspecified')) # special case
        self.impl.write("    }\n")
        self.impl.write("    return false;\n")
        self.impl.write("}\n\n")

        # write enums / signature
        self.header.write("enum class General_Category {\n")
        self.header.write("    {},\n".format('Unspecified')) # special case for general purpose get() function
        for name in sorted(cats.keys()):
            self.header.write("    {},\n".format(name))
        self.header.write("};\n\n")

        self.header.write("bool contains(General_Category _cat, char32_t _codepoint) noexcept;\n\n")

        self.header.write('namespace general_category {\n')
        self.header.write("    {} get(char32_t _value) noexcept;\n\n".format(type_name))
        for name in sorted(cats.keys()):
            self.header.write(
                    '    inline bool {}(char32_t _codepoint) {{ return contains(General_Category::{}, _codepoint); }}\n'.
                    format(name.lower(), name))
        self.header.write('}\n\n') # }}}

    def collect_range_table_with_prop(self, f): # {{{
        table = []
        while True:
            line = f.readline()
            if not line:
                break
            m = self.singleValueRE.match(line)
            if m:
                code = int(m.group(1), 16)
                prop = m.group(2)
                comment = m.group(3)
                table.append({'start': code, 'end': code, 'property': prop, 'comment': comment})
            m = self.rangeValueRE.match(line)
            if m:
                start = int(m.group(1), 16)
                end = int(m.group(2), 16)
                prop = m.group(3)
                comment = m.group(4)
                table.append({'start': start, 'end': end, 'property': prop, 'comment': comment})
        table.sort(key = lambda a: a['start'])
        return table # }}}

    def process_east_asian_width(self): # {{{
        WIDTH_NAMES = {
            'A': "Ambiguous",
            'F': "FullWidth",
            'H': 'HalfWidth',
            'N': 'Neutral',
            'Na': 'Narrow',
            'W': "Wide",
            '?': "Unspecified", # manually added (not part of the UCD spec)
        }
        type_name = 'EastAsianWidth'
        table_name = type_name
        prop_type = '::unicode::{}'.format(type_name)

        with open(self.ucd_dir + '/EastAsianWidth.txt') as f:
            table = self.collect_range_table_with_prop(f)

            # api: enum
            self.header.write('enum class {} {{\n'.format(table_name))
            for v in WIDTH_NAMES.values():
                self.header.write('    {},\n'.format(v))
            self.header.write('};\n\n')

            # api: enum to_string
            self.header.write('inline std::string to_string({} _value) {{\n'.format(type_name))
            self.header.write('    switch (_value) {\n')
            for v in WIDTH_NAMES.values():
                self.header.write('        case {}::{}: return "{}";\n'.format(type_name, v, v))
            self.header.write('    }\n');
            self.header.write('    return "Unknown";\n');
            self.header.write('};\n\n')

            # api: signature
            self.header.write('EastAsianWidth east_asian_width(char32_t _codepoint) noexcept;\n\n')

            # impl: range tables
            self.impl.write("namespace tables {\n")
            self.impl.write("auto constexpr {} = std::array{{ // {}\n".format(table_name, FOLD_OPEN))
            for propRange in table:
                self.impl.write("    Prop<{}>{{ {{ 0x{:>04X}, 0x{:>04X} }}, {}::{} }}, // {}\n".format(
                                prop_type,
                                propRange['start'],
                                propRange['end'],
                                prop_type,
                                WIDTH_NAMES[propRange['property']],
                                propRange['comment']))
            self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
            self.impl.write("} // end namespace tables\n\n")

            # impl: function
            self.impl.write(
                'EastAsianWidth east_asian_width(char32_t _codepoint) noexcept {\n' +
                '    if (auto const p = search(tables::EastAsianWidth, _codepoint); p.has_value())\n'
                '        return p.value();\n' +
                '    return EastAsianWidth::Unspecified;\n' +
                '}\n\n'
            )
            # }}}

def main():
    header_file = PROJECT_ROOT + '/src/unicode/ucd.h'
    impl_file = PROJECT_ROOT + '/src/unicode/ucd.cpp'
    ucdgen = UCDGenerator(UCD_DIR, header_file, impl_file)
    ucdgen.generate()
    ucdgen.close()

main()
