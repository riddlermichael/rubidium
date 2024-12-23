#pragma once

#include <rb/core/traits/alwaysFalse.hpp>

namespace rb::core {

namespace impl {

	template <class T, class U = T&&>
	U declvalImpl(int);

	template <class T>
	T declvalImpl(...);

} // namespace impl

inline namespace traits {

	template <class T>
	decltype(impl::declvalImpl<T>(0)) declval() noexcept { // `decltype(auto)` couldn't be used here
		static_assert(alwaysFalse<T>, "usage of rb::core::declval is not allowed in an evaluated context");
		return impl::declvalImpl<T>(0);
	}

} // namespace traits
} // namespace rb::core

#define RB_DECLVAL(...) rb::core::declval<__VA_ARGS__>()
