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
#include <unicode/run_segmenter.h>

#include <algorithm>
#include <functional>

using namespace std;

namespace unicode {

run_segmenter::run_segmenter(char32_t const* _data,
                             size_t _size,
                             size_t _startOffset)
    : startOffset_{_startOffset},
      size_{_size},
      scriptSegmenter_{_data, _size},
      emojiSegmenter_{_data, _size}
{
}

bool run_segmenter::consume(out<range> _result)
{
    if (finished())
        return false;

    bool isEmoji = false;

    consumeUntilSplitPosition(scriptSegmenter_, out(scriptRunPosition_), out(candidate_.script));
    consumeUntilSplitPosition(emojiSegmenter_, out(emojiRunPosition_), out(isEmoji));
    // TODO: Orientation segmentation
    // TODO: word boundary segmentation ?

    auto const positions = {scriptRunPosition_, emojiRunPosition_};
    auto const minPosition = min_element(begin(positions), end(positions));

    lastSplit_ = *minPosition;

    candidate_.start = candidate_.end;
    candidate_.end = lastSplit_ - startOffset_;
    candidate_.presentationStyle = isEmoji ? PresentationStyle::Emoji : PresentationStyle::Text;

    *_result = candidate_;
    return true;
}

template <typename Segmenter, typename Property>
void run_segmenter::consumeUntilSplitPosition(Segmenter& _segmenter,
                                              out<size_t> _position,
                                              out<Property> _property)
{
    if (*_position > lastSplit_)
        return;

    if (*_position > size_)
        return;

    for (;;)
    {
        if (!_segmenter.consume(_position, _property))
            break;

        if (*_position > lastSplit_)
            break;
    }
}

} // end namespace
