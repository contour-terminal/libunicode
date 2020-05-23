#include <unicode/text_segmenter.h>

#include <algorithm>
#include <functional>

using namespace std;

namespace unicode {

text_segmenter::text_segmenter(char32_t const* _data,
                               size_t _size,
                               size_t _startOffset)
    : startOffset_{_startOffset},
      size_{_size},
      scriptSegmenter_{_data, _size},
      emojiSegmenter_{_data, _size}
{
}

bool text_segmenter::consume(out<segment> _result)
{
    if (finished())
        return false;

    consumeUntilSplitPosition(scriptSegmenter_, out(scriptRunPosition_), out(candidate_.script));
    consumeUntilSplitPosition(emojiSegmenter_, out(emojiRunPosition_), out(candidate_.emoji));

    auto const positions = {scriptRunPosition_, emojiRunPosition_};
    auto const minPosition = min_element(begin(positions), end(positions));

    lastSplit_ = *minPosition;

    candidate_.start = candidate_.end;
    candidate_.end = lastSplit_ - startOffset_;

    *_result = candidate_;
    return true;
}

template <typename Segmenter, typename Property>
void text_segmenter::consumeUntilSplitPosition(Segmenter& _segmenter,
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
