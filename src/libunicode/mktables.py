#! /usr/bin/env python3
"""/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2020-2021 Christian Parpart <christian@parpart.family>
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

from abc import ABC, abstractmethod
from collections import defaultdict
from sys import argv
from typing import Dict, List, Set
import os
import re
from codecs import open as codecs_open

PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__)) + '/../..'
HEADER_ROOT = PROJECT_ROOT + '/src/libunicode'
SCRIPT_MTIME = os.stat(__file__).st_mtime
PropertyValueAliases_fname = 'PropertyValueAliases.txt'
DerivedGeneralCategory_fname = '/extracted/DerivedGeneralCategory.txt'
DerivedCoreProperties_fname = 'DerivedCoreProperties.txt'
GraphemeBreakProperty_fname = '/auxiliary/GraphemeBreakProperty.txt'
Scripts_fname = 'Scripts.txt'
Blocks_fname = 'Blocks.txt'
ScriptExtensions_fname = 'ScriptExtensions.txt'
Emoji_data_fname = '/emoji/emoji-data.txt'
EastAsianWidth_fname = 'EastAsianWidth.txt'

PLANES = [
    {'plane':  0, 'start':   0x0000, 'end':  0x0FFFF, 'short':    'BMP', 'name': 'Basic Multilingual Plane'},
    {'plane':  1, 'start':  0x10000, 'end':  0x1FFFF, 'short':    'SMP', 'name': 'Supplementary Multilingual Plane'},
    {'plane':  2, 'start':  0x20000, 'end':  0x2FFFF, 'short':    'SIP', 'name': 'Supplementary Ideographic Plane'},
    {'plane':  3, 'start':  0x30000, 'end':  0x3FFFF, 'short':    'TIP', 'name': 'Tertiary Ideographic Plane'},
    {'plane':  4, 'start':  0x40000, 'end':  0x4FFFF, 'short':       '', 'name': 'Unassigned'},
    {'plane':  5, 'start':  0x50000, 'end':  0x5FFFF, 'short':       '', 'name': 'Unassigned'},
    {'plane':  6, 'start':  0x60000, 'end':  0x6FFFF, 'short':       '', 'name': 'Unassigned'},
    {'plane':  7, 'start':  0x70000, 'end':  0x7FFFF, 'short':       '', 'name': 'Unassigned'},
    {'plane':  8, 'start':  0x80000, 'end':  0x8FFFF, 'short':       '', 'name': 'Unassigned'},
    {'plane':  9, 'start':  0x90000, 'end':  0x9FFFF, 'short':       '', 'name': 'Unassigned'},
    {'plane': 10, 'start':  0xA0000, 'end':  0xAFFFF, 'short':       '', 'name': 'Unassigned'},
    {'plane': 11, 'start':  0xB0000, 'end':  0xBFFFF, 'short':       '', 'name': 'Unassigned'},
    {'plane': 12, 'start':  0xC0000, 'end':  0xCFFFF, 'short':       '', 'name': 'Unassigned'},
    {'plane': 13, 'start':  0xD0000, 'end':  0xDFFFF, 'short':       '', 'name': 'Unassigned'},
    {'plane': 14, 'start':  0xE0000, 'end':  0xEFFFF, 'short':    'SSP', 'name': 'Supplementary Special-purpose Plane'},
    {'plane': 15, 'start':  0xF0000, 'end':  0xFFFFF, 'short': 'SPUA-A', 'name': 'Supplementary Private Use Area Plane'},
    {'plane': 16, 'start': 0x100000, 'end': 0x10FFFF, 'short': 'SPUA-B', 'name': 'Supplementary Private Use Area Plane'},
]

# Unicode 15.0 database: https://www.unicode.org/Public/15.0.0/ucd/
UCD_DIR = argv[1]

FOLD_OPEN = '{{{'
FOLD_CLOSE = '}}}'

def uopen(filename):
    return codecs_open(filename, encoding = 'utf8')

def sanitize_identifier(_identifier):
    return _identifier.replace(' ', '_').replace('-', '_')

def minimal_uint(maxValue):
    if maxValue < 2 ** 8:
        return 'uint8_t'
    elif maxValue < 2 ** 16:
        return 'uint16_t'
    elif maxValue < 2 ** 32:
        return 'uint32_t'
    else:
        return 'uint64_t'

class EnumBuilder(ABC): # {{{
    @abstractmethod
    def output(self):
        """ Retrieves the output file name. """
        pass

    @abstractmethod
    def begin(self, _enum_class: str, _first_value: int = 0):
        """ starts a new enum class scope """
        pass

    @abstractmethod
    def member(self, _member: str):
        """ appends a new enum class member """
        pass

    @abstractmethod
    def end(self):
        """ finishes the current enum class scope """
        pass

    @abstractmethod
    def close(self):
        pass
    # }}}
class EnumBuilderArray(EnumBuilder): # {{{
    def __init__(self, builders: list):
        self.builders = builders

    def close(self):
        for b in self.builders:
            b.close()

    def begin(self, _enum_class, _first = 0):
        for b in self.builders:
            b.begin(_enum_class, _first)

    def member(self, _member):
        for b in self.builders:
            b.member(_member)

    def end(self):
        for b in self.builders:
            b.end()

    def output(self):
        result = []
        for b in self.builders:
            result.append(b.output())
        return result
    # }}}
class EnumClassWriter(EnumBuilder): # {{{
    def __init__(self, _header_filename: str):
        self.filename = _header_filename
        self.file = open(_header_filename, 'w', encoding='utf-8', newline='\u000A')
        self.next_value = 0

        self.file.write(globals()['__doc__'])
        self.file.write('#pragma once\n')
        self.file.write('\n')
        self.file.write('#include <cstdint>\n')
        self.file.write('\n')
        self.file.write('namespace unicode\n{\n\n')

    def begin(self, _enum_class, _first_value):
        self.enum_class = _enum_class
        self.next_value = _first_value
        self.collected_enum_values = []

    def member(self, _member):
        self.collected_enum_values.append(_member)

    def end(self):
        self.file.write('enum class {}: {}\n{{\n'.format(self.enum_class, minimal_uint(len(self.collected_enum_values))))
        for name in self.collected_enum_values:
            self.file.write('    {0} = {1},\n'.format(sanitize_identifier(name), self.next_value))
            self.next_value += 1
        self.file.write("};\n\n")

    def output(self):
        return self.filename

    def close(self):
        self.file.write("} // namespace unicode\n")
        self.file.close()
    # }}}
class EnumOstreamWriter(EnumBuilder): # {{{
    def __init__(self, _header_filename: str):
        self.filename = _header_filename
        self.names = []
        self.file = open(_header_filename, 'w', encoding='utf-8', newline='\u000A')

        self.file.write(globals()['__doc__'])
        self.file.write('#pragma once\n')
        self.file.write('\n')
        self.file.write('#include <libunicode/ucd.h>\n')
        self.file.write('\n')
        self.file.write('#include <ostream>\n')
        self.file.write('\n')
        self.file.write('namespace unicode\n{\n\n')

    def begin(self, _enum_class, _first):
        self.enum_class = _enum_class
        self.file.write('inline std::ostream& operator<<(std::ostream& os, {} value) noexcept\n{{\n'.format(_enum_class))
        self.names.append(_enum_class);
        self.file.write('    // clang-format off\n')
        self.file.write('    switch (value)\n')
        self.file.write('    {\n')

    def member(self, _member):
        self.file.write('    case {0}::{1}: return os << "{2}";\n'.format(self.enum_class, sanitize_identifier(_member), _member))
        return

    def end(self):
        self.file.write("    }\n")
        self.file.write('    // clang-format on\n')
        self.file.write('    return os << "(" << static_cast<unsigned>(value) << ")";\n')
        self.file.write("}\n\n")
        pass

    def output(self):
        return self.filename

    def close(self):
        self.file.write("} // namespace unicode\n")
        self.file.close()
    # }}}
class EnumFmtWriter(EnumBuilder): # {{{
    def __init__(self, _header_filename: str):
        self.filename = _header_filename
        self.file = open(_header_filename, 'w', encoding='utf-8', newline='\u000A')

        self.file.write(globals()['__doc__'])
        self.file.write('#pragma once\n')
        self.file.write('\n')
        self.file.write('#include <libunicode/ucd_enums.h>\n')
        self.file.write('\n')
        self.file.write('#include <format>\n')
        self.file.write('\n')

    def begin(self, _enum_class, _first):
        self.enum_class = 'unicode::' + _enum_class
        self.file.write('template <>\n')
        self.file.write('struct std::formatter<{}>: std::formatter<std::string_view>\n{{\n'.format(self.enum_class))
        self.file.write('    auto format({} value, auto& ctx) const\n'.format(self.enum_class))
        self.file.write('    {\n')
        self.file.write('        std::string_view name;\n')
        self.file.write('        switch (value)\n')
        self.file.write('        {\n')
        self.file.write('            // clang-format off\n')

    def member(self, _member):
        self.file.write(
            '            case {0}::{1}: name = "{2}"; break;\n'.format(
            self.enum_class, sanitize_identifier(_member), _member)
        )

    def end(self):
        self.file.write('            // clang-format off\n')
        self.file.write("        }\n")
        self.file.write('        return formatter<string_view>::format(name, ctx);\n')
        self.file.write("    }\n")
        self.file.write("};\n\n")

    def output(self):
        return self.filename

    def close(self):
        self.file.close()
    # }}}
class MergedRange: # {{{
    def __init__(self, _from, _to, _value, _comments, _count):
        self.range_from = _from
        self.range_to = _to
        self.value = _value
        self.comments = _comments
        self.count = _count
# }}}
class RangeTableGenerator: # {{{
    def __init__(self):
        self.range_from = 0
        self.range_to = 0
        self.value = ""
        self.comments = []
        self.count = 0
        self.result = []

    def flush(self):
        # Do not flush on initial state
        if self.value != '':
            self.result.append(MergedRange(self.range_from,
                                           self.range_to,
                                           self.value,
                                           self.comments,
                                           self.count))
        self.value = ''
        self.range_from = 0
        self.range_to = 0
        self.comments = []
        self.count = 0

    def append(self, _from, _to, _value, _comment):
        RANGE_COMMENT_FMT = '0x{:>04X} .. 0x{:>04X}: {}'
        if self.range_to + 1 == _from and self.value == _value:
            # This is a direct upper neighbor of the current range.
            if len(self.comments) == 1:
                # We are adding a second range, so modify the first range's comment.
                self.comments[0] = RANGE_COMMENT_FMT.format(self.range_from,
                                                            self.range_to,
                                                            self.comments[0])
            if len(self.comments) != 0:
                _comment = RANGE_COMMENT_FMT.format(_from, _to, _comment)
            self.range_to = _to
        else:
            self.flush()
            self.range_from = _from
            self.range_to = _to
            self.value = _value
        self.comments.append(_comment)
        self.count += 1
# }}}
class UCDGenerator: # {{{
    def __init__(self, _ucd_dir, _header_file, _impl_file):
        self.ucd_dir = _ucd_dir
        self.header_filename = _header_file
        self.impl_filename = _impl_file
        self.header = open(_header_file, 'w', encoding='utf-8', newline='\u000A')
        self.impl = open(_impl_file, 'w', encoding='utf-8', newline='\u000A')

        self.singleValueRE = re.compile(r'([0-9A-F]+)\s*;\s*(\w+)\s*#\s*(.*)$')
        self.rangeValueRE = re.compile(r'([0-9A-F]+)\.\.([0-9A-F]+)\s*;\s*(\w+)\s*#\s*(.*)$')
        self.singleValueMultiRE = re.compile(r'([0-9A-F]+)\s*;\s*([\w\s]+)#\s*(.*)$')
        self.rangeValueMultiRE = re.compile(r'([0-9A-F]+)\.\.([0-9A-F]+)\s*;\s*([\w\s]+)#\s*(.*)$')

        self.general_category_map = dict()
        self.general_category = list()
        self.blocks = list()

        self.builder = EnumBuilderArray([ # TODO: rename to enum_builder
            EnumClassWriter(HEADER_ROOT + '/ucd_enums.h'),
            EnumOstreamWriter(HEADER_ROOT + '/ucd_ostream.h'),
            EnumFmtWriter(HEADER_ROOT + '/ucd_fmt.h')
        ])

    def generate(self):
        self.load_property_value_aliases()
        self.load_general_category()
        self.load_core_properties()
        self.load_scripts()
        self.load_script_extensions()
        self.load_blocks()

        self.file_header()
        self.write_planes()
        self.write_properties()
        self.write_core_properties()
        self.write_general_categories()
        self.write_scripts()
        self.write_script_extensions()
        self.write_blocks()

        self.process_grapheme_break_props()
        self.process_east_asian_width()
        self.process_emoji_props()

        self.load_bidi_mirrored()
        self.load_bidi_mirroring_glyph()
        self.write_bidi_mirroring()

        self.file_footer()

    def close(self):
        self.builder.close()
        self.header.close()
        self.impl.close()

    def file_header(self): # {{{
        self.header.write(globals()['__doc__'])
        self.header.write("""#pragma once

