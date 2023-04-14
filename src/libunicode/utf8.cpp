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

ConvertResult from_utf8(utf8_decoder_state& _state, uint8_t _byte)
{
    if (!_state.expectedLength)
    {
        if ((_byte & 0b1000'0000) == 0)
        {
            _state.currentLength = 1;
            return Success { _byte };
        }
        else if ((_byte & 0b1110'0000) == 0b1100'0000)
        {
            _state.currentLength = 1;
            _state.expectedLength = 2;
            _state.character = _byte & 0b0001'1111;
        }
        else if ((_byte & 0b1111'0000) == 0b1110'0000)
        {
            _state.currentLength = 1;
            _state.expectedLength = 3;
            _state.character = _byte & 0b0000'1111;
        }
        else if ((_byte & 0b1111'1000) == 0b1111'0000)
        {
            _state.currentLength = 1;
            _state.expectedLength = 4;
            _state.character = _byte & 0b0000'0111;
        }
        else
        {
            _state.currentLength = 1;
            return Invalid {};
        }
    }
    else
    {
        _state.character <<= 6;
        _state.character |= _byte & 0b0011'1111;
        _state.currentLength++;
    }

    if (_state.currentLength < _state.expectedLength)
        return { Incomplete {} };

    _state.expectedLength = 0; // reset state
    return { Success { _state.character } };
}

} // namespace unicode
