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
#pragma once

#include <libunicode/grapheme_segmenter.h>

#if defined(LIBUNICODE_USE_INTRINSICS)
    #include <libunicode/intrinsics.h>
#endif
#include <libunicode/utf8.h>
#include <libunicode/width.h>

#include <cassert>
#include <ostream>
#include <string_view>

// clang-format off
#if __has_include(<experimental/simd>) && defined(LIBUNICODE_USE_STD_SIMD)
    #define LIBUNICODE_HAS_STD_SIMD
    #include <experimental/simd>
    namespace stdx = std::experimental;
#elif __has_include(<simd>) && defined(LIBUNICODE_USE_STD_SIMD)
    #define LIBUNICODE_HAS_STD_SIMD
    #include <simd>
    namespace stdx = std;
#endif
// clang-format on

namespace unicode
{

// Represents the reason why the processing stopped.
enum class StopCondition
{
    // Unexpected input usually means control characters.
    UnexpectedInput,
    // End of input range is reached.
    EndOfInput,
    // Total number of columns (east asian widths) to process at most has been reached.
    EndOfWidth,
};

// Represents the result of a single call to process().
struct grapheme_segmentation_result
{
    // Represents the text that was scanned.
    std::string_view text;

    // Represents the sum of all east asian widths of the grapheme clusters of the given text.
    unsigned width;

    // Represents the reason why the processing stopped.
    StopCondition stop_condition;

    constexpr auto operator<=>(grapheme_segmentation_result const& rhs) const noexcept = default;
};

// Convenience listener interface for grapheme segmentation events.
class grapheme_segmentation_listener
{
  public:
    virtual ~grapheme_segmentation_listener() = default;

    virtual void on_invalid(std::string_view invalid) noexcept = 0;
    virtual void on_ascii(std::string_view text) noexcept = 0;
    virtual void on_grapheme_cluster(std::string_view text, unsigned width) noexcept = 0;
};

template <typename T>
concept GraphemeSegmentationListenerConcept = requires(T t, T const& u) {
    t.on_invalid(std::string_view {});
    t.on_ascii(std::string_view {});
    t.on_grapheme_cluster(std::string_view {}, unsigned {});
};

template <typename T>
concept OptionalGraphemeSegmentationListenerConcept = GraphemeSegmentationListenerConcept<T> || std::same_as<T, void>;

// {{{ grapheme_line_segmenter details
namespace detail
{
    template <typename...>
    struct EventHandler;

    template <GraphemeSegmentationListenerConcept EventListener>
    struct EventHandler<EventListener>
    {
        EventListener& listener;
        constexpr void on_invalid(std::string_view s) noexcept { listener.on_invalid(s); }
        constexpr void on_ascii(std::string_view s) noexcept { listener.on_ascii(s); }
        constexpr void on_grapheme_cluster(std::string_view s, unsigned w) noexcept { listener.on_grapheme_cluster(s, w); }
    };

    template <GraphemeSegmentationListenerConcept EventListener>
    EventHandler(EventListener) -> EventHandler<EventListener>;

    template <>
    struct EventHandler<void>
    {
        void on_invalid(std::string_view) noexcept {}
        void on_ascii(std::string_view) noexcept {}
        void on_grapheme_cluster(std::string_view, unsigned) noexcept {}
    };

    template <>
    struct EventHandler<>: EventHandler<void>
    {
    };

    [[maybe_unused]] inline int countTrailingZeroBits(unsigned int value) noexcept
    {
#if defined(_WIN32)
        // return _tzcnt_u32(value);
        // Don't do _tzcnt_u32, because that's only available on x86-64, but not on ARM64.
        unsigned long r = 0;
        _BitScanForward(&r, value);
        return r;
#else
        return __builtin_ctz(value);
#endif
    }

    constexpr bool is_control(char ch) noexcept
    {
        return static_cast<uint8_t>(ch) < 0x20;
    }

    // Tests if given UTF-8 byte is part of a complex Unicode codepoint, that is, a value greater than U+7E.
    constexpr bool is_complex(char ch) noexcept
    {
        return static_cast<uint8_t>(ch) & 0x80;
    }

    // Tests if given UTF-8 byte is a single US-ASCII text codepoint. This excludes control characters.
    constexpr bool is_ascii(char ch) noexcept
    {
        return !is_control(ch) && !is_complex(ch);
    }

    struct unicode_process_state
    {
        // Holds the UTF-8 decoding state between calls to process().
        utf8_decoder_state utf8 {};