#include <libunicode/ucd_enums.h>

#include <string>
#include <utility>

namespace unicode
{

""")

        self.impl.write(globals()['__doc__'])
        self.impl.write("""
#include <libunicode/ucd.h>
#include <libunicode/ucd_private.h>

#include <algorithm>
#include <array>

namespace unicode
{

""") # }}}

    def file_footer(self): # {{{
        self.header.write("} // namespace unicode\n")
        self.impl.write("} // namespace unicode\n")
# }}}

    def load_property_value_aliases(self): # {{{
        black_list = set()
        black_list.add('Block')

        with uopen(self.ucd_dir + '/' + PropertyValueAliases_fname) as f:
            # gc ; C   ; Other    # Cc | Cf | Cn | Co | Cs
            headerRE = re.compile(r'^#\s*(\w+) \((\w+)\)$')
            lineRE = re.compile(r'^(\w+)\s*;\s*([a-zA-Z0-9_\.]+)\s*;\s*([a-zA-Z0-9_]+).*$')
            property_values = dict()

            while True:
                line = f.readline()
                if not line:
                    break
                m = headerRE.match(line)
                if m:
                    name = m.group(1)
                    name_abbrev = m.group(2)
                    if not name in black_list:
                        property_values[name] = dict()
                    continue
                # TODO: treat Canonical_Combining_Class differently (see .txt file)
                m = lineRE.match(line)
                if m:
                    a = m.group(1)
                    value_abbrev = m.group(2)
                    value = m.group(3)
                    if not name in black_list:
                        property_values[name][value_abbrev] = value

            self.property_values = property_values
        # }}}

    def write_planes(self): # {{{
        names = set()
        for plane in PLANES:
            names.add(plane['name'])

        self.builder.begin('Plane')
        for name in sorted(names):
            self.builder.member(sanitize_identifier(name))
        self.builder.end()

        element_type = 'Prop<::unicode::{}>'.format('Plane')
        self.table_prop_start('Plane', 'Plane', len(PLANES))
        for plane in PLANES:
            self.table_prop_element(
                element_type,
                plane['start'],
                plane['end'],
                'Plane',
                sanitize_identifier(plane['name']),
                'Plane {} {}'.format(plane['plane'], plane['short'])
            )
        self.table_prop_end()
        self.table_prop_mapping('Plane', 'Unassigned')
        # }}})

    # {{{ array<Prop<Key>, N> writer
    def table_prop_mapping(self, _name, _default):
        self.header.write('{} {}(char32_t codepoint) noexcept;\n\n'.format(_name, _name.lower()))
        self.impl.write('{} {}(char32_t codepoint) noexcept {{\n'.format(_name, _name.lower()))
        self.impl.write('    return search(tables::{0}, codepoint).value_or({0}::{1});\n'.format(_name, _default))
        self.impl.write('}\n\n')

    def table_prop_start(self, _element_type, _name, _count):
        element_type = 'Prop<::unicode::{}>'.format(_element_type)
        self.impl.write('namespace tables\n{\n')
        self.impl.write('    // clang-format off\n')
        self.impl.write("    auto static const {} = std::array<{}, {}>{{ // {}\n".format(
            _name,
            element_type,
            _count,
            FOLD_OPEN))
        pass

    def table_prop_element(self, _element_type, _start, _end, _name, _prop, _comment = ''):
        self.impl.write('        {} {{ {{ 0x{:>04X}, 0x{:>04X} }}, unicode::{}::{} }},'.format(
                        _element_type,
                        _start,
                        _end,
                        _name,
                        _prop))
        if _comment != '':
            self.impl.write(' // {}'.format(_comment.strip()))
        self.impl.write('\n')
        pass

    def table_prop_end(self):
        self.impl.write("    }}; // {}\n".format(FOLD_CLOSE))
        self.impl.write('    // clang-format off\n')
        self.impl.write("} // namespace tables\n\n")
    # }}}

    def write_properties(self): # {{{
        for name in sorted(self.property_values.keys()):
            if len(self.property_values[name].keys()) == 0:
                continue

            # Do not create enums for primitive boolean classes.
            if (len(self.property_values[name].keys()) == 2
                    and ('Yes' in self.property_values[name].values())
                    and ('No'  in self.property_values[name].values())):
                continue

            # Filter some properties.
            if name in ['Script', 'General_Category', 'EastAsianWidth']:
                # XXX those properties are generated somewhere else.
                continue

            # Construct member valus
            values = list()
            for key in self.property_values[name].keys():
                values.append(self.property_values[name][key])
            values.sort()

            # build enum class
            self.builder.begin(name)
            if name in ['Grapheme_Cluster_Break']:
                self.builder.member('Undefined')
            for value in values:
                self.builder.member(value)
            self.builder.end()
        # }}}

    def load_general_category(self): # {{{
        with uopen(self.ucd_dir + DerivedGeneralCategory_fname) as f:
            headerRE = re.compile(r'^#\s*General_Category=(\w+)$')
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

    def load_core_properties(self): # {{{
        with uopen(self.ucd_dir + '/' + DerivedCoreProperties_fname) as f:
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
        self.builder.begin('Core_Property')
        for name in sorted(props.keys()):
            self.builder.member(name)
            self.impl.write("auto static const {} = std::array<{}, {}>{{ // {}\n".format(
                name,
                'Interval',
                len(props[name]),
                FOLD_OPEN))
            for propRange in props[name]:
                self.impl.write("    Interval{{ 0x{:>04X}, 0x{:>04X} }}, // {}\n".format(propRange['start'], propRange['end'], propRange['comment']))
            self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
        self.builder.end()
        self.impl.write("} // end namespace tables\n\n")

        # write out test function
        self.impl.write("bool contains(Core_Property prop, char32_t codepoint) noexcept {\n")
        self.impl.write("    switch (prop)\n")
        self.impl.write("    {\n")
        for name in sorted(props.keys()):
            self.impl.write("        case Core_Property::{0:}: return contains(tables::{0:}, codepoint);\n".format(name))
        self.impl.write("    }\n")
        self.impl.write("    return false;\n")
        self.impl.write("}\n\n")

        # API: write enum and tester
        self.header.write("bool contains(Core_Property prop, char32_t codepoint) noexcept;\n\n")
        # }}}

    def load_generic_properties(self, filename): # {{{
        with uopen(filename) as f:
            props = list()
            while True:
                line = f.readline()
                if not line:
                    break
                m = self.singleValueRE.match(line)
                if m:
                    code = int(m.group(1), 16)
                    prop = m.group(2)
                    comment = m.group(3)
                    props.append({'start': code, 'end': code, 'property': prop, 'comment': comment})
                m = self.rangeValueRE.match(line)
                if m:
                    start = int(m.group(1), 16)
                    end = int(m.group(2), 16)
                    prop = m.group(3)
                    comment = m.group(4)
                    props.append({'start': start, 'end': end, 'property': prop, 'comment': comment})

            # sort table
            props.sort(key = lambda a: a['start'])

            return props
        # }}}

    def load_properties(self, filename: str, prop_key: str): # {{{
        with uopen(filename) as f:
            headerRE = re.compile(r'^#\s*{}:\s*(\w+)$'.format(prop_key))

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

            return props
        # }}}

    def process_props(self, filename: str, prop_key: str): # {{{
        props = self.load_properties(filename, prop_key)
        self.write_tables_and_functions(props)
        # }}}

    def write_tables_and_functions(self, props): # {{{
        # write range tables
        self.impl.write("namespace tables {\n")
        for name in sorted(props.keys()):
            element_type = 'Prop<::unicode::{}>'.format(name)
            self.impl.write("auto static const {} = std::array<{}, {}>{{ // {}\n".format(
                name,
                element_type,
                len(props[name]),
                FOLD_OPEN))
            for propRange in props[name]:
                self.impl.write("    {} {{ {{ 0x{:>04X}, 0x{:>04X} }}, ::unicode::{}::{} }}, // {}\n".format(
                           element_type,
                           propRange['start'],
                           propRange['end'],
                           name,
                           propRange['property'],
                           propRange['comment']))
            self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
        self.impl.write("} // end namespace tables\n\n")

        for name in sorted(props.keys()):
            self.header.write(f'{name} {name.lower()}(char32_t codepoint) noexcept;\n')
            self.impl.write(f'{name} {name.lower()}(char32_t codepoint) noexcept {{\n')
            self.impl.write(f'    return search(tables::{name}, codepoint).value_or({name}::Undefined);\n')
            self.impl.write('}\n\n')
        self.header.write('\n')
        self.impl.write('\n')
        # }}}

    def process_grapheme_break_props(self):
        self.process_props(self.ucd_dir + GraphemeBreakProperty_fname, 'Property')

    def load_scripts(self):
        self.scripts = self.load_generic_properties(self.ucd_dir + '/' + Scripts_fname)

    def write_scripts(self): # {{{
        name = 'Script'
        props = self.scripts
        element_type = 'Prop<unicode::{}>'.format(name)

        self.impl.write("namespace tables {\n")
        self.impl.write("auto static const {} = std::array<{}, {}>{{ // {}\n".format(
            name,
            element_type,
            len(props),
            FOLD_OPEN))
        pset = set()
        for propRange in props:
            pset.add(propRange['property'])
            self.impl.write("    {} {{ {{ 0x{:>04X}, 0x{:>04X} }}, unicode::{}::{} }}, // {}\n".format(
                            element_type,
                            propRange['start'],
                            propRange['end'],
                            name,
                            propRange['property'],
                            propRange['comment']))

        self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
        self.impl.write("} // end namespace tables\n\n")

        self.builder.begin(name, 0)
        self.builder.member('Invalid')
        self.builder.member('Unknown')
        self.builder.member('Common')
        for p in sorted(pset):
            if p != 'Common':
                self.builder.member(p)
        self.builder.end()

        # codepoint-to-script mapping function
        self.header.write('{} {}(char32_t codepoint) noexcept;\n\n'.format(name, name.lower()))

        self.impl.write('{} {}(char32_t codepoint) noexcept {{\n'.format(name, name.lower()))
        self.impl.write('    return search(tables::{}, codepoint).value_or(Script::Unknown);\n'.format(name))
        self.impl.write('}\n\n')
        # }}}

    def load_blocks(self): # {{{
        filename = self.ucd_dir + '/' + Blocks_fname
        with uopen(filename) as f:
            line_regex = re.compile(r'^([0-9A-Fa-f]+)\.\.([0-9A-Fa-f]+);\s*(.*)$')
            blocks = list()
            while True:
                line = f.readline()
                if not line:
                    break
                m = line_regex.match(line)
                if m:
                    range_start = int(m.group(1), 16)
                    range_end = int(m.group(2), 16)
                    block_title = m.group(3)
                    blocks.append({
                        'start': range_start,
                        'end': range_end,
                        'title': block_title
                    })
            self.blocks = blocks
        return
    # }}}

    def load_script_extensions(self): # {{{
        filename = self.ucd_dir + '/' + ScriptExtensions_fname
        with uopen(filename) as f:
            props = list()
            while True:
                line = f.readline()
                if not line:
                    break
                m = self.singleValueMultiRE.match(line)
                if m:
                    code = int(m.group(1), 16)
                    prop = sorted(m.group(2).strip().split(' '))
                    comment = m.group(3)
                    props.append({'start': code, 'end': code, 'property': prop, 'comment': comment})
                m = self.rangeValueMultiRE.match(line)
                if m:
                    start = int(m.group(1), 16)
                    end = int(m.group(2), 16)
                    prop = sorted(m.group(3).strip().split(' '))
                    comment = m.group(4)
                    props.append({'start': start, 'end': end, 'property': prop, 'comment': comment})

            # sort table
            props.sort(key = lambda a: a['start'])

            self.script_extensions = props
        # }}}

    def write_script_extensions(self): # {{{
        self.impl.write("namespace tables {{ // {} ScriptExtensions\n".format(FOLD_OPEN))
        # construct indirected lists
        done_list = list()
        for sce in self.script_extensions:
            scripts = sce['property']
            key = 'sce_{}'.format('_'.join(scripts))
            if key in done_list:
                continue
            done_list.append(key)
            self.impl.write('auto static const {} = std::array<{}, {}>{{\n'.format(
                            key,
                            'unicode::Script',
                            len(scripts)))
            for script_abbrev in scripts:
                script = self.property_values['Script'][script_abbrev]
                self.impl.write('    unicode::Script::{},\n'.format(script))
            self.impl.write('};\n\n')

        # construct main lookup table
        element_type = 'Prop<std::pair<unicode::Script const*, std::size_t>>'
        self.impl.write('static const std::array<{}, {}> {} {{ {{\n'.format(
                        element_type,
                        len(self.script_extensions),
                        'sce'))
        for sce in self.script_extensions:
            scripts = sce['property']
            key = 'sce_{}'.format('_'.join(scripts))
            self.impl.write("    {} {{ {{ 0x{:>04X}, 0x{:>04X} }}, {} }}, // {}\n".format(
                            element_type,
                            sce['start'],
                            sce['end'],
                            '{{ {0}.data(), {0}.size() }}'.format(key),
                            sce['comment']))
        self.impl.write('} };\n')
        self.impl.write("}} // {}\n\n".format(FOLD_CLOSE))

        # getter function
        self.header.write("size_t script_extensions(char32_t codepoint, Script* result, size_t capacity) noexcept;\n\n")
        self.impl.write("size_t script_extensions(char32_t codepoint, Script* result, size_t capacity) noexcept {\n")
        self.impl.write("    auto const p = search(tables::{}, codepoint);\n".format('sce'))
        self.impl.write("    if (!p.has_value()) {{\n".format('sce'))
        self.impl.write('        *result = script(codepoint);\n')
        self.impl.write('        return 1;\n')
        self.impl.write('    }\n')
        self.impl.write('    auto const cap = std::min(capacity, p.value().second);\n')
        self.impl.write('    for (size_t i = 0; i < cap; ++i)\n')
        self.impl.write('        result[i] = p->first[i];\n')
        self.impl.write('    return cap;\n')
        self.impl.write("}\n\n")
    # }}}

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
        with uopen(self.ucd_dir + Emoji_data_fname) as f:
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
                self.impl.write("auto static const {} = std::array<{}, {}>{{ // {}\n".format(
                    name,
                    'Interval',
                    len(props[name]),
                    FOLD_OPEN
                ))
                for propRange in props[name]:
                    self.impl.write("    Interval{{ 0x{:>04X}, 0x{:>04X} }}, // {}\n".format(
                               propRange['start'],
                               propRange['end'],
                               propRange['comment']))
                self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
            self.impl.write("} // end namespace tables\n\n")

            # write out test function
            for name in sorted(props.keys()):
                self.impl.write('bool is_{}(char32_t codepoint) noexcept {{\n'.format(name.lower()))
                self.impl.write("    return contains(tables::{0:}, codepoint);\n".format(name))
                self.impl.write("}\n\n")

            # write enums / signature
            for name in sorted(props.keys()):
                self.header.write('bool is_{}(char32_t codepoint) noexcept;\n'.format(name.lower()))
            self.header.write('\n')
        # }}}

    def load_bidi_mirrored(self): # {{{
        """Parse UnicodeData.txt field 9 to collect all codepoints with Bidi_Mirrored=Y."""
        filepath = os.path.join(self.ucd_dir, 'UnicodeData.txt')
        mirrored_cps = []
        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                fields = line.split(';')
                if len(fields) < 10:
                    continue
                if fields[9].strip() == 'Y':
                    mirrored_cps.append(int(fields[0], 16))
        mirrored_cps.sort()

        # Compress into intervals
        intervals = []
        for cp in mirrored_cps:
            if intervals and intervals[-1][1] + 1 == cp:
                intervals[-1] = (intervals[-1][0], cp)
            else:
                intervals.append((cp, cp))
        self.bidi_mirrored_intervals = intervals
        # }}}

    def load_bidi_mirroring_glyph(self): # {{{
        """Parse BidiMirroring.txt to collect (source, target) pairs."""
        filepath = os.path.join(self.ucd_dir, 'BidiMirroring.txt')
        pairs = []
        lineRE = re.compile(r'^([0-9A-F]+)\s*;\s*([0-9A-F]+)\s*#\s*(.*)$')
        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                m = lineRE.match(line)
                if m:
                    source = int(m.group(1), 16)
                    target = int(m.group(2), 16)
                    pairs.append((source, target))
        pairs.sort()
        self.bidi_mirroring_glyph_pairs = pairs
        # }}}

    def write_bidi_mirroring(self): # {{{
        """Generate bidi mirroring tables and functions into ucd.h and ucd.cpp."""
        intervals = self.bidi_mirrored_intervals
        pairs = self.bidi_mirroring_glyph_pairs

        # --- Write tables to ucd.cpp ---
        self.impl.write("namespace tables {\n")

        # Bidi_Mirrored interval table
        self.impl.write("auto static const Bidi_Mirrored = std::array<Interval, {}>{{ // {}\n".format(
            len(intervals), FOLD_OPEN))
        for (start, end) in intervals:
            self.impl.write("    Interval{{ 0x{:>04X}, 0x{:>04X} }},\n".format(start, end))
        self.impl.write("}}; // {}\n".format(FOLD_CLOSE))

        # Bidi_Mirroring_Glyph pair table
        self.impl.write("auto static const Bidi_Mirroring_Glyph = std::array<std::pair<char32_t, char32_t>, {}>{{ // {}\n".format(
            len(pairs), FOLD_OPEN))
        for (source, target) in pairs:
            self.impl.write("    std::pair<char32_t, char32_t>{{ 0x{:>04X}, 0x{:>04X} }},\n".format(source, target))
        self.impl.write("}}; // {}\n".format(FOLD_CLOSE))

        self.impl.write("} // end namespace tables\n\n")

        # --- Write function implementations to ucd.cpp ---
        self.impl.write("bool is_mirrored(char32_t codepoint) noexcept {\n")
        self.impl.write("    return contains(tables::Bidi_Mirrored, codepoint);\n")
        self.impl.write("}\n\n")

        self.impl.write("char32_t bidi_mirroring_glyph(char32_t codepoint) noexcept {\n")
        self.impl.write("    auto const it = std::lower_bound(\n")
        self.impl.write("        tables::Bidi_Mirroring_Glyph.begin(),\n")
        self.impl.write("        tables::Bidi_Mirroring_Glyph.end(),\n")
        self.impl.write("        codepoint,\n")
        self.impl.write("        [](auto const& pair, char32_t cp) { return pair.first < cp; });\n")
        self.impl.write("    if (it != tables::Bidi_Mirroring_Glyph.end() && it->first == codepoint)\n")
        self.impl.write("        return it->second;\n")
        self.impl.write("    return codepoint;\n")
        self.impl.write("}\n\n")

        # --- Write function declarations to ucd.h ---
        self.header.write("/// Returns true if the codepoint has the Bidi_Mirrored property.\n")
        self.header.write("[[nodiscard]] bool is_mirrored(char32_t codepoint) noexcept;\n\n")
        self.header.write("/// Returns the Bidi_Mirroring_Glyph for a codepoint.\n")
        self.header.write("/// If no mirroring glyph exists, returns the input codepoint unchanged.\n")
        self.header.write("[[nodiscard]] char32_t bidi_mirroring_glyph(char32_t codepoint) noexcept;\n\n")
        # }}}

    def write_blocks(self): # {{{
        UNSPECIFIED = 'Unspecified'
        element_type = 'Prop<{}>'.format('::unicode::Block')

        # Construct enum class:
        block_titles = set()
        for block in self.blocks:
            block_titles.add(block['title'])
        self.builder.begin('Block')
        self.builder.member(UNSPECIFIED)
        for block_title in sorted(block_titles):
            self.builder.member(sanitize_identifier(block_title))
        self.builder.end()

        # Constract Table definition for associating codepoint ranges with a block:
        self.impl.write("namespace tables {\n")
        self.impl.write("auto static const {} = std::array<{}, {}>{{ // {}\n".format(
            'Block',
            element_type,
            len(self.blocks),
            FOLD_OPEN))
        for block in self.blocks:
            self.impl.write(
                '    {} {{ {{ 0x{:>04X}, 0x{:>04X} }}, {}::{} }},\n'.format(
                element_type,
                block['start'],
                block['end'],
                '::unicode::Block',
                sanitize_identifier(block['title'])))
        self.impl.write("};\n") # close table
        self.impl.write("} // end namespace tables {}\n\n")

        # write out search function
        self.impl.write("Block block(char32_t codepoint) noexcept {\n")
        self.impl.write("    return search(tables::Block, codepoint).value_or(::unicode::Block::Unspecified);\n")
        self.impl.write("}\n\n")

        self.header.write("Block block(char32_t codepoint) noexcept;\n\n")

        # }}}

    def write_general_categories(self): # {{{
        UNSPECIFIED = 'Unspecified'
        gcats = self.general_category

        self.impl.write("namespace tables {\n")
        type_name = "General_Category"
        fqdn_type_name = "::unicode::{}".format(type_name)
        element_type = 'Prop<{}>'.format(fqdn_type_name)
        self.impl.write("auto const {} = std::array<{}, {}>{{\n".format(
            type_name,
            element_type,
            len(gcats)
        ))
        cats = set()
        for cat in gcats:
            cats.add(cat['property'])
            self.impl.write(
                "    {} {{ {{ 0x{:>04X}, 0x{:>04X} }}, {}::{} }}, // {}\n".format(
                element_type,
                cat['start'],
                cat['end'],
                fqdn_type_name,
                cat['property'],
                cat['comment']))
        self.impl.write("};\n")
        self.impl.write("} // end namespace tables\n\n")

        # builder
        self.builder.begin('General_Category')
        self.builder.member(UNSPECIFIED)
        for cat in sorted(cats):
            self.builder.member(cat)
        self.builder.end()

        # getter impl
        self.impl.write("namespace {}\n".format(type_name.lower()))
        self.impl.write("{\n")
        self.impl.write("    {} get(char32_t value) noexcept {{\n".format(type_name))
        self.impl.write("        return search(tables::{}, value).value_or({}::{});\n".format(type_name, type_name, UNSPECIFIED))
        self.impl.write("    }\n")
        self.impl.write("}\n\n")
        # -----------------------------------------------------------------------------------------------

        cats = self.general_category_map

        # write range tables
        self.impl.write("namespace tables {\n")
        for name in sorted(cats.keys()):
            self.impl.write("auto static const {} = std::array<{}, {}>{{ // {}\n".format(
                name,
                'Interval',
                len(cats[name]),
                FOLD_OPEN
            ))
            for propRange in cats[name]:
                self.impl.write("    Interval{{ 0x{:>04X}, 0x{:>04X} }}, // {}\n".format(propRange['start'], propRange['end'], propRange['comment']))
            self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
        self.impl.write("} // end namespace tables\n\n")

        # write out test function
        self.impl.write("bool contains(General_Category generalCategory, char32_t codepoint) noexcept {\n")
        self.impl.write("    switch (generalCategory)\n")
        self.impl.write("    {\n")
        for name in sorted(cats.keys()):
            self.impl.write("        case General_Category::{0:}: return contains(tables::{0:}, codepoint);\n".format(name))
        self.impl.write("        case General_Category::{0:}: return false;\n".format(UNSPECIFIED)) # special case
        self.impl.write("    }\n")
        self.impl.write("    return false;\n")
        self.impl.write("}\n\n")

        # write signature
        self.header.write("bool contains(General_Category generalCategory, char32_t codepoint) noexcept;\n\n")

        self.header.write('// Disabling clang-format to avoid single-line folding implementations.\n')
        self.header.write('// clang-format off\n')
        self.header.write('namespace general_category\n')
        self.header.write('{\n')
        self.header.write("    {} get(char32_t value) noexcept;\n\n".format(type_name))
        for name in sorted(cats.keys()):
            self.header.write(
                    '    inline bool is_{}(char32_t codepoint) noexcept\n'
                    '    {{\n'
                    '        return contains(General_Category::{}, codepoint);\n'
                    '    }}\n\n'.
                    format(name.lower(), name))
        self.header.write('} // namespace general_category\n') # }}}
        self.header.write('// clang-format on\n')
        self.header.write('\n')

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

    def range_table_merge_siblings(self, _table):
        m = RangeTableGenerator()
        for propRange in _table:
            m.append(int(propRange['start']),
                     int(propRange['end']),
                     propRange['property'],
                     propRange['comment'])
        m.flush()
        return m.result

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

        # TODO: Merge sequentially directly connected neighbors that have the same value

        with uopen(self.ucd_dir + '/' + EastAsianWidth_fname) as f:
            table = self.collect_range_table_with_prop(f)
            compact_ranges = self.range_table_merge_siblings(table)

            # api: enum
            self.builder.begin(table_name)
            for v in WIDTH_NAMES.values():
                self.builder.member(v)
            self.builder.end()

            # api: enum to_string
            self.header.write('inline std::string to_string({} value)\n'.format(type_name))
            self.header.write('{\n')
            self.header.write('    switch (value)\n')
            self.header.write('    {\n')
            for v in WIDTH_NAMES.values():
                self.header.write('        case {}::{}: return "{}";\n'.format(type_name, v, v))
            self.header.write('    }\n');
            self.header.write('    return "Unknown";\n');
            self.header.write('}\n\n')

            # api: signature
            self.header.write('EastAsianWidth east_asian_width(char32_t codepoint) noexcept;\n\n')

            # impl: range tables
            self.impl.write("namespace tables {\n")
            element_type = 'Prop<{}>'.format(prop_type)
            self.impl.write("auto static const {} = std::array<{}, {}>{{ // {}\n".format(
                table_name,
                element_type,
                len(compact_ranges),
                FOLD_OPEN
            ))
            for range in compact_ranges:
                if len(range.comments) > 1:
                    for comment in range.comments:
                        self.impl.write("    // {}\n".format(comment))
                self.impl.write("    {} {{ {{ 0x{:>04X}, 0x{:>04X} }}, {}::{} }},".format(
                                element_type,
                                range.range_from,
                                range.range_to,
                                prop_type,
                                WIDTH_NAMES[range.value]))
                if range.count == 1 and len(range.comments) == 1:
                    self.impl.write(" // {}".format(range.comments[0]))
                elif range.count > 1:
                    self.impl.write(" // #{}".format(range.count))
                self.impl.write('\n')
            self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
            self.impl.write("} // end namespace tables\n\n")

            # impl: function
            self.impl.write(
                'EastAsianWidth east_asian_width(char32_t codepoint) noexcept {\n' +
                '    return search(tables::EastAsianWidth, codepoint).value_or(EastAsianWidth::Unspecified);\n'
                '}\n\n'
            )
            # }}}
# }}}

# ============================================================================
# Case Mapping and Normalization Data Generation
# ============================================================================

# UCD file names for case mapping and normalization
CASE_FOLDING_FNAME = 'CaseFolding.txt'
SPECIAL_CASING_FNAME = 'SpecialCasing.txt'
DERIVED_NORMALIZATION_PROPS_FNAME = 'DerivedNormalizationProps.txt'
COMPOSITION_EXCLUSIONS_FNAME = 'CompositionExclusions.txt'
UNICODE_DATA_FNAME = 'UnicodeData.txt'


def parse_codepoint(s: str) -> int:
    """Parse a hex codepoint string."""
    return int(s.strip(), 16)


def parse_codepoints(s: str) -> List[int]:
    """Parse a space-separated list of hex codepoints."""
    if not s.strip():
        return []
    return [int(cp, 16) for cp in s.split()]


class SimpleCaseMapping:
    """Simple 1:1 case mapping."""
    def __init__(self, source: int, target: int):
        self.source = source
        self.target = target


class FullCaseMapping:
    """Full case mapping (may be 1:many)."""
    def __init__(self, source: int, targets: List[int]):
        self.source = source
        self.targets = targets


class Decomposition:
    """Unicode decomposition mapping."""
    def __init__(self, source: int, targets: List[int], decomp_type: str):
        self.source = source
        self.targets = targets
        self.type = decomp_type  # 'canonical' or compatibility type


class CaseNormalizationParser:
    """Parser for Unicode Character Database files for case mapping and normalization."""

    def __init__(self, ucd_dir: str):
        self.ucd_dir = ucd_dir

        # Simple case mappings from UnicodeData.txt (fields 12, 13, 14)
        self.simple_uppercase: Dict[int, int] = {}
        self.simple_lowercase: Dict[int, int] = {}
        self.simple_titlecase: Dict[int, int] = {}

        # Full case mappings from SpecialCasing.txt
        self.full_uppercase: Dict[int, List[int]] = {}
        self.full_lowercase: Dict[int, List[int]] = {}
        self.full_titlecase: Dict[int, List[int]] = {}

        # Case folding from CaseFolding.txt
        self.simple_casefold: Dict[int, int] = {}  # C + S
        self.full_casefold: Dict[int, List[int]] = {}  # C + F

        # Decompositions from UnicodeData.txt (field 5)
        self.decompositions: Dict[int, Decomposition] = {}

        # Canonical Combining Class from UnicodeData.txt (field 3)
        self.ccc: Dict[int, int] = {}

        # Composition exclusions
        self.composition_exclusions: Set[int] = set()

        # Quick check properties
        self.nfc_qc_no: Set[int] = set()
        self.nfc_qc_maybe: Set[int] = set()
        self.nfkc_qc_no: Set[int] = set()
        self.nfkc_qc_maybe: Set[int] = set()
        self.nfd_qc_no: Set[int] = set()
        self.nfkd_qc_no: Set[int] = set()

    def parse_all(self):
        """Parse all relevant UCD files."""
        self._parse_unicode_data()
        self._parse_case_folding()
        self._parse_special_casing()
        self._parse_composition_exclusions()
        self._parse_derived_normalization_props()

    def _parse_unicode_data(self):
        """Parse UnicodeData.txt for simple case mappings, CCC, and decompositions."""
        filepath = os.path.join(self.ucd_dir, UNICODE_DATA_FNAME)

        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue

                fields = line.split(';')
                if len(fields) < 15:
                    continue

                codepoint = parse_codepoint(fields[0])

                # Field 3: Canonical Combining Class
                ccc = int(fields[3]) if fields[3] else 0
                if ccc != 0:
                    self.ccc[codepoint] = ccc

                # Field 5: Decomposition mapping
                decomp_field = fields[5].strip()
                if decomp_field:
                    self._parse_decomposition(codepoint, decomp_field)

                # Field 12: Simple Uppercase Mapping
                if fields[12].strip():
                    self.simple_uppercase[codepoint] = parse_codepoint(fields[12])

                # Field 13: Simple Lowercase Mapping
                if fields[13].strip():
                    self.simple_lowercase[codepoint] = parse_codepoint(fields[13])

                # Field 14: Simple Titlecase Mapping
                if fields[14].strip():
                    self.simple_titlecase[codepoint] = parse_codepoint(fields[14])

    def _parse_decomposition(self, codepoint: int, field: str):
        """Parse a decomposition field from UnicodeData.txt."""
        # Format: [<type>] <codepoints>
        # e.g., "<font> 0041" or "0041 0308"
        field = field.strip()
        if not field:
            return

        decomp_type = 'canonical'
        if field.startswith('<'):
            end = field.index('>')
            decomp_type = field[1:end]
            field = field[end+1:].strip()

        if field:
            targets = parse_codepoints(field)
            self.decompositions[codepoint] = Decomposition(codepoint, targets, decomp_type)

    def _parse_case_folding(self):
        """Parse CaseFolding.txt."""
        filepath = os.path.join(self.ucd_dir, CASE_FOLDING_FNAME)

        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue

                # Format: <code>; <status>; <mapping>; # <name>
                parts = line.split(';')
                if len(parts) < 3:
                    continue

                codepoint = parse_codepoint(parts[0])
                status = parts[1].strip()
                mapping = parse_codepoints(parts[2])

                if status == 'C':
                    # Common - used for both simple and full
                    if len(mapping) == 1:
                        self.simple_casefold[codepoint] = mapping[0]
                    self.full_casefold[codepoint] = mapping
                elif status == 'S':
                    # Simple - only for simple folding
                    if len(mapping) == 1:
                        self.simple_casefold[codepoint] = mapping[0]
                elif status == 'F':
                    # Full - only for full folding
                    self.full_casefold[codepoint] = mapping
                # 'T' (Turkic) is ignored for default case folding

    def _parse_special_casing(self):
        """Parse SpecialCasing.txt for full case mappings."""
        filepath = os.path.join(self.ucd_dir, SPECIAL_CASING_FNAME)

        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue

                # Format: <code>; <lower>; <title>; <upper>; (<condition_list>;)? # <comment>
                parts = line.split(';')
                if len(parts) < 4:
                    continue

                codepoint = parse_codepoint(parts[0])
                lower = parse_codepoints(parts[1])
                title = parse_codepoints(parts[2])
                upper = parse_codepoints(parts[3])

                # Check for conditions (skip conditional mappings for now)
                # Conditional mappings have more than 4 semicolon-separated fields before the comment
                comment_idx = line.find('#')
                if comment_idx != -1:
                    before_comment = line[:comment_idx]
                    field_count = before_comment.count(';')
                    if field_count > 4:
                        # This is a conditional mapping, skip it
                        continue

                # Only store non-trivial mappings (length > 1 or different from simple)
                if len(lower) > 1 or (len(lower) == 1 and lower[0] != self.simple_lowercase.get(codepoint, codepoint)):
                    self.full_lowercase[codepoint] = lower
                if len(title) > 1 or (len(title) == 1 and title[0] != self.simple_titlecase.get(codepoint, codepoint)):
                    self.full_titlecase[codepoint] = title
                if len(upper) > 1 or (len(upper) == 1 and upper[0] != self.simple_uppercase.get(codepoint, codepoint)):
                    self.full_uppercase[codepoint] = upper

    def _parse_composition_exclusions(self):
        """Parse CompositionExclusions.txt."""
        filepath = os.path.join(self.ucd_dir, COMPOSITION_EXCLUSIONS_FNAME)

        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue

                # Format: <codepoint> # <comment>
                parts = line.split('#')
                cp_part = parts[0].strip()
                if not cp_part:
                    continue

                if '..' in cp_part:
                    start, end = cp_part.split('..')
                    for cp in range(parse_codepoint(start), parse_codepoint(end) + 1):
                        self.composition_exclusions.add(cp)
                else:
                    self.composition_exclusions.add(parse_codepoint(cp_part))

    def _parse_derived_normalization_props(self):
        """Parse DerivedNormalizationProps.txt for quick check properties."""
        filepath = os.path.join(self.ucd_dir, DERIVED_NORMALIZATION_PROPS_FNAME)

        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue

                # Format: <codepoint(s)> ; <property> ; <value>? # <comment>
                parts = line.split(';')
                if len(parts) < 2:
                    continue

                cp_part = parts[0].strip()
                prop = parts[1].strip()
                value = parts[2].strip() if len(parts) > 2 else ''

                # Parse codepoint range
                codepoints = []
                if '..' in cp_part:
                    start, end = cp_part.split('..')
                    codepoints = range(parse_codepoint(start), parse_codepoint(end) + 1)
                else:
                    codepoints = [parse_codepoint(cp_part)]

                for cp in codepoints:
                    if prop == 'NFC_QC':
                        if value == 'N':
                            self.nfc_qc_no.add(cp)
                        elif value == 'M':
                            self.nfc_qc_maybe.add(cp)
                    elif prop == 'NFKC_QC':
                        if value == 'N':
                            self.nfkc_qc_no.add(cp)
                        elif value == 'M':
                            self.nfkc_qc_maybe.add(cp)
                    elif prop == 'NFD_QC':
                        if value == 'N':
                            self.nfd_qc_no.add(cp)
                    elif prop == 'NFKD_QC':
                        if value == 'N':
                            self.nfkd_qc_no.add(cp)
                    elif prop == 'Full_Composition_Exclusion':
                        self.composition_exclusions.add(cp)


def _generate_simple_case_table(mappings: Dict[int, int], name: str) -> str:
    """Generate a sorted array of simple case mappings."""
    sorted_items = sorted(mappings.items())

    lines = []
    lines.append(f"// Simple {name} mappings: source -> target")
    lines.append(f"// Total entries: {len(sorted_items)}")
    lines.append(f"inline constexpr std::array<std::pair<char32_t, char32_t>, {len(sorted_items)}> simple_{name}_table {{{{")

    for i, (src, tgt) in enumerate(sorted_items):
        comma = ',' if i < len(sorted_items) - 1 else ''
        lines.append(f"    {{ 0x{src:04X}, 0x{tgt:04X} }}{comma} // {chr(src) if 0x20 <= src < 0x7F else ''}")

    lines.append("}};")
    return '\n'.join(lines)


def _generate_full_case_table(mappings: Dict[int, List[int]], name: str) -> str:
    """Generate a sorted array of full case mappings."""
    sorted_items = sorted(mappings.items())

    lines = []
    lines.append(f"// Full {name} mappings: source -> [target1, target2, target3]")
    lines.append(f"// Total entries: {len(sorted_items)}")
    lines.append(f"struct full_{name}_entry {{")
    lines.append(f"    char32_t source;")
    lines.append(f"    char32_t targets[3];")
    lines.append(f"    uint8_t length;")
    lines.append(f"}};")
    lines.append(f"")
    lines.append(f"inline constexpr std::array<full_{name}_entry, {len(sorted_items)}> full_{name}_table {{{{")

    for i, (src, targets) in enumerate(sorted_items):
        comma = ',' if i < len(sorted_items) - 1 else ''
        padded = targets + [0] * (3 - len(targets))
        targets_str = ', '.join(f'0x{t:04X}' for t in padded)
        lines.append(f"    {{ 0x{src:04X}, {{ {targets_str} }}, {len(targets)} }}{comma}")

    lines.append("}};")
    return '\n'.join(lines)


def _generate_ccc_table(ccc_map: Dict[int, int]) -> str:
    """Generate canonical combining class lookup data."""
    sorted_items = sorted(ccc_map.items())

    lines = []
    lines.append(f"// Canonical Combining Class entries for non-zero CCC values")
    lines.append(f"// Total entries: {len(sorted_items)}")
    lines.append(f"inline constexpr std::array<std::pair<char32_t, uint8_t>, {len(sorted_items)}> ccc_table {{{{")

    for i, (cp, ccc) in enumerate(sorted_items):
        comma = ',' if i < len(sorted_items) - 1 else ''
        lines.append(f"    {{ 0x{cp:04X}, {ccc} }}{comma}")

    lines.append("}};")
    return '\n'.join(lines)


def _generate_decomposition_table(decomps: Dict[int, Decomposition]) -> str:
    """Generate decomposition mapping table."""
    # Filter to canonical decompositions only for this table
    canonical = {k: v for k, v in decomps.items() if v.type == 'canonical'}
    sorted_items = sorted(canonical.items())

    # Find max decomposition length
    max_len = max((len(d.targets) for d in canonical.values()), default=0)
    max_len = max(max_len, 4)  # Minimum 4 for alignment

    lines = []
    lines.append(f"// Canonical decomposition mappings")
    lines.append(f"// Total entries: {len(sorted_items)}, max length: {max_len}")
    lines.append(f"struct canonical_decomposition_entry {{")
    lines.append(f"    char32_t source;")
    lines.append(f"    char32_t targets[{max_len}];")
    lines.append(f"    uint8_t length;")
    lines.append(f"}};")
    lines.append(f"")
    lines.append(f"inline constexpr std::array<canonical_decomposition_entry, {len(sorted_items)}> canonical_decomposition_table {{{{")

    for i, (cp, decomp) in enumerate(sorted_items):
        comma = ',' if i < len(sorted_items) - 1 else ''
        padded = decomp.targets + [0] * (max_len - len(decomp.targets))
        targets_str = ', '.join(f'0x{t:04X}' for t in padded)
        lines.append(f"    {{ 0x{cp:04X}, {{ {targets_str} }}, {len(decomp.targets)} }}{comma}")

    lines.append("}};")
    return '\n'.join(lines)


def _generate_composition_table(decomps: Dict[int, Decomposition], exclusions: Set[int]) -> str:
    """Generate composition pairs table (reverse of canonical decomposition)."""
    # Only include canonical decompositions of length 2 that are not excluded
    compositions = []
    for cp, decomp in decomps.items():
        if decomp.type == 'canonical' and len(decomp.targets) == 2:
            if cp not in exclusions:
                compositions.append((decomp.targets[0], decomp.targets[1], cp))

    # Sort by first, then second codepoint
    compositions.sort()

    lines = []
    lines.append(f"// Canonical composition pairs (first + second -> composed)")
    lines.append(f"// Total entries: {len(compositions)}")
    lines.append(f"struct composition_pair {{")
    lines.append(f"    char32_t first;")
    lines.append(f"    char32_t second;")
    lines.append(f"    char32_t composed;")
    lines.append(f"}};")
    lines.append(f"")
    lines.append(f"inline constexpr std::array<composition_pair, {len(compositions)}> composition_table {{{{")

    for i, (first, second, composed) in enumerate(compositions):
        comma = ',' if i < len(compositions) - 1 else ''
        lines.append(f"    {{ 0x{first:04X}, 0x{second:04X}, 0x{composed:04X} }}{comma}")

    lines.append("}};")
    return '\n'.join(lines)


def _generate_quick_check_table(codepoints: Set[int], name: str) -> str:
    """Generate a sorted array of codepoints for quick check."""
    sorted_cps = sorted(codepoints)

    lines = []
    lines.append(f"// {name} codepoints")
    lines.append(f"// Total entries: {len(sorted_cps)}")
    lines.append(f"inline constexpr std::array<char32_t, {len(sorted_cps)}> {name}_table {{{{")

    for i in range(0, len(sorted_cps), 8):
        chunk = sorted_cps[i:i+8]
        line = '    ' + ', '.join(f'0x{cp:04X}' for cp in chunk)
        if i + 8 < len(sorted_cps):
            line += ','
        lines.append(line)

    lines.append("}};")
    return '\n'.join(lines)


def _generate_case_normalization_header(parser: CaseNormalizationParser) -> str:
    """Generate the complete case_normalization_data.h header file."""
    lines = []

    # Header
    lines.append(globals()['__doc__'])
    lines.append("""#pragma once

#include <array>
#include <cstdint>
#include <utility>

namespace unicode::detail
{

// clang-format off
""")

    # Simple case mappings
    lines.append(_generate_simple_case_table(parser.simple_uppercase, "uppercase"))
    lines.append("")
    lines.append(_generate_simple_case_table(parser.simple_lowercase, "lowercase"))
    lines.append("")
    lines.append(_generate_simple_case_table(parser.simple_titlecase, "titlecase"))
    lines.append("")
    lines.append(_generate_simple_case_table(parser.simple_casefold, "casefold"))
    lines.append("")

    # Full case mappings
    lines.append(_generate_full_case_table(parser.full_uppercase, "uppercase"))
    lines.append("")
    lines.append(_generate_full_case_table(parser.full_lowercase, "lowercase"))
    lines.append("")
    lines.append(_generate_full_case_table(parser.full_titlecase, "titlecase"))
    lines.append("")
    lines.append(_generate_full_case_table(parser.full_casefold, "casefold"))
    lines.append("")

    # CCC table
    lines.append(_generate_ccc_table(parser.ccc))
    lines.append("")

    # Decomposition tables
    lines.append(_generate_decomposition_table(parser.decompositions))
    lines.append("")

    # Composition table
    lines.append(_generate_composition_table(parser.decompositions, parser.composition_exclusions))
    lines.append("")

    # Quick check tables
    lines.append(_generate_quick_check_table(parser.nfc_qc_no, "nfc_qc_no"))
    lines.append("")
    lines.append(_generate_quick_check_table(parser.nfc_qc_maybe, "nfc_qc_maybe"))
    lines.append("")
    lines.append(_generate_quick_check_table(parser.nfkc_qc_no, "nfkc_qc_no"))
    lines.append("")
    lines.append(_generate_quick_check_table(parser.nfkc_qc_maybe, "nfkc_qc_maybe"))
    lines.append("")
    lines.append(_generate_quick_check_table(parser.nfd_qc_no, "nfd_qc_no"))
    lines.append("")
    lines.append(_generate_quick_check_table(parser.nfkd_qc_no, "nfkd_qc_no"))
    lines.append("")

    # Composition exclusions
    lines.append(_generate_quick_check_table(parser.composition_exclusions, "composition_exclusions"))
    lines.append("")

    # Footer
    lines.append("// clang-format on")
    lines.append("")
    lines.append("} // namespace unicode::detail")
    lines.append("")  # Ensure trailing newline

    return '\n'.join(lines)


def generate_case_normalization_tables(ucd_dir: str, output_file: str):
    """Generate case mapping and normalization data tables."""
    parser = CaseNormalizationParser(ucd_dir)
    parser.parse_all()

    header = _generate_case_normalization_header(parser)

    with open(output_file, 'w', encoding='utf-8', newline='\n') as f:
        f.write(header)


def needs_run():
    try:
        for filename in ['ucd.cpp', 'ucd.h', 'ucd_enums.h', 'ucd_ostream.h', 'case_normalization_data.h']:
            st = os.stat(HEADER_ROOT + '/' + filename)
            if st.st_mtime < SCRIPT_MTIME:
                return True
    except FileNotFoundError:
        return True
    return False

def main():
    if needs_run():
        header_file = HEADER_ROOT + '/ucd.h'
        impl_file = HEADER_ROOT + '/ucd.cpp'
        ucdgen = UCDGenerator(UCD_DIR, header_file, impl_file)
        ucdgen.generate()
        ucdgen.close()

        # Generate case mapping and normalization data
        case_norm_file = HEADER_ROOT + '/case_normalization_data.h'
        generate_case_normalization_tables(UCD_DIR, case_norm_file)
    else:
        print("Output files up-to-date.")

main()
