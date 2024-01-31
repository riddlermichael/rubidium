#pragma once

#include <rb/core/traits/Bool.hpp>

namespace rb::core {
inline namespace traits {

	template <class T, class U>
	inline constexpr bool isSame = impl::IsSameImpl<T, U>::kValue;

	template <class T, class U>
	using IsSame = Bool<isSame<T, U>>;

} // namespace traits
} // namespace rb::core
