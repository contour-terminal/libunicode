/**
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
#pragma once

#include <libunicode/utf8.h>

#include <string_view>

namespace unicode
{

/// Holds the result of a call to scan_test().
struct scan_result
{
    /// Number of columns scanned.
    /// One column equals a single narrow-width codepoint.
    /// Codepoints with property East Asian Width Wide are treated as two columns.
    size_t count;

    /// Pointer to one byte after the last scanned codepoint.
    char const* next;

    /// Pointer to UTF-8 grapheme cluster start.
    char const* start;

    /// Pointer to UTF-8 grapheme cluster end, i.e. one byte behind
    /// the last successfuly processed complete UTF-8 byte..
    char const* end;
};

/// Holds the state to keep through a consecutive sequence of calls to scan_test().
///
/// This state holds the UTF-8 decoding state, if processing had to be stopped
/// at an incomplete UTF-8 byte sequence,
/// and the last decoded Unicode codepoint necessary for grapheme cluster segmentation.
struct scan_state
{
    utf8_decoder_state utf8 {};
    char32_t lastCodepointHint {};
};

/// Callback-interface that allows precisely understanding the structure of a UTF-8 sequence.
class grapheme_cluster_receiver
{
  public:
    virtual ~grapheme_cluster_receiver() = default;

    virtual void receiveAsciiSequence(std::string_view codepoints) noexcept = 0;
    virtual void receiveGraphemeCluster(std::string_view codepoints, size_t columnCount) noexcept = 0;
    virtual void receiveInvalidGraphemeCluster() noexcept = 0;
};

/// Quite obviousely, this grapheme_cluster_receiver will do nothing.
class null_receiver final: public grapheme_cluster_receiver
{
  public:
    void receiveAsciiSequence(std::string_view) noexcept override {}
    void receiveGraphemeCluster(std::string_view, size_t) noexcept override {}
    void receiveInvalidGraphemeCluster() noexcept override {}

    static null_receiver& get() noexcept
    {
        static null_receiver instance {};
        return instance;
    }
};

namespace detail
{
    size_t scan_for_text_ascii(std::string_view text, size_t maxColumnCount) noexcept;
    scan_result scan_for_text_nonascii(scan_state& state,
                                       std::string_view text,
                                       size_t maxColumnCount,
                                       grapheme_cluster_receiver& receiver) noexcept;
} // namespace detail

/// Scans a sequence of UTF-8 encoded bytes.
///
/// This call will return early one of the conditions is met:
///
/// - given the input sequence, the right most invalid or complete UTF-8 sequence is processed,
/// - maxColumnCount is reached and the next grapheme cluster would exceed the given limit,
/// - a control character is about to be processed.
///
/// When this function returns, it is guaranteed to not contain an incomplete UTF-8 sequence
/// at the end of the output sequence.
///
/// Calling this function again with more bytes will resume decoding that UTF-8 sequence
/// with the help of the passed UTF-8 decoder state.
///
/// @return scanned textual result. This is, a sequence of
///         either valid or invalid UTF-8 codepoints,
///         but not incomplete codepoints at the end.
scan_result scan_text(scan_state& state, std::string_view text, size_t maxColumnCount) noexcept;

scan_result scan_text(scan_state& state,
                      std::string_view text,
                      size_t maxColumnCount,
                      grapheme_cluster_receiver& receiver) noexcept;

} // namespace unicode
