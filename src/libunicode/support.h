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
#include <functional>

namespace unicode
{

// Generally, the compiler may decide to inline or not,
// but when debugging, we want to make sure that certain functions are not inlined.
#if !defined(NDEBUG)
    #if defined(__GNUC__) || defined(__clang__)
        #define LIBUNICODE_INLINE __attribute__((noinline)) inline
    #elif defined(_MSC_VER)
        #define LIBUNICODE_INLINE __declspec(noinline) inline
    #else
        #define LIBUNICODE_INLINE inline
    #endif
#else
    #define LIBUNICODE_INLINE inline
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define LIBUNICODE_PACKED __attribute__((packed))
#else
    #define LIBUNICODE_PACKED
#endif

#if defined(__cpp_char8_t)
using char8_type = char8_t;
#else
using char8_type = uint8_t;
#endif

// Helper API solely for use of function parameters to visually denote output parameters.
template <typename T>
class out
{
  public:
    constexpr out(std::reference_wrapper<T> ref) noexcept: _ref { &ref.value() } {}
    constexpr explicit out(T& ref) noexcept: _ref { &ref } {}
    constexpr out(out const&) noexcept = default;
    constexpr out(out&&) noexcept = default;
    constexpr out& operator=(out const&) noexcept = default;
    constexpr out& operator=(out&&) noexcept = default;

    constexpr T& get() noexcept { return *_ref; }
    constexpr T const& get() const noexcept { return *_ref; }

    constexpr T& operator*() noexcept { return *_ref; }
    constexpr T const& operator*() const noexcept { return *_ref; }

    constexpr T* operator->() noexcept { return _ref; }
    constexpr T const* operator->() const noexcept { return _ref; }

    constexpr void assign(T value) { *_ref = std::move(value); }

  private:
    T* _ref;
};

// dynamic array with a fixed capacity.
template <typename T, std::size_t N>
class fs_array
{
  public:
    using value_type = T;
    using array_type = std::array<value_type, N>;
    using iterator = typename array_type::iterator;
    using const_iterator = typename array_type::const_iterator;

    constexpr fs_array() noexcept: values_ { {} }, size_ { 0 } {}

    constexpr void clear() noexcept
    {
        for (size_t i = 0; i < size_; ++i)
            values_[i].~T();
        size_ = 0;
    }

    constexpr size_t capacity() const noexcept { return N; }
    constexpr size_t size() const noexcept { return size_; }
    constexpr bool empty() const noexcept { return size_ == 0; }

    constexpr bool push_back(T value) noexcept
    {
        if (size_ == N)
            return false;
        values_[size_++] = std::move(value);
        return true;
    }

    constexpr void resize(size_t n) noexcept
    {
        if (n < N)
            size_ = n;
    }

    constexpr T const& operator[](size_t i) const noexcept { return values_[i]; }
    constexpr T const& at(size_t i) const noexcept { return values_.at(i); }

    constexpr iterator begin() noexcept { return values_.begin(); }
    constexpr iterator end() noexcept
    {
        using SizeT = typename std::iterator_traits<decltype(values_.begin())>::difference_type;
        return std::next(values_.begin(), static_cast<SizeT>(size_));
    }

    constexpr const_iterator begin() const noexcept { return values_.begin(); }
    constexpr const_iterator end() const noexcept
    {
        using SizeT = typename std::iterator_traits<decltype(values_.begin())>::difference_type;
        return std::next(values_.begin(), static_cast<SizeT>(size_));
    }

    constexpr T* data() noexcept { return values_.data(); }
    constexpr T const* data() const noexcept { return values_.data(); }

    constexpr T& front() noexcept { return at(0); }
    constexpr T const& front() const noexcept { return at(0); }

    constexpr T& back() noexcept { return at(size_ - 1); }
    constexpr T const& back() const noexcept { return at(size_ - 1); }

  private:
    array_type values_;
    size_t size_;
};

} // namespace unicode