        // Holds the pointer to the next UTF-8 byte to process when resuming processing.
        char const* utf8DecodeNext {};

        // Start position of current grapheme cluster.
        char const* currentClusterStart {};

        // Start position of current codepoint.
        char const* currentCodepointStart {};

        // Current grapheme cluster's East Asian Width.
        unsigned currentClusterWidth = 0;

        // Holds the previously processed codepoint.
        // This information is used to decide if we have hit the boundary of a grapheme cluster
        // of a complex Unicode codepoint sequence or not.
        // This value is not needed when processing trivial (US-ASCII) codepoints.
        char32_t lastCodepointHint = 0;
    };

    // Holds the result of complex Unicode processing.
    struct unicode_process_result
    {
        // Sum of all east asian widths of the grapheme clusters processed.
        unsigned totalWidth {};

        // The end position of the last processed byte.
        char const* end {};

        StopCondition stop_condition = StopCondition::UnexpectedInput;
    };

    constexpr unicode_process_result make_scan_result(unsigned consumedWidths,
                                                      char const* const end,
                                                      StopCondition stopCondition) noexcept
    {
        return { .totalWidth = consumedWidths, .end = end, .stop_condition = stopCondition };
    }

    // Flushes out pending grapheme cluster, if any.
    template <typename EventHandlerT>
    inline unsigned flush_grapheme_cluster(EventHandlerT& eventHandler,
                                           unicode_process_state& state,
                                           char const* const current,
                                           unsigned maxWidth) noexcept
    {
        if (state.utf8.expectedLength) // Incomplete UTF-8 sequence hit.
        {
            assert(state.currentClusterStart <= current);
            if (1 <= maxWidth)
            {
                eventHandler.on_invalid(std::string_view(state.currentClusterStart, current));
                state = {
                    .utf8DecodeNext = current,
                    .currentClusterStart = current,
                    .currentCodepointStart = current,
                };
                return 1;
            }
            else
                return 0;
        }
        else if (state.currentClusterStart && state.currentClusterStart < current)
        {
            // Current grapheme cluster is complete.
            unsigned const width = state.currentClusterWidth;
            if (width <= maxWidth)
            {
                assert(state.currentClusterStart < current);
                eventHandler.on_grapheme_cluster(std::string_view(state.currentClusterStart, current), width);
                state = {
                    .utf8DecodeNext = current,
                    .currentClusterStart = current,
                    .currentCodepointStart = current,
                };
                return width;
            }
            else
            {
                // Currently scanned grapheme cluster won't fit. Revert to cluster start.
                auto const revertPoint = state.currentClusterStart;
                state = {
                    .utf8DecodeNext = revertPoint,
                    .currentClusterStart = revertPoint,
                    .currentCodepointStart = revertPoint,
                };
                return 0;
            }
        }
        else
        {
            // Current grapheme cluster is empty.
            assert(state.currentClusterStart == current);
            return 0;
        }
    }

    /// Feeds the next Unicode codepoint into the grapheme cluster processor,
    /// determining if the current grapheme cluster is complete (grapheme cluster boundary detected).
    ///
    /// @retval true on grapheme cluster boundary detected
    /// @retval false on grapheme cluster boundary not detected
    inline bool feed_grapheme_cluster(unicode_process_state& state,
                                      char32_t const nextCodepoint,
                                      char const* const input) noexcept
    {
        // We've successfully decoded the next UTF-8 codepoint.

        auto const prevCodepoint = state.lastCodepointHint;

        auto constexpr BoundaryFound = true;
        auto constexpr BoundaryNotFound = false;

        if (!prevCodepoint)
        {
            state.lastCodepointHint = nextCodepoint;
            state.currentCodepointStart = input;
            state.currentClusterWidth = unicode::width(nextCodepoint);

            return BoundaryNotFound;
        }

        else if (grapheme_segmenter::breakable(prevCodepoint, nextCodepoint))
        {
            return BoundaryFound;
        }
        else
        {
            // Increase width on VS16 but do not decrease on VS15.
            if (nextCodepoint == 0xFE0F) // VS16
                state.currentClusterWidth = 2;
            else if (nextCodepoint == 0xFE0E) // VS15
            {
#if 0
                state.currentClusterWidth = 1;
#endif
            }
            // else
            //     state.currentClusterWidth = std::max(state.currentClusterWidth, unicode::width(nextCodepoint));

            state.lastCodepointHint = nextCodepoint;
            state.currentCodepointStart = input;
            return BoundaryNotFound;
        }
    }

