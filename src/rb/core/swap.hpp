#pragma once

#include <rb/core/requires.hpp>
#include <rb/core/traits/assignable.hpp>
#include <rb/core/traits/constructible.hpp>
#include <rb/core/traits/declval.hpp>
#include <rb/core/traits/ops.hpp>
#include <rb/core/types.hpp>

namespace rb::core {

template <class T>
constexpr auto swap(T& lhs, T& rhs) noexcept(isNothrowMoveConstructible<T> && isNothrowMoveAssignable<T>)
    -> EnableIf<isMoveConstructible<T> && isMoveAssignable<T>> {
	T t = lhs;
	lhs = rhs;
	rhs = t;
}

namespace impl::swap {

	using core::swap;

	template <class T, class U, class = void>
	struct IsSwappableWith : False {};

	template <class T, class U>
	struct IsSwappableWith<T, U, Void<decltype(swap(declval<T>(), declval<U>()))>> : True {};

	template <class T, class U, class = void>
	struct IsNothrowSwappableWith : False {};

	template <class T, class U>
	struct IsNothrowSwappableWith<T, U, Void<decltype(swap(declval<T>(), declval<U>()))>>
	    : Bool<noexcept(swap(declval<T>(), declval<U>()))> {};

	template <class T, class = void>
	struct IsSwappable : False {};

	template <class T>
	struct IsSwappable<T, Void<decltype(swap(declval<T&>(), declval<T&>()))>> : True {};

	template <class T, class = void>
	struct IsNothrowSwappable : False {};

	template <class T>
	struct IsNothrowSwappable<T, Void<decltype(swap(declval<T&>(), declval<T&>()))>>
	    : Bool<noexcept(swap(declval<T&>(), declval<T&>()))> {};

} // namespace impl::swap

template <class T, class U>
using IsSwappableWith = And<impl::swap::IsSwappableWith<T, U>, impl::swap::IsSwappableWith<U, T>>;

template <class T, class U>
inline constexpr bool isSwappableWith = IsSwappableWith<T, U>::value;

template <class T, class U>
using IsNothrowSwappableWith = And<impl::swap::IsNothrowSwappableWith<T, U>, impl::swap::IsNothrowSwappableWith<U, T>>;

template <class T, class U>
inline constexpr bool isNothrowSwappableWith = IsNothrowSwappableWith<T, U>::value;

template <class T>
using IsSwappable = impl::swap::IsSwappable<T>;

template <class T>
inline constexpr bool isSwappable = IsSwappable<T>::value;

template <class T>
using IsNothrowSwappable = impl::swap::IsNothrowSwappable<T>;

template <class T>
inline constexpr bool isNothrowSwappable = IsNothrowSwappable<T>::value;

template <class T, usize n>
constexpr EnableIf<isSwappable<T>> swap(T (&lhs)[n], T (&rhs)[n]) noexcept(isNothrowSwappable<T>) {
	for (usize pos = 0; pos < n; ++pos) {
		swap(lhs[pos], rhs[pos]);
	}
}

} // namespace rb::core
