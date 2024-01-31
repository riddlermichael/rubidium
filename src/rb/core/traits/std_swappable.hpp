#pragma once

#include <utility>

#include <rb/core/traits/ops.hpp>
#include <rb/core/traits/Void.hpp>

namespace rb::core {

namespace impl::std_swap {

	using std::swap;

	template <class T, class U, class = void>
	struct IsStdSwappableWith : False {};

	template <class T, class U>
	struct IsStdSwappableWith<T, U, Void<decltype(swap(std::declval<T>(), std::declval<U>()))>> : True {};

	template <class T, class U, class = void>
	struct IsNothrowStdSwappableWith : False {};

	template <class T, class U>
	struct IsNothrowStdSwappableWith<T, U, Void<decltype(swap(std::declval<T>(), std::declval<U>()))>>
	    : Bool<noexcept(swap(std::declval<T>(), std::declval<U>()))> {};

	template <class T, class = void>
	struct IsStdSwappable : False {};

	template <class T>
	struct IsStdSwappable<T, Void<decltype(swap(std::declval<T&>(), std::declval<T&>()))>> : True {};

	template <class T, class = void>
	struct IsNothrowStdSwappable : False {};

	template <class T>
	struct IsNothrowStdSwappable<T, Void<decltype(swap(std::declval<T&>(), std::declval<T&>()))>>
	    : Bool<noexcept(swap(std::declval<T&>(), std::declval<T&>()))> {};

} // namespace impl::std_swap

inline namespace traits {

	template <class T, class U>
	using IsStdSwappableWith =
	    And<impl::std_swap::IsStdSwappableWith<T, U>, impl::std_swap::IsStdSwappableWith<U, T>>;

	template <class T, class U>
	inline constexpr bool isStdSwappableWith = IsStdSwappableWith<T, U>::value;

	template <class T, class U>
	using IsNothrowStdSwappableWith =
	    And<impl::std_swap::IsNothrowStdSwappableWith<T, U>, impl::std_swap::IsNothrowStdSwappableWith<U, T>>;

	template <class T, class U>
	inline constexpr bool isNothrowStdSwappableWith = IsNothrowStdSwappableWith<T, U>::value;

	template <class T>
	using IsStdSwappable = impl::std_swap::IsStdSwappable<T>;

	template <class T>
	inline constexpr bool isStdSwappable = IsStdSwappable<T>::value;

	template <class T>
	using IsNothrowStdSwappable = impl::std_swap::IsNothrowStdSwappable<T>;

	template <class T>
	inline constexpr bool isNothrowStdSwappable = IsNothrowStdSwappable<T>::value;

} // namespace traits

} // namespace rb::core
