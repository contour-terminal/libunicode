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
#include <unicode/capi.h>
#include <unicode/convert.h>
#include <unicode/grapheme_segmenter.h>
#include <unicode/ucd.h>
#include <unicode/width.h>

#include <iterator>

int u32_gc_count(u32_char_t const* codepoints, size_t size)
{
    if (!size)
        return 0;

    int count = 1;
    auto segmenter =
        unicode::grapheme_segmenter((char32_t const*) codepoints, (char32_t const*) codepoints + size);
    while (segmenter.codepointsAvailable())
    {
        ++segmenter;
        ++count;
    }
    return count;
}

int u8_gc_count(u8_char_t const* codepoints, size_t size)
{
    auto const u32 = unicode::convert_to<char32_t>(std::string_view(codepoints, size));
    return u32_gc_count((uint32_t const*) u32.data(), u32.size());
}

int u32_gc_width(u32_char_t const* codepoints, size_t size, int mode)
{
    int totalWidth = 0;
    auto segmenter =
        unicode::grapheme_segmenter((char32_t const*) codepoints, (char32_t const*) codepoints + size);
    while (segmenter.codepointsAvailable())
    {
        auto const cluster = *segmenter;
        int thisWidth = unicode::width(cluster.front());
        if (mode != GC_WIDTH_MODE_NON_MODIFIABLE)
        {
            for (size_t i = 1; i < size; ++i)
            {
                auto const codepoint = codepoints[i];
                auto const width = [&]() {
                    switch (codepoint)
                    {
                    case 0xFE0E: return 1;
                    case 0xFE0F: return 2;
                    default: return unicode::width(codepoint);
                    }
                }();
                if (width && width != thisWidth)
                    thisWidth = width;
            }
        }
        totalWidth += thisWidth;
        ++segmenter;
    }
    return totalWidth;
}

int u8_gc_width(u8_char_t const* codepoints, size_t count, int allowMod)
{
    (void) codepoints;
    (void) count;
    (void) allowMod;

    return -1; // TODO
}

int u32_grapheme_unbreakable(u32_char_t a, u32_char_t b)
{
    return unicode::grapheme_segmenter::nonbreakable(a, b);
}

struct u8u32_stream_state
{
    unicode::decoder<char> conv {};
};

u8u32_stream_state_t u8u32_stream_convert_create()
{
    return new u8u32_stream_state();
}

int u8u32_stream_convert_run(u8u32_stream_state_t handle, u8_char_t input, u32_char_t* output)
{
    if (auto const codepoint = handle->conv(static_cast<uint8_t>(input)); codepoint.has_value())
    {
        *output = codepoint.value();
        return 1;
    }
    return 0;
}

void u8u32_stream_convert_destroy(u8u32_stream_state_t* handle)
{
    delete *handle;
    *handle = nullptr;
}

int u32u8_convert(u32_char_t const* source, size_t slen, u8_char_t* dest, size_t dlen)
{
    auto conv = unicode::encoder<u8_char_t> {};
    auto nwritten = 0;

    for (size_t i = 0; i < slen; ++i)
    {
        u8_char_t buf[4];
        auto const bufEnd = conv(source[i], buf);
        auto const bufLength = static_cast<size_t>(std::distance(buf, bufEnd));
        if (!(bufLength < dlen))
            return -1;

        for (size_t k = 0; k < bufLength; ++k)
            dest[k] = buf[k];
        nwritten += static_cast<int>(bufLength);
        dest += bufLength;
        dlen -= bufLength;
    }

    return nwritten;
}