    // Flushes out pending grapheme cluster, if any.
    //
    // @param eventHandler the event listener interface to report events to
    // @param state the current state of the complex Unicode processing
    // @param maxWidth the maximum number of widths to fill in the current line
    //
    // @note A call to this function is idempotent.
    //
    // @returns the result of the processing
    template <typename EventHandlerT>
    inline grapheme_segmentation_result flush_complex_unicode(EventHandlerT& eventHandler,
                                                              unicode_process_state& state,
                                                              unsigned maxWidth) noexcept
    {
        auto const clusterStart = state.currentClusterStart;
        auto const count = flush_grapheme_cluster(eventHandler, state, state.utf8DecodeNext, maxWidth);
        return {
            .text = std::string_view { clusterStart, state.utf8DecodeNext },
            .width = count,
            .stop_condition = StopCondition::EndOfInput,
        };
    }

    // Processes up to [start, end) ASCII characters.
    //
    // A call to this function will never process control characters nor non-ASCII (complex Unicode)
    // characters.
    //
    // @returns the number of ASCII characters processed (equal to the sum of East Asian Width for each).
    inline std::pair<StopCondition, char const*> process_only_ascii(char const* start, char const* end) noexcept
    {
        auto input = start;

#if defined(LIBUNICODE_HAS_STD_SIMD)
        using char8_type = unsigned char;
        constexpr auto batchSize = stdx::simd_abi::max_fixed_size<char8_type>;
        using batch_type = stdx::fixed_size_simd<char8_type, batchSize>;
        while (input <= end - batchSize)
        {
            auto const batch = batch_type { input, stdx::element_aligned };
            auto const testPack = (batch < 0x20) | (batch >= 0x80);
            if (stdx::popcount(testPack) > 0)
                return { StopCondition::UnexpectedInput, input + stdx::find_first_set(testPack) };
            input += batchSize;
        }
#elif defined(LIBUNICODE_USE_INTRINSICS)
        auto constexpr BatchSize = sizeof(intrinsics::m128i);
        auto const ControlCodeMax = intrinsics::set1_epi8(0x20); // 0..0x1F
        auto const Complex = intrinsics::set1_epi8(-128);        // equals to 0x80 (0b1000'0000)

        while (input <= end - BatchSize)
        {
            auto const batch = intrinsics::load_unaligned((intrinsics::m128i*) input);
            auto const isControl = intrinsics::compare_less(batch, ControlCodeMax);
            auto const isComplex = intrinsics::and128(batch, Complex);
            auto const testPack = intrinsics::or128(isControl, isComplex);
            if (auto const check = static_cast<unsigned>(intrinsics::movemask_epi8(testPack)); check != 0)
                return { StopCondition::UnexpectedInput, input + countTrailingZeroBits(check) };
            input += BatchSize;
        }
#endif

        while (true)
        {
            if (input == end)
                return { StopCondition::EndOfInput, input };
            if (!is_ascii(*input))
                return { StopCondition::UnexpectedInput, input };
            ++input;
        }
    }

    inline std::pair<StopCondition, char const*> process_only_ascii(std::string_view text) noexcept
    {
        return process_only_ascii(text.data(), text.data() + text.size());
    }

