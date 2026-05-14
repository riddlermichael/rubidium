#pragma once

#include <rb/core/traits/Bool.hpp>

namespace rb::core {
inline namespace traits {

	template <class T, class... Ts>
	inline constexpr bool isSame = (... && impl::IsSameImpl<T, Ts>::value);

	template <class T, class... Ts>
	using IsSame = Bool<isSame<T, Ts...>>;

} // namespace traits
} // namespace rb::core
