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
#include <libunicode/utf8.h>

namespace unicode
{

ConvertResult from_utf8(utf8_decoder_state& state, uint8_t value) noexcept
{
    if (!state.expectedLength)
    {
        if ((value & 0b1000'0000) == 0)
        {
            state.currentLength = 1;
            return Success { value };
        }
        else if ((value & 0b1110'0000) == 0b1100'0000)
        {
            state.currentLength = 1;
            state.expectedLength = 2;
            state.character = value & 0b0001'1111;
        }
        else if ((value & 0b1111'0000) == 0b1110'0000)
        {
            state.currentLength = 1;
            state.expectedLength = 3;
            state.character = value & 0b0000'1111;
        }
        else if ((value & 0b1111'1000) == 0b1111'0000)
        {
            state.currentLength = 1;
            state.expectedLength = 4;
            state.character = value & 0b0000'0111;
        }
        else
        {
            state.currentLength = 1;
            state.expectedLength = 0;
            return Invalid {};
        }
    }
    // clang-format off
    else if ((value & 0b1110'0000) == 0b1100'0000
          || (value & 0b1111'0000) == 0b1110'0000
          || (value & 0b1111'1000) == 0b1111'0000)
    // clang-format on
    {
        // We have a new codepoint, but the previous one was incomplete.
        state.expectedLength = 0;
        // Return Invalid for the current incomplete codepoint,
        // but have already started the next codepoint.
        from_utf8(state, value);
        return { Invalid {} };
    }
    else
    {
        state.character <<= 6;
        state.character |= value & 0b0011'1111;
        state.currentLength++;
    }

    if (state.currentLength < state.expectedLength)
        return { Incomplete {} };

    state.expectedLength = 0; // reset state
    return { Success { state.character } };
}

} // namespace unicode