    // Processes up to maxWidth grapheme clusters.
    //
    // @param events            event listener interface to report events to
    // @param utf8              the UTF-8 decoding state to use
    // @param lastCodepointHint the last codepoint to use resuming processing
    //                          the first grapheme cluster. This value is 0 if there is no such hint.
    // @param start             the start of the text to process
    // @param end               the end of the text to process (this is one byte past the last byte)
    // @param maxWidth          the maximum number of widths to fill in the current line
    //
    // - The returned width is the sum of all east asian widths of the grapheme
    //   clusters of the returned text.
    // - The returned text is a substring of the input text.
    // - And the returned text is guaranteed to not contain any control characters,
    //   nor any incomplete UTF-8 sequences.
    //
    // @returns a sequence of grapheme clusters up to maxWidth width.
    template <typename EventHandlerT>
    auto process_only_complex_unicode(
        EventHandlerT& eventHandler, unicode_process_state& state, char const* start, char const* end, unsigned maxWidth) noexcept
        -> detail::unicode_process_result
    {
        if (!state.utf8DecodeNext)
        {
            // Initialize state, as it's the first call to process_only_complex_unicode() for this line.
            state = {
                .utf8DecodeNext = start,
                .currentClusterStart = start,
                .currentCodepointStart = start,
                .currentClusterWidth = 0,
                .lastCodepointHint = 0,
            };
        }

        char const* input = state.utf8DecodeNext; // current input processing position
        unsigned consumedWidths = 0;              // sum of all widths consumed for the current line

        while (true)
        {
            if (not(input != end))
            {
                // We've reached the end of the input.
                // There may be an incomplete grapheme cluster left, that we need to resume processing with on the next call.
                state.utf8DecodeNext = input;
                return make_scan_result(consumedWidths, input, StopCondition::EndOfInput);
            }
            assert(consumedWidths <= maxWidth);

            if (!detail::is_complex(*input))
            {
                consumedWidths += flush_grapheme_cluster(eventHandler, state, input, maxWidth);
                state.utf8DecodeNext = input;
                return make_scan_result(consumedWidths, input, StopCondition::UnexpectedInput);
            }

            auto const result = from_utf8(state.utf8, static_cast<uint8_t>(*input++));

            if (std::holds_alternative<unicode::Incomplete>(result))
            {
                continue;
            }
            else if (auto const* success = std::get_if<unicode::Success>(&result); success)
            {
                auto const currentCodepoint = success->value;
                auto const boundary = feed_grapheme_cluster(state, currentCodepoint, input);
                if (boundary)
                {
                    if (consumedWidths + state.currentClusterWidth <= maxWidth)
                    {
                        consumedWidths += state.currentClusterWidth;
                        assert(state.currentClusterStart < state.currentCodepointStart);
                        eventHandler.on_grapheme_cluster(std::string_view(state.currentClusterStart, state.currentCodepointStart),
                                                         state.currentClusterWidth);
                        state.lastCodepointHint = currentCodepoint;
                        state.currentClusterWidth = unicode::width(currentCodepoint);
                        state.currentClusterStart = state.currentCodepointStart;
                        state.currentCodepointStart = input;

                        if (consumedWidths == maxWidth)
                        {
                            // We've reached the end of the line.
                            state.utf8DecodeNext = state.currentClusterStart;
                            return make_scan_result(consumedWidths, state.currentClusterStart, StopCondition::EndOfWidth);
                        }
                    }
                    else
                    {
                        // Currently scanned grapheme cluster won't fit. Revert to cluster start.
                        state.utf8DecodeNext = state.currentClusterStart;
                        return make_scan_result(consumedWidths, state.currentClusterStart, StopCondition::EndOfWidth);
                    }
                }
            }
            else if (std::holds_alternative<unicode::Invalid>(result))
            {
                state.currentClusterWidth = 1;
                if (consumedWidths + state.currentClusterWidth <= maxWidth)
                {
                    eventHandler.on_invalid(std::string_view(state.currentClusterStart, input));
                    consumedWidths += state.currentClusterWidth;
                    state.currentClusterStart = input;
                }
                else
                {
                    // Currently scanned grapheme cluster won't fit. Revert to cluster start.
                    state.utf8DecodeNext = state.currentClusterStart;
                    return make_scan_result(consumedWidths, state.currentClusterStart, StopCondition::EndOfWidth);
                }
                continue;
            }
        }
    }

    template <typename EventHandlerT>
    unicode_process_result process_only_complex_unicode(EventHandlerT& eventHandler,
                                                        unicode_process_state& state,
                                                        std::string_view text,
                                                        unsigned maxWidth) noexcept

    {
        return process_only_complex_unicode(eventHandler, state, text.data(), text.data() + text.size(), maxWidth);
    }

} // namespace detail
// }}}

template <typename...>
class grapheme_line_segmenter;

// Segments UTF-8 encoded text into sequences of grapheme clusters up to a given total width.
//
// This grapheme cluster segmenter is stateful and can be used to scan text in chunks.
// It can resume scanning where it left off.
//
// A segment consists of a sequence of zero or more grapheme clusters,
// including zero or more ASCII characters, and including invalid UTF-8 sequences.
//
// Invalid UTF-8 sequences are reported as a single invalid grapheme cluster with
// an east asian width Narrow (width 1), because when used for rendering, they'll be
// rendered as a single replacement character (U+FFFD).
//
// A sequence of grapheme clusters will never contain control characters,
// and process() will never process control characters but stop at the first one.
//
// Use move_forward_to() to move the internal state forward to a given position,
// e.g. to skip over control characters.
template <OptionalGraphemeSegmentationListenerConcept OptionalEventListener>
class grapheme_line_segmenter<OptionalEventListener>
{
  public:
    grapheme_line_segmenter() = default;

