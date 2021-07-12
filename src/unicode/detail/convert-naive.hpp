#pragma once

#include <unicode/detail/convert-common.hpp>

namespace unicode::accelerator
{
    struct naive
    {
        static constexpr std::size_t alignment = 1;
    };
};

namespace unicode::detail
{

template <>
LIBUNICODE_FORCE_INLINE
void convertAsciiBlockOnce<accelerator::naive>(unsigned char const*& _input, char32_t*& _output) noexcept
{
    for (size_t i = 0; i < accelerator::naive::alignment && is_ascii(*_input); ++i)
        *_output++ = static_cast<uint32_t>(*_input++);
}

}
