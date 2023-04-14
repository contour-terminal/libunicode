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

#include <array>
#include <optional>

namespace unicode
{

struct Interval // NOLINT(readability-identifier-naming)
{
    char32_t from;
    char32_t to;
};

template <size_t N>
constexpr bool contains(std::array<Interval, N> const& ranges, char32_t codepoint) noexcept
{
    auto a = size_t { 0 };
    auto b = static_cast<size_t>(ranges.size()) - 1;
    while (a < b)
    {
        auto const i = ((b + a) / 2);
        auto const& I = ranges[i];
        if (I.to < codepoint)
            a = i + 1;
        else if (I.from > codepoint)
        {
            if (i == 0)
                return false;
            b = i - 1;
        }
        else
            return true;
    }
    return a == b && ranges[a].from <= codepoint && codepoint <= ranges[a].to;
}

template <typename T>
struct Prop // NOLINT(readability-identifier-naming)
{
    Interval interval;
    T property;
};

template <typename T, size_t N>
constexpr std::optional<T> search(std::array<Prop<T>, N> const& ranges, char32_t codepoint)
{
    auto a = size_t { 0 };
    auto b = static_cast<size_t>(ranges.size()) - 1;

    while (a < b)
    {
        auto const i = static_cast<size_t>((b + a) / 2);
        auto const& I = ranges[i];
        if (I.interval.to < codepoint)
            a = i + 1;
        else if (I.interval.from > codepoint)
            b = i - 1;
        else
            return I.property;
    }

    if (a == b && ranges[a].interval.from <= codepoint && codepoint <= ranges[a].interval.to)
        return ranges[a].property;

    return std::nullopt;
}

} // namespace unicode