    template <GraphemeSegmentationListenerConcept Listener>
    explicit grapheme_line_segmenter(Listener& listener, std::string_view text) noexcept: _eventHandler { listener }
    {
        if (!text.empty())
            reset(text);
    }

    // Resets the grapheme line segmenter and re-initializes it with the given buffer.
    //
    // @param buffer the buffer to scan. Its underlying storage must be used by the
    //               subsequent calls to process()
    void reset(std::string_view buffer) noexcept
    {
        _buffer = buffer;

        _complexUnicodeState = {
            .utf8DecodeNext = buffer.data(),
            .currentClusterStart = buffer.data(),
            .currentCodepointStart = buffer.data(),
        };
    }

    // Expands the internal buffer by the given number of bytes.
    //
    // @param size the number of bytes to expand the buffer by
    void expand_buffer_by(size_t size) noexcept
    {
        assert(size > 0);
        _buffer = std::string_view(_buffer.data(), _buffer.size() + size);
    }

    // Moves the internal state forward to the given position.
    //
    // A call to this function will also reset the internal UTF-8 decoding state,
    // and will reset the last codepoint hint.
    //
    // @p pos must be a pointer to a position within the current buffer
    void move_forward_to(char const* pos) noexcept
    {
        assert(_buffer.data() <= pos && pos <= _buffer.data() + _buffer.size());
        _complexUnicodeState = {
            .utf8DecodeNext = pos,
            .currentClusterStart = pos,
            .currentCodepointStart = pos,
            .currentClusterWidth = 0,
            .lastCodepointHint = 0,
        };
    }

    // Processes the given text.
    //
    // Subsequent calls to this function will continue processing the text
    // where the previous call left off.
    //
    // @param maxWidth the maximum number of width to fill in the current line
    auto process(unsigned maxWidth) noexcept -> grapheme_segmentation_result
    {
        // Points to the beginning of a grapheme cluster.
        char const* const resultStart = _complexUnicodeState.currentClusterStart;

        if (_complexUnicodeState.utf8DecodeNext == end())
            return { .text = {}, .width = 0, .stop_condition = StopCondition::EndOfInput };

        // Total number of widths used in the current line.
        unsigned processedTotalWidth = 0;

        enum class State
        {
            ASCII,
            ComplexUnicode,
        };

        while (true)
        {
            if (next() == end())
                return { .text = std::string_view { resultStart, _complexUnicodeState.currentClusterStart },
                         .width = processedTotalWidth,
                         .stop_condition = StopCondition::EndOfInput };

            if (processedTotalWidth == maxWidth)
                return { .text = std::string_view { resultStart, _complexUnicodeState.currentClusterStart },
                         .width = processedTotalWidth,
                         .stop_condition = StopCondition::EndOfWidth };
            assert(processedTotalWidth < maxWidth);

            if (detail::is_control(*next()))
                return { .text = std::string_view { resultStart, _complexUnicodeState.currentClusterStart },
                         .width = processedTotalWidth,
                         .stop_condition = StopCondition::UnexpectedInput };

            switch (detail::is_complex(*next()) ? State::ComplexUnicode : State::ASCII)
            {
                case State::ASCII: {
                    assert(_complexUnicodeState.utf8.expectedLength == 0);
                    assert(_complexUnicodeState.utf8.currentLength == 0);
                    auto const text = std::string_view { next(), std::min(end(), next() + maxWidth) };
                    auto const [stop, consumedEnd] = detail::process_only_ascii(text.data(), text.data() + text.size());
                    if (consumedEnd == text.data())
                        return { .text = std::string_view { resultStart, consumedEnd },
                                 .width = processedTotalWidth,
                                 .stop_condition = stop };
                    auto const consumedWidth = static_cast<unsigned>(std::distance(text.data(), consumedEnd));
                    _eventHandler.on_ascii(text.substr(0, consumedWidth));
                    _complexUnicodeState.utf8DecodeNext = consumedEnd;
                    _complexUnicodeState.currentClusterStart = _complexUnicodeState.utf8DecodeNext;
                    _complexUnicodeState.currentCodepointStart = _complexUnicodeState.utf8DecodeNext;
                    _complexUnicodeState.lastCodepointHint = 0;
                    // Note, we don't store the actual previous codepoint hint here, because
                    // all previous grapheme clusters (US-ASCII here) have been emitted already.
                    processedTotalWidth += consumedWidth;
                    break;
                }
                case State::ComplexUnicode: {
                    // We know we have complex UTF-8 codepoints here.
                    // We need to process them one by one to determine their width.
                    // We also need to check if the current grapheme cluster fits into the current line.
                    // If not, we need to stop processing and return the current line.
                    // If the input is exhausted, we need to return the current line.
                    assert(processedTotalWidth < maxWidth);
                    auto const sub = detail::process_only_complex_unicode(
                        _eventHandler, _complexUnicodeState, _complexUnicodeState.utf8DecodeNext, end(), maxWidth);
                    processedTotalWidth += sub.totalWidth;
                    if (sub.stop_condition != StopCondition::UnexpectedInput)
                        // The most recent grapheme cluster does not fit into the current line or the input is exhausted.
                        return { .text = std::string_view { resultStart, _complexUnicodeState.currentClusterStart },
                                 .width = processedTotalWidth,
                                 .stop_condition = sub.stop_condition };
                    break;
                }
            }
        }
    }

