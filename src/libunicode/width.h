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

#include <string_view>

namespace unicode
{

/// Returns the number of text columns the given codepoint would need to be displayed.
unsigned width(char32_t codepoint) noexcept;

/// Measures a single grapheme cluster one codepoint at a time.
///
/// This is the one place a cluster's width is decided. grapheme_cluster_width() drives it over a
/// complete cluster, and scan_text() drives it as it decodes, so the two cannot drift apart on how
/// wide the same text is -- a disagreement that shows up as a terminal drawing text one column
/// narrower than it reserved cells for.
///
/// Feed a cluster's codepoints in order with push(); read width() at any point for the width of what
/// has been fed so far; call reset() to begin the next cluster.
class grapheme_cluster_width_accumulator
{
  public:
    /// Feeds the next codepoint of the current grapheme cluster.
    void push(char32_t codepoint) noexcept;

    /// Width, in columns, of everything pushed since construction or the last reset().
    [[nodiscard]] unsigned width() const noexcept;

    /// Begins a new grapheme cluster, discarding all accumulated state.
    void reset() noexcept { *this = {}; }

  private:
    // A variation selector or spacing mark re-presents the codepoint most recently *measured*, which
    // is not always the one just before it: zero-width members are stepped over, and a selector that
    // has already been applied must not apply a second time. Tracking that codepoint, its width, and
    // whether it is still open to modification is what keeps this in step with wcwidth.
    char32_t _lastMeasured = 0;
    int _lastMeasuredWidth = 0;
    bool _lastMeasuredIsOpen = false; // false blocks VS15/VS16: nothing to re-present

    // Signed, because VS15 settles its correction against the running total the way wcwidth does.
    int _total = 0;   // width of the segments already closed off
    int _current = 0; // width of the segment being accumulated

    bool _previousWasVirama = false;
    bool _skipNext = false; // a ZWJ consumes the codepoint that follows it
    unsigned _regionalIndicators = 0;
};

/// Computes the display width of a single grapheme cluster.
///
/// A cluster is not simply as wide as its base codepoint: later members can widen it.
/// - VS16 (U+FE0F) requests the emoji presentation, which is two columns -- but only for a base an
///   emoji variation sequence is defined for. `✔️` (U+2714) is two columns; `✓️` (U+2713, which has
///   no variation sequence) stays one, as does a variation selector after ordinary text.
/// - VS15 (U+FE0E) requests the text presentation, which is one column, under the same restriction.
///   It does not narrow a CJK ideograph, a Hangul jamo, or a cluster widened by a spacing mark.
/// - A spacing mark (General_Category=Mc) takes room of its own beside its base, unlike a
///   non-spacing mark, so `का` (ka + AA matra) is two columns while `कं` (ka + anusvara) is one.
/// - A consonant joined to the previous one through a virama forms a conjunct that is two columns
///   wide: `क्न` is two, but a dangling `क्` with nothing behind the virama is one.
/// - An emoji modifier (skin tone) is absorbed by the emoji before it and adds nothing, whether or
///   not that emoji is Emoji_Modifier_Base.
/// - ZWJ (U+200D) does NOT widen. It consumes the codepoint that follows it, so an emoji ZWJ
///   sequence is measured by its first segment: U+2764 U+200D U+1F525 (heart on fire, written
///   without VS16) is one column. RGI sequences reach two columns through VS16 or a wide base.
///
/// There is no clamp at two columns. A cluster holding several codepoints that each take room is as
/// wide as their sum, which is what wcwidth reports: two Hangul Choseong jamo form one cluster four
/// columns wide.
///
/// This is a port of Python wcwidth 0.8's wcswidth(), which terminal applications measure against,
/// rather than an approximation of it -- an earlier approximation agreed with it on only 99% of a
/// 3607-grapheme corpus. The cluster algorithm agrees with wcswidth() on every cluster tested,
/// including all of GraphemeBreakTest.txt and every emoji variation sequence.
///
/// The underlying per-codepoint width() is deliberately NOT a wcwidth clone, so a cluster whose
/// codepoints hit one of those differences inherits it: libunicode reports one column where wcwidth
/// reports zero for unassigned Default_Ignorable codepoints (most of U+E01F0..U+E0FFF and
/// U+E0080..U+E00FF), and it measures the Hangul filler characters U+3164 and U+FFA0 as rendered
/// rather than as nothing. Those choices predate this function and are pinned by their own tests.
///
/// @param graphemeCluster a single grapheme cluster (e.g., as returned by grapheme_segmenter).
/// @return the display column width of the grapheme cluster.
unsigned grapheme_cluster_width(std::u32string_view graphemeCluster) noexcept;

/// Computes the total display width of a UTF-8 encoded string.
///
/// Performs grapheme cluster segmentation internally,
/// then sums up the width of each grapheme cluster.
///
/// @param utf8Text the UTF-8 encoded text.
/// @return the total display column width.
unsigned grapheme_cluster_width(std::string_view utf8Text) noexcept;

} // namespace unicode
