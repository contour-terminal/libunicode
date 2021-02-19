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

from abc import ABC, abstractmethod
from sys import argv
import os
import re
from codecs import open as codecs_open

PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__)) + '/../..'
HEADER_ROOT = PROJECT_ROOT + '/src/unicode'
SCRIPT_MTIME = os.stat(__file__).st_mtime

# unicode database (extracted zip file): https://www.unicode.org/Public/UCD/latest/ucd/
UCD_DIR = argv[1]

FOLD_OPEN = '{{{'
FOLD_CLOSE = '}}}'

def uopen(filename):
    return codecs_open(filename, encoding = 'utf8')

def sanitize_identifier(_identifier):
    return _identifier.replace(' ', '_').replace('-', '_')

class EnumBuilder(ABC): # {{{
    @abstractmethod
    def output(self):
        """ Retrieves the output file name. """
        pass

    @abstractmethod
    def begin(self, _enum_class: str):
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

    def begin(self, _enum_class):
        for b in self.builders:
            b.begin(_enum_class)

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
        self.member_count = 0

        self.file.write(globals()['__doc__'])
        self.file.write('#pragma once\n')
        self.file.write('\n')
        self.file.write('namespace unicode {\n\n')

    def begin(self, _enum_class):
        self.enum_class = _enum_class
        self.file.write('enum class {} {{\n'.format(_enum_class))

    def member(self, _member):
        self.file.write('    {0} = {1},\n'.format(sanitize_identifier(_member), self.member_count))
        self.member_count += 1

    def end(self):
        self.file.write("};\n\n")
        self.member_count = 0

    def output(self):
        return self.filename

    def close(self):
        self.file.write("} // end namespace\n")
        self.file.close()
    # }}}
class EnumOstreamWriter(EnumBuilder): # {{{
    def __init__(self, _header_filename: str):
        self.filename = _header_filename
        self.file = open(_header_filename, 'w', encoding='utf-8', newline='\u000A')

        self.file.write(globals()['__doc__'])
        self.file.write('#pragma once\n')
        self.file.write('\n')
        self.file.write('#include <ostream>\n')
        self.file.write('#include <unicode/ucd.h>\n')
        self.file.write('\n')
        self.file.write('namespace unicode {\n\n')

    def begin(self, _enum_class):
        self.enum_class = _enum_class
        self.file.write('inline std::ostream& operator<<(std::ostream& os, {} _value) noexcept {{\n'.format(_enum_class))
        self.file.write('    switch (_value) {\n')

    def member(self, _member):
        self.file.write('        case {0}::{1}: return os << "{2}";\n'.format(self.enum_class, sanitize_identifier(_member), _member))
        return

    def end(self):
        self.file.write("    }\n")
        self.file.write('    return os << "(" << static_cast<unsigned>(_value) << ")";\n')
        self.file.write("}\n\n")
        pass

    def output(self):
        return self.filename

    def close(self):
        self.file.write("} // end namespace\n")
        self.file.close()
    # }}}