    // Flushes out pending grapheme cluster, if any.
    //
    // @param maxWidth the maximum number of widths to fill in the current line
    //
    // @note A call to this function is idempotent.
    //
    // @returns the result of the processing
    grapheme_segmentation_result flush(unsigned maxWidth) noexcept
    {
        return flush_complex_unicode(_eventHandler, _complexUnicodeState, maxWidth);
    }

    // Processes a single byte.
    //
    // Subsequent calls to this function will continue processing the text
    //
    // @param byte the byte to process
    //
    // @return the result of the processing
    ConvertResult process_single_byte(uint8_t byte) noexcept { return from_utf8(_complexUnicodeState.utf8, byte); }

    // Tests whether there is an incomplete UTF-8 codepoint pending.
    bool is_utf8_byte_pending() const noexcept { return _complexUnicodeState.utf8.expectedLength > 0; }

    // Returns a copy of the current UTF-8 decoding state.
    [[nodiscard]] utf8_decoder_state utf8_state() const noexcept { return _complexUnicodeState.utf8; }

    // Returns the last processed UTF-32 codepoint.
    constexpr char32_t last_codepoint_hint() const noexcept { return _complexUnicodeState.lastCodepointHint; }

    // Resets the last codepoint hint.
    constexpr void reset_last_codepoint_hint(char32_t value = 0) noexcept { _complexUnicodeState.lastCodepointHint = value; }

    void update_next_utf8_decode(char const* next) noexcept { _complexUnicodeState.utf8DecodeNext = next; }
    char const* next() const noexcept { return _complexUnicodeState.utf8DecodeNext; }
    char const* end() const noexcept { return _buffer.data() + _buffer.size(); }

    static std::pair<StopCondition, char const*> process_only_ascii(std::string_view text) noexcept
    {
        return detail::process_only_ascii(text.data(), text.data() + text.size());
    }

  private:
    detail::unicode_process_state _complexUnicodeState {};

    // The buffer to scan. Its underlying storage must be used by the subsequent calls to process().
    // While consuming the buffer, the front of the buffer will be moved forward.
    std::string_view _buffer;

    detail::EventHandler<OptionalEventListener> _eventHandler;
};

template <>
class grapheme_line_segmenter<>: public grapheme_line_segmenter<void>
{
  public:
    grapheme_line_segmenter() = default;
};

template <GraphemeSegmentationListenerConcept Listener>
grapheme_line_segmenter(Listener) -> grapheme_line_segmenter<Listener>;

template <GraphemeSegmentationListenerConcept Listener>
grapheme_line_segmenter(Listener&, std::string_view) -> grapheme_line_segmenter<Listener>;

} // namespace unicode

// {{{ ostream support
namespace std
{
inline std::ostream& operator<<(std::ostream& os, unicode::StopCondition value)
{
    std::string_view name;
    switch (value)
    {
        case unicode::StopCondition::UnexpectedInput: name = "UnexpectedInput"; break;
        case unicode::StopCondition::EndOfInput: name = "EndOfInput"; break;
        case unicode::StopCondition::EndOfWidth: name = "EndOfWidth"; break;
        default: name = "INVALID"; break;
    }
    return os << name;
}

inline std::ostream& operator<<(std::ostream& os, unicode::grapheme_segmentation_result const& value)
{
    return os << "{text: \"" << std::string(value.text) << "\", width: " << value.width << ", stop: " << value.stop_condition
              << "}";
}
} // namespace std
// }}}
