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
#include <libunicode/word_segmenter.h>

#include <optional>

namespace unicode
{

namespace
{
    /// AHLetter = ALetter | Hebrew_Letter
    bool is_AHLetter(Word_Break wb) noexcept
    {
        return wb == Word_Break::ALetter || wb == Word_Break::Hebrew_Letter;
    }

    /// MidNumLetQ = MidNumLet | Single_Quote
    bool is_MidNumLetQ(Word_Break wb) noexcept
    {
        return wb == Word_Break::MidNumLet || wb == Word_Break::Single_Quote;
    }

    /// MidLetter or MidNumLetQ
    bool is_MidLetterLike(Word_Break wb) noexcept
    {
        return wb == Word_Break::MidLetter || is_MidNumLetQ(wb);
    }

    /// MidNum or MidNumLetQ
    bool is_MidNumLike(Word_Break wb) noexcept
    {
        return wb == Word_Break::MidNum || is_MidNumLetQ(wb);
    }

    /// Whether a Word_Break value is "transparent" per WB4: Extend, Format, or ZWJ.
    bool is_transparent(Word_Break wb) noexcept
    {
        return wb == Word_Break::Extend || wb == Word_Break::Format || wb == Word_Break::ZWJ;
    }

    /// Whether a Word_Break value is a newline-class value (Newline, CR, LF).
    bool is_newline_class(Word_Break wb) noexcept
    {
        return wb == Word_Break::Newline || wb == Word_Break::CR || wb == Word_Break::LF;
    }

    /// Peek past transparent (Extend/Format/ZWJ) codepoints and return the
    /// Word_Break property of the next non-transparent codepoint, if any.
    std::optional<Word_Break> peek_next_non_transparent(char32_t const* rest, size_t count) noexcept
    {
        for (size_t i = 0; i < count; ++i)
        {
            auto const wb = codepoint_properties::get(rest[i]).word_break;
            if (!is_transparent(wb))
                return wb;
        }
        return std::nullopt;
    }

