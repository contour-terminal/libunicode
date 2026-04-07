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

/// Computes the display width of a single grapheme cluster.
///
/// Variation selectors are respected:
/// - VS16 (U+FE0F) forces the cluster width to 2 (emoji presentation).
/// - VS15 (U+FE0E) forces the cluster width to 1 (text presentation).
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
