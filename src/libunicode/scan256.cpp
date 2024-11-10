// SPDX-License-Identifier: Apache-2.0
#include <libunicode/scan.h>
#include <libunicode/scan_simd_impl.h>

namespace unicode::detail
{
size_t scan_for_text_ascii_256(std::string_view text, size_t maxColumnCount) noexcept
{
    return scan_for_text_ascii_simd<256>(text, maxColumnCount);
}
} // namespace unicode::detail