    /// Update state when a new non-transparent codepoint is encountered.
    void update_effective(Word_Break wb, word_segmenter_state& state) noexcept
    {
        state.prev_prev_wb = state.prev_wb;
        state.prev_wb = wb;
        state.raw_prev_wb = wb;
        state.ri_counter = (wb == Word_Break::Regional_Indicator) ? static_cast<uint8_t>(state.ri_counter + 1) : 0;
        state.saw_transparent = false;
    }

} // namespace

void word_process_init(char32_t firstCodepoint, word_segmenter_state& state) noexcept
{
    auto const props = codepoint_properties::get(firstCodepoint);
    auto const wb = props.word_break;

    state.raw_prev_wb = wb;
    state.prev_prev_wb = Word_Break::Other;

    if (is_transparent(wb))
    {
        // Transparent characters as first codepoint: keep prev_wb as Other,
        // but raw_prev_wb tracks the actual character.
        state.prev_wb = Word_Break::Other;
        state.ri_counter = 0;
    }
    else
    {
        state.prev_wb = wb;
        state.ri_counter = (wb == Word_Break::Regional_Indicator) ? 1 : 0;
    }
}

bool word_process_breakable(char32_t nextCodepoint, char32_t const* rest, size_t restCount, word_segmenter_state& state) noexcept
{
    auto const props = codepoint_properties::get(nextCodepoint);
    auto const B = props.word_break;

    // --- Rules evaluated BEFORE WB4 (on raw previous) ---

    // WB3: CR x LF
    if (state.raw_prev_wb == Word_Break::CR && B == Word_Break::LF)
    {
        update_effective(B, state);
        return false;
    }

    // WB3a: (Newline | CR | LF) ÷
    if (is_newline_class(state.raw_prev_wb))
    {
        if (is_transparent(B))
        {
            state.raw_prev_wb = B;
            state.prev_wb = Word_Break::Other;
            state.prev_prev_wb = Word_Break::Other;
            state.ri_counter = 0;
        }
        else
        {
            update_effective(B, state);
        }
        return true;
    }

    // WB3b: ÷ (Newline | CR | LF)
    if (is_newline_class(B))
    {
        update_effective(B, state);
        return true;
    }

    // WB3c: ZWJ x \p{Extended_Pictographic}
    if (state.raw_prev_wb == Word_Break::ZWJ && props.is_extended_pictographic())
    {
        update_effective(B, state);
        return false;
    }

    // WB4: Ignore Extend, Format, ZWJ for subsequent rules
    // (except after Newline/CR/LF which was already handled above)
    if (is_transparent(B))
    {
        state.raw_prev_wb = B;
        state.saw_transparent = true;
        // Do NOT update prev_wb/prev_prev_wb — B is transparent
        return false;
    }

    // --- From here, B is non-transparent. Use effective state for rules. ---
    auto const A = state.prev_wb;

    // WB3d: WSegSpace x WSegSpace
    // Only applies when directly adjacent (no intervening Extend/Format/ZWJ).
    if (A == Word_Break::WSegSpace && B == Word_Break::WSegSpace && !state.saw_transparent)
    {
        update_effective(B, state);
        return false;
    }

    // WB5: AHLetter x AHLetter
    if (is_AHLetter(A) && is_AHLetter(B))
    {
        update_effective(B, state);
        return false;
    }

    // WB6: AHLetter x (MidLetter | MidNumLetQ) AHLetter
    if (is_AHLetter(A) && is_MidLetterLike(B))
    {
        auto const next = peek_next_non_transparent(rest, restCount);
        if (next && is_AHLetter(*next))
        {
            update_effective(B, state);
            return false;
        }
    }

    // WB7: AHLetter (MidLetter | MidNumLetQ) x AHLetter
    if (is_MidLetterLike(A) && is_AHLetter(B) && is_AHLetter(state.prev_prev_wb))
    {
        update_effective(B, state);
        return false;
    }

    // WB7a: Hebrew_Letter x Single_Quote
    if (A == Word_Break::Hebrew_Letter && B == Word_Break::Single_Quote)
    {
        update_effective(B, state);
        return false;
    }

    // WB7b: Hebrew_Letter x Double_Quote Hebrew_Letter
    if (A == Word_Break::Hebrew_Letter && B == Word_Break::Double_Quote)
    {
        auto const next = peek_next_non_transparent(rest, restCount);
        if (next && *next == Word_Break::Hebrew_Letter)
        {
            update_effective(B, state);
            return false;
        }
    }

    // WB7c: Hebrew_Letter Double_Quote x Hebrew_Letter
    if (A == Word_Break::Double_Quote && B == Word_Break::Hebrew_Letter && state.prev_prev_wb == Word_Break::Hebrew_Letter)
    {
        update_effective(B, state);
        return false;
    }

    // WB8: Numeric x Numeric
    if (A == Word_Break::Numeric && B == Word_Break::Numeric)
    {
        update_effective(B, state);
        return false;
    }

    // WB9: AHLetter x Numeric
    if (is_AHLetter(A) && B == Word_Break::Numeric)
    {
        update_effective(B, state);
        return false;
    }

    // WB10: Numeric x AHLetter
    if (A == Word_Break::Numeric && is_AHLetter(B))
    {
        update_effective(B, state);
        return false;
    }

    // WB11: Numeric (MidNum | MidNumLetQ) x Numeric
    if (is_MidNumLike(A) && B == Word_Break::Numeric && state.prev_prev_wb == Word_Break::Numeric)
    {
        update_effective(B, state);
        return false;
    }

    // WB12: Numeric x (MidNum | MidNumLetQ) Numeric
    if (A == Word_Break::Numeric && is_MidNumLike(B))
    {
        auto const next = peek_next_non_transparent(rest, restCount);
        if (next && *next == Word_Break::Numeric)
        {
            update_effective(B, state);
            return false;
        }
    }

    // WB13: Katakana x Katakana
    if (A == Word_Break::Katakana && B == Word_Break::Katakana)
    {
        update_effective(B, state);
        return false;
    }

    // WB13a: (AHLetter | Numeric | Katakana | ExtendNumLet) x ExtendNumLet
    if (B == Word_Break::ExtendNumLet
        && (is_AHLetter(A) || A == Word_Break::Numeric || A == Word_Break::Katakana || A == Word_Break::ExtendNumLet))
    {
        update_effective(B, state);
        return false;
    }

    // WB13b: ExtendNumLet x (AHLetter | Numeric | Katakana)
    if (A == Word_Break::ExtendNumLet && (is_AHLetter(B) || B == Word_Break::Numeric || B == Word_Break::Katakana))
    {
        update_effective(B, state);
        return false;
    }

    // WB15/WB16: Do not break within emoji flag sequences.
    // sot (RI RI)* RI x RI  /  [^RI] (RI RI)* RI x RI
    if (A == Word_Break::Regional_Indicator && B == Word_Break::Regional_Indicator && (state.ri_counter % 2) == 1)
    {
        update_effective(B, state);
        return false;
    }

    // WB999: Otherwise, break everywhere.
    update_effective(B, state);
    return true;
}

} // namespace unicode