class UCDGenerator:
    def __init__(self, _ucd_dir, _header_file, _impl_file):
        self.ucd_dir = _ucd_dir
        self.header_filename = _header_file
        self.impl_filename = _impl_file
        self.header = open(_header_file, 'w', encoding='utf-8', newline='\u000A')
        self.impl = open(_impl_file, 'w', encoding='utf-8', newline='\u000A')

        self.singleValueRE = re.compile('([0-9A-F]+)\s*;\s*(\w+)\s*#\s*(.*)$')
        self.rangeValueRE = re.compile('([0-9A-F]+)\.\.([0-9A-F]+)\s*;\s*(\w+)\s*#\s*(.*)$')
        self.singleValueMultiRE = re.compile('([0-9A-F]+)\s*;\s*([\w\s]+)#\s*(.*)$')
        self.rangeValueMultiRE = re.compile('([0-9A-F]+)\.\.([0-9A-F]+)\s*;\s*([\w\s]+)#\s*(.*)$')

        self.general_category_map = dict()
        self.general_category = list()
        self.blocks = list()

        self.builder = EnumBuilderArray([ # TODO: rename to enum_builder
            EnumClassWriter(HEADER_ROOT + '/ucd_enums.h'),
            EnumOstreamWriter(HEADER_ROOT + '/ucd_ostream.h')
        ])

    def generate(self):
        self.load_property_value_aliases()
        self.load_general_category()
        self.load_core_properties()
        self.load_scripts()
        self.load_script_extensions()
        self.load_blocks()

        self.file_header()
        self.write_properties()
        self.write_core_properties()
        self.write_general_categories()
        self.write_scripts()
        self.write_script_extensions()
        self.write_blocks()

        self.process_grapheme_break_props()
        self.process_east_asian_width()
        self.process_emoji_props()

        self.file_footer()

    def close(self):
        self.builder.close()
        self.header.close()
        self.impl.close()

    def file_header(self): # {{{
        self.header.write(globals()['__doc__'])
        self.header.write("""#pragma once

#include <unicode/ucd_enums.h>

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

    def load_property_value_aliases(self): # {{{
        black_list = set()
        black_list.add('Block')

        with uopen(self.ucd_dir + '/PropertyValueAliases.txt') as f:
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
            if name in ['Script', 'General_Category', 'Grapheme_Cluster_Break', 'EastAsianWidth']:
                # XXX those properties are generated somewhere else.
                continue

            # Construct member valus
            values = list()
            for key in self.property_values[name].keys():
                values.append(self.property_values[name][key])
            values.sort()

            # build enum class
            self.builder.begin(name)
            for value in values:
                self.builder.member(value)
            self.builder.end()
        # }}}

    def load_general_category(self): # {{{
        with uopen(self.ucd_dir + '/extracted/DerivedGeneralCategory.txt') as f:
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

    def load_core_properties(self): # {{{
        with uopen(self.ucd_dir + '/DerivedCoreProperties.txt') as f:
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
        self.impl.write("bool contains(Core_Property _prop, char32_t _codepoint) noexcept {\n")
        self.impl.write("    switch (_prop) {\n")
        for name in sorted(props.keys()):
            self.impl.write("        case Core_Property::{0:}: return contains(tables::{0:}, _codepoint);\n".format(name))
        self.impl.write("    }\n")
        self.impl.write("    return false;\n")
        self.impl.write("}\n\n")

        # API: write enum and tester
        self.header.write("bool contains(Core_Property _prop, char32_t _codepoint) noexcept;\n\n")
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

    def process_props(self, filename, prop_key): # {{{
        with uopen(filename) as f:
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
                element_type = 'Prop<::unicode::{}>'.format(name)
                self.impl.write("auto static const {} = std::array<{}, {}>{{ // {}\n".format(
                    name,
                    element_type,
                    len(props[name]),
                    FOLD_OPEN))
                for propRange in props[name]:
                    self.impl.write("    {}{{ {{ 0x{:>04X}, 0x{:>04X} }}, ::unicode::{}::{} }}, // {}\n".format(
                               element_type,
                               propRange['start'],
                               propRange['end'],
                               name,
                               propRange['property'],
                               propRange['comment']))
                self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
            self.impl.write("} // end namespace tables\n\n")

            # XXX write enums / signature
            # for name in sorted(props.keys()):
            #     self.header.write('enum class {} {{\n'.format(name))
            #     enum_set = set()
            #     for enum in props[name]:
            #         enum_set.add(enum['property'])
            #     for enum in sorted(enum_set):
            #         self.header.write("    {},\n".format(enum))
            #     self.header.write("};\n\n")

            # builder
            for name in sorted(props.keys()):
                enum_set = set()
                for enum in props[name]:
                    enum_set.add(enum['property'])

                self.builder.begin(name)
                for enum in sorted(enum_set):
                    self.builder.member(enum)
                self.builder.end()

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

    def load_scripts(self):
        self.scripts = self.load_generic_properties(self.ucd_dir + '/Scripts.txt')

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
            self.impl.write("    {}{{ {{ 0x{:>04X}, 0x{:>04X} }}, unicode::{}::{} }}, // {}\n".format(
                            element_type,
                            propRange['start'],
                            propRange['end'],
                            name,
                            propRange['property'],
                            propRange['comment']))

        self.impl.write("}}; // {}\n".format(FOLD_CLOSE))
        self.impl.write("} // end namespace tables\n\n")

        self.builder.begin(name)
        self.builder.member('Unknown')
        for p in sorted(pset):
            self.builder.member(p)
        self.builder.end()

        # codepoint-to-script mapping function
        self.header.write('{} {}(char32_t _codepoint) noexcept;\n\n'.format(name, name.lower()))

        self.impl.write('{} {}(char32_t _codepoint) noexcept {{\n'.format(name, name.lower()))
        self.impl.write('    if (auto const p = search(tables::{}, _codepoint); p.has_value())\n'.format(name))
        self.impl.write('        return p.value();\n')
        self.impl.write('    return Script::Unknown;\n')
        self.impl.write('}\n\n')
        # }}}

    def load_blocks(self): # {{{
        filename = self.ucd_dir + "/Blocks.txt"
        with uopen(filename) as f:
            line_regex = re.compile('^([0-9A-Fa-f]+)\.\.([0-9A-Fa-f]+);\s*(.*)$')
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
        filename = self.ucd_dir + '/ScriptExtensions.txt'
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
            self.impl.write("    {}{{ {{ 0x{:>04X}, 0x{:>04X} }}, {} }}, // {}\n".format(
                            element_type,
                            sce['start'],
                            sce['end'],
                            '{{ {0}.data(), {0}.size() }}'.format(key),
                            sce['comment']))
        self.impl.write('} };\n')
        self.impl.write("}} // {}\n\n".format(FOLD_CLOSE))

        # getter function
        self.header.write("size_t script_extensions(char32_t _codepoint, Script* _result, size_t _capacity) noexcept;\n\n")
        self.impl.write("size_t script_extensions(char32_t _codepoint, Script* _result, size_t _capacity) noexcept {\n")
        self.impl.write("    if (auto const p = search(tables::{}, _codepoint); p.has_value()) {{\n".format('sce'))
        self.impl.write('        auto const cap = std::min(_capacity, p.value().second);\n')
        self.impl.write('        for (size_t i = 0; i < cap; ++i)\n')
        self.impl.write('            _result[i] = p.value().first[i];\n')
        self.impl.write('        return cap;\n')
        self.impl.write("    }\n")
        self.impl.write('    *_result = script(_codepoint);\n')
        self.impl.write('    return 1;\n')
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
        with uopen(self.ucd_dir + '/emoji/emoji-data.txt') as f:
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
                self.impl.write('bool {}(char32_t _codepoint) noexcept {{\n'.format(name.lower()))
                self.impl.write("    return contains(tables::{0:}, _codepoint);\n".format(name))
                self.impl.write("}\n\n")

            # write enums / signature
            for name in sorted(props.keys()):
                self.header.write('bool {}(char32_t _codepoint) noexcept;\n'.format(name.lower()))
            self.header.write('\n')
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
                '    {}{{ {{ 0x{:>04X}, 0x{:>04X} }}, {}::{} }},\n'.format(
                element_type,
                block['start'],
                block['end'],
                '::unicode::Block',
                sanitize_identifier(block['title'])))
        self.impl.write("};\n") # close table
        self.impl.write("} // end namespace tables {}\n\n")

        # write out search function
        self.impl.write("std::optional<Block> block(char32_t _codepoint) noexcept {\n")
        self.impl.write("    return search(tables::Block, _codepoint).value_or(::unicode::Block::Unspecified);\n")
        self.impl.write("}\n\n")

        self.header.write("std::optional<Block> block(char32_t _codepoint) noexcept;\n\n")

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
                "    {}{{ {{ 0x{:>04X}, 0x{:>04X} }}, {}::{} }}, // {}\n".format(
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
        self.impl.write("namespace {} {{\n".format(type_name.lower()))
        self.impl.write("    {} get(char32_t _value) noexcept {{\n".format(type_name))
        self.impl.write("        if (auto const p = search(tables::{}, _value); p.has_value())\n".format(type_name))
        self.impl.write('            return p.value();\n')
        self.impl.write('        return {}::{};\n'.format(type_name, UNSPECIFIED))
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
        self.impl.write("bool contains(General_Category _cat, char32_t _codepoint) noexcept {\n")
        self.impl.write("    switch (_cat) {\n")
        for name in sorted(cats.keys()):
            self.impl.write("        case General_Category::{0:}: return contains(tables::{0:}, _codepoint);\n".format(name))
        self.impl.write("        case General_Category::{0:}: return false;\n".format(UNSPECIFIED)) # special case
        self.impl.write("    }\n")
        self.impl.write("    return false;\n")
        self.impl.write("}\n\n")

        # write signature
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

        with uopen(self.ucd_dir + '/EastAsianWidth.txt') as f:
            table = self.collect_range_table_with_prop(f)

            # api: enum
            self.builder.begin(table_name)
            for v in WIDTH_NAMES.values():
                self.builder.member(v)
            self.builder.end()

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
            element_type = 'Prop<{}>'.format(prop_type)
            self.impl.write("auto static const {} = std::array<{}, {}>{{ // {}\n".format(
                table_name,
                element_type,
                len(table),
                FOLD_OPEN
            ))
            for propRange in table:
                self.impl.write("    {}{{ {{ 0x{:>04X}, 0x{:>04X} }}, {}::{} }}, // {}\n".format(
                                element_type,
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
# }}}

def needs_run():
    try:
        for filename in ['ucd.cpp', 'ucd.h', 'ucd_enums.h', 'ucd_ostream.h']:
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
    else:
        print("Output files up-to-date.")

main()
