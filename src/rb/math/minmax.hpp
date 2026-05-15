#pragma once

#include <rb/core/helpers.hpp>
#include <rb/core/traits/builtins.hpp>
#include <rb/core/traits/requirements.hpp>

namespace rb::math {

template <class T>
inline constexpr bool isScalarLike =
    core::isTriviallyCopyable<T>
    && sizeof(T) <= 2 * sizeof(void*);

// 1 arg

template <class T>
[[nodiscard]] constexpr RB_REQUIRES_RETURN(T, isScalarLike<T>)
max(T value) noexcept {
	return value;
}

template <class T>
[[nodiscard]] constexpr RB_REQUIRES_RETURN(T const&, !isScalarLike<T>)
max(T const& value) noexcept {
	return value;
}

template <class T>
RB_REQUIRES_RETURN(void, !isScalarLike<T>)
max(T const&& value) = delete;

template <class T>
[[nodiscard]] constexpr RB_REQUIRES_RETURN(T, isScalarLike<T>)
min(T value) noexcept {
	return value;
}

template <class T>
[[nodiscard]] constexpr RB_REQUIRES_RETURN(T const&, !isScalarLike<T>)
min(T const& value) noexcept {
	return value;
}

template <class T>
RB_REQUIRES_RETURN(void, !isScalarLike<T>)
min(T const&& value) = delete;

// 2 arg

template <class T>
[[nodiscard]] constexpr RB_REQUIRES_RETURN(T, isScalarLike<T>&& core::isLessThanComparable<T>)
max(T lhs, T rhs) RB_NOEXCEPT_LIKE(lhs < rhs) {
	return lhs < rhs ? rhs : lhs;
}

template <class T>
[[nodiscard]] constexpr RB_REQUIRES_RETURN(T const&, !isScalarLike<T> && core::isLessThanComparable<T>)
max(T const& lhs, T const& rhs) RB_NOEXCEPT_LIKE(lhs < rhs) {
	return lhs < rhs ? rhs : lhs;
}

template <class T>
RB_REQUIRES_RETURN(void, !isScalarLike<T>)
max(T const& lhs, T const&& rhs) = delete;

template <class T>
RB_REQUIRES_RETURN(void, !isScalarLike<T>)
max(T const&& lhs, T const& rhs) = delete;

template <class T>
[[nodiscard]] constexpr RB_REQUIRES_RETURN(T, isScalarLike<T>&& core::isLessThanComparable<T>)
min(T lhs, T rhs) RB_NOEXCEPT_LIKE(lhs < rhs) {
	return lhs < rhs ? lhs : rhs;
}

template <class T>
[[nodiscard]] constexpr RB_REQUIRES_RETURN(T const&, !isScalarLike<T> && core::isLessThanComparable<T>)
min(T const& lhs, T const& rhs) RB_NOEXCEPT_LIKE(lhs < rhs) {
	return lhs < rhs ? lhs : rhs;
}

template <class T>
RB_REQUIRES_RETURN(void, !isScalarLike<T>)
min(T const& lhs, T const&& rhs) = delete;

template <class T>
RB_REQUIRES_RETURN(void, !isScalarLike<T>)
min(T const&& lhs, T const& rhs) = delete;

// 2+ arg

template <class T, class... Args>
[[nodiscard]] constexpr RB_REQUIRES_RETURN(T, isScalarLike<T>&& core::isLessThanComparable<T>)
max(T first, T second, Args... args) RB_NOEXCEPT_LIKE(first < second) {
	return max(max(first, second), args...);
}

template <class T, class U, class V, class... Args>
[[nodiscard]] constexpr RB_REQUIRES_RETURN(T const&, !isScalarLike<T> && core::isLessThanComparable<T>)
max(T& first, U& second, V& third, Args&... args) RB_NOEXCEPT_LIKE(first < second) {
	return max(max(first, second), third, args...);
}

template <class T, class... Args>
[[nodiscard]] constexpr RB_REQUIRES_RETURN(T, isScalarLike<T>&& core::isLessThanComparable<T>)
min(T first, T second, Args... args) RB_NOEXCEPT_LIKE(first < second) {
	return min(min(first, second), args...);
}

template <class T, class U, class V, class... Args>
[[nodiscard]] constexpr RB_REQUIRES_RETURN(T const&, !isScalarLike<T> && core::isLessThanComparable<T>)
min(T& first, U& second, V& third, Args&... args) RB_NOEXCEPT_LIKE(first < second) {
	return min(min(first, second), third, args...);
}

} // namespace rb::math
