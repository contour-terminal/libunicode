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

namespace unicode {

struct Interval
{
	char32_t from;
	char32_t to;
};

template <size_t N>
constexpr bool contains(std::array<Interval, N> const& _ranges, char32_t _codepoint)
{
	auto a = size_t{0};
	auto b = static_cast<size_t>(_ranges.size()) - 1;
	while (a < b)
	{
		auto const i = static_cast<size_t>((b + a) / 2);
		auto const& I = _ranges[i];
		if (I.to < _codepoint)
			a = i + 1;
		else if (I.from > _codepoint)
			b = i - 1;
		else
			return true;
	}
	return a == b && _ranges[a].from <= _codepoint && _codepoint <= _ranges[a].to;
}

template <typename T> struct Prop
{
	Interval interval;
	T property;
};

template <typename T, size_t N>
constexpr std::optional<T> search(std::array<Prop<T>, N> const& _ranges, char32_t _codepoint)
{
	auto a = size_t{0};
	auto b = static_cast<size_t>(_ranges.size()) - 1;

	while (a < b)
	{
		auto const i = static_cast<size_t>((b + a) / 2);
		auto const& I = _ranges[i];
		if (I.interval.to < _codepoint)
			a = i + 1;
		else if (I.interval.from > _codepoint)
			b = i - 1;
		else
			return I.property;
	}

	if (a == b && _ranges[a].interval.from <= _codepoint && _codepoint <= _ranges[a].interval.to)
		return _ranges[a].property;

	return std::nullopt;
}

} // end namespace
