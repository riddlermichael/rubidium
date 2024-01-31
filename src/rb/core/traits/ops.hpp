#pragma once

#include <rb/core/traits/Bool.hpp>
#include <rb/core/traits/Conditional.hpp>

namespace rb::core {
inline namespace traits {

	template <class...>
	struct And : True {};

	template <class T>
	struct And<T> : T {};

	template <class T, class... Args>
	struct And<T, Args...> : Conditional<static_cast<bool>(T::value), And<Args...>, T> {};

	template <class...>
	struct Or : False {};

	template <class T>
	struct Or<T> : T {};

	template <class T, class... Args>
	struct Or<T, Args...> : Conditional<static_cast<bool>(T::value), T, Or<Args...>> {};

	template <class T>
	struct Not : Bool<!static_cast<bool>(T::value)> {};

	/// Equivalent to `!T || Args...`.
	template <class T, class... Args>
	struct If : Or<Not<T>, Args...> {};

} // namespace traits
} // namespace rb::core
