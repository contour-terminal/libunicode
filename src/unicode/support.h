#pragma once

#include <functional>

namespace unicode {

// Helper API solely for use of function parameters to visually denote output parameters.
template <typename T>
class out {
  public:
    constexpr out(std::reference_wrapper<T> _ref) noexcept : ref_{&_ref.value()} {}
    constexpr explicit out(T& _ref) noexcept : ref_{&_ref} {}
    constexpr out(out const&) noexcept = default;
    constexpr out(out&&) noexcept = default;
    constexpr out& operator=(out const&) noexcept = default;
    constexpr out& operator=(out&&) noexcept = default;

    constexpr T& get() noexcept { return *ref_; }
    constexpr T const& get() const noexcept { return *ref_; }

    constexpr T& operator*() noexcept { return *ref_; }
    constexpr T const& operator*() const noexcept { return *ref_; }

    constexpr T* operator->() noexcept { return ref_; }
    constexpr T const* operator->() const noexcept { return ref_; }

    constexpr void assign(T _value) { *ref_ = std::move(_value); }

  private:
    T* ref_;
};

// dynamic array with a fixed capacity.
template <typename T, std::size_t N>
class fs_array {
  public:
    using value_type = T;
    using array_type = std::array<value_type, N>;
    using iterator = typename array_type::iterator;
    using const_iterator = typename array_type::const_iterator;

    constexpr fs_array() noexcept : values_{{}}, size_{0} {}

    constexpr void clear() noexcept {
        for (size_t i = 0; i < size_; ++i)
            values_[i].~T();
        size_ = 0;
    };

    constexpr size_t capacity() const noexcept { return N; }
    constexpr size_t size() const noexcept { return size_; }
    constexpr bool empty() const noexcept { return size_ == 0; }

    constexpr bool push_back(T _value) noexcept
    {
        if (size_ == N)
            return false;
        values_[size_++] = std::move(_value);
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
    constexpr iterator end() noexcept { return values_.end(); }

    constexpr const_iterator begin() const noexcept { return values_.begin(); }
    constexpr const_iterator end() const noexcept { return values_.end(); }

    constexpr T* data() noexcept { return values_.data(); }
    constexpr T const* data() const noexcept { return values_.data(); }

  private:
    array_type values_;
    size_t size_;
};

}
