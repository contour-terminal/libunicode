#include "scan_simd_impl.hpp"

namespace unicode::detail
{
size_t scan_for_text_ascii_512(std::string_view text, size_t maxColumnCount) noexcept
{
    return scan_for_text_ascii_simd<512>(text, maxColumnCount);
}
} // namespace unicode::detail
