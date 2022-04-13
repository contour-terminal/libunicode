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
#ifndef LIBUNICODE_CAPI_H
#define LIBUNICODE_CAPI_H 1

#include <stddef.h>
#include <stdint.h>

#if !defined(__cplusplus)
extern "C"
{
#endif

#define U32_CODEPOINT_MAX  0x10FFFF // 0b1'0000'1111'1111'1111'1111
#define U32_CODEPOINT_MIN  0
#define U32_CODEPOINT_MASK 0x1FFFFF // 0b1'1111'1111'1111'1111'1111

    /// UTF-8 character or 8bit segment of an UTF-8 character.
    typedef char u8_char_t;

    /// UTF-32 codepoint between 0 and 0x10FFFF. Any valud outside that
    /// range must be properly handled by the functions above to avoid undefined
    /// behavior.
    typedef uint_least32_t u32_char_t;

/**
 * Verifies that _p codepoint is a valid codepoint,
 */
#define u32_is_valid_codepoint(_codepoint) \
    ((_codepoint) < 0xD800 || ((_codepoint) > 0xDFFF && (_codepoint) <= 0x10FFFF)

/**
 * Extracts the unused higher order bits and moves them bit-wise to the right.
 *
 * A UTF-32 character is 32 bits wide (on a machine at least 32 bits wide),
 * and the largest valid UTF-32 codepoint is 0x10FFFF.
 * That is, the 21 least significant bits are used and the 11 most significant
 * bits are available other application specific purposes.
 */
#define u32_unused_bit_mask(_codepoint) ((_codepoint) >> 21)

/**
 * Returns the number of available bits that are free
 * for application-specific use.
 *
 * If the machine type for UTF-32 is actually 32 bits wide, this
 * function yields 11 bits. It is guaranteed to always return at least 11.
 */
#define u32_unused_bit_capacity() (8 * sizeof(u32_char_t) - 21)

/**
 * Tests if given bit at @p _index of the unused most significant bits is set.
 */
#define u32_unused_bit_get(_codepoint, _index) (((_codepoint) & (1 << ((_index) + 21))) != 0)

/**
 * Sets the bit at @p _index of the unused most significant bits.
 */
#define u32_unused_bit_on(_codepoint, _index) ((_codepoint) | (1 << ((_index) + 21)))

/**
 * Clears the bit at @p _index of the unused most significant bits.
 */
#define u32_unused_bit_off(_codepoint, _index) ((_codepoint) & ~(1 << ((_index) + 21)))

/**
 * Returns @p _codepoint with all unused bits cleared.
 */
#define u32_unused_bit_cleared(_codepoint) ((_codepoint) &U32_CODEPOINT_MASK)

    /**
     * Counts the number of grapheme clusters for given sequence of codepoints.
     *
     * Use this function to determine the number of
     * user perceived characters (grapheme clusters).
     *
     * @param codepoints   pointer to the first codepoint.
     * @param n            number of codepoints to count the grapheme clusters for.
     *
     * @return number of user perceived characters (grapheme clusters) counted
     *         in [codepoints, codepoints+n).
     */
    int u32_gc_count(u32_char_t const* codepoints, size_t n);
    int u8_gc_count(u8_char_t const* codepoints, size_t n);

/**
 * Determines that u32_gc_width()/u8_gc_width() must not respect
 * variation selectors, and thus, will not change the width of a
 * processed grapheme cluster.
 *
 * Using this is not recommended unless backwards compatibility with
 * broken clients is of concern.
 */
#define GC_WIDTH_MODE_NON_MODIFIABLE 0

/**
 * Mandates that u32_gc_width()/u8_gc_width() must respect
 * variation selectors, thus, allow changing the width of
 * a processed grapheme cluster.
 */
#define GC_WIDTH_MODE_MODIFIABLE 1

    /**
     * Computes the display width for given sequence of codepoints,
     * respecting grapheme cluters, and modifiers.
     *
     * @param codepoints  pointer to first codepoint
     * @param n           number of codepoints
     * @param mode        determines how to deal with variation selectors that do
     *                    force changing the width or a grapheme cluster.
     *                    Valid values are:
     *                    GC_WIDTH_MODE_MODIFIABLE (allow, recommended),
     *                    GC_WIDTH_MODE_NON_MODIFIABLE (disallowed).
     *
     * Use this function to determine how many terminal grid cells a
     * string of codepoints should occupy when being rendered.
     */
    int u32_gc_width(u32_char_t const* codepoints, size_t n, int mode);

    /**
     * UTF-8 version of @c u32_gc_width().
     *
     * @see u32_gc_width(u32_char_t const* codepoints, size_t n, int allowMod)
     */
    int u8_gc_width(u8_char_t const* codepoints, size_t n, int allowMod);

    /**
     * Tests if two consecutive codepoints do belong to the same grapheme cluster,
     * i.e. are unbreakable and thus should not be broken up.
     *
     * @retval 1   both codepoints to belong to the same grapheme cluster.
     * @retval 0   both codepoints do not belong to the same grapheme cluster.
     *
     * @note The grapheme cluster segmentation algorithm walks through an
     * ordered sequence of checks that would either yield return value true
     * or value. If non of these rules match, true will be returned, meaning
     * that the both codepoints @p a and @p b can be broken up.
     * This implies that codepoints outside the valid Unicode range will also yield
     * return code true.
     */
    int u32_grapheme_unbreakable(u32_char_t a, u32_char_t b);

    /**
     * Opaque handle for the UTF-8 to UTF-32 stream converter.
     */
    struct u8u32_stream_state;
    typedef struct u8u32_stream_state* u8u32_stream_state_t;

    /**
     * Constructs an UTF-8-to-UTF-32 streamed converter context.
     */
    u8u32_stream_state_t u8u32_stream_convert_create();

    /**
     * Processes a single UTF-8 byte to incrementally convert
     * consecutively incoming UTF-8 bytes into a sequence of UTF-32 codepoints.
     *
     * @param handle  The handle to the previously created streaming context.
     * @param input   A UTF-8 character to be procecced consecutively.
     * @param output  Will contain the fully parsed UTF-32 codepoint every time
     *                one is available.
     *
     * @retval 0 The codepoint is incomplete and needs more data; @p output is not touched.
     * @retval 1 The UTF-8 codepoint was fully processed and stored into @p output.
     *
     * @note Invalid input is silently ignored.
     */
    int u8u32_stream_convert_run(u8u32_stream_state_t handle, u8_char_t input, u32_char_t* output);

    /**
     * Destroys the UTF-8-to-UTF-32 streaming converter context.
     * The parameer @p handle will be set to NULL when this call leaves.
     */
    void u8u32_stream_convert_destroy(u8u32_stream_state_t* handle);

    /**
     * Convertes a UTF-32 sequence to UTF-8.
     *
     * @param source Pointer sequence of UTF-32 characters to convert.
     * @param slen   Number of UTF-32 characters to convert.
     * @param dest   Destination address where to store the converted UTF-8 sequence to.
     * @param dlen   Number of bytes to write to @p _dest at most.
     *
     * @note No trailing zero byte will be written.
     *
     * @retval >0     Success. TRhe number of bytes written to @p _dest is returned.
     * @retval  0     _slen is 0, and nothing was converted.
     * @retval -1     Some characters have been converted but target destination
     *                is not large enough to continue.
     */
    int u32u8_convert(u32_char_t const* source, size_t slen, u8_char_t* dest, size_t dlen);

#if !defined(__cplusplus)
}
#endif

#endif
