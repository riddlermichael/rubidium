#pragma once

#include <rb/core/traits/Bool.hpp>

namespace rb::core {
inline namespace traits {

	template <class T>
	inline constexpr bool isIntegral = impl::isIntegralImpl<T>;

	template <class T>
	using IsIntegral = Bool<isIntegral<T>>;

} // namespace traits
} // namespace rb::core
