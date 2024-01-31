#pragma once

#include <rb/core/traits/IsFloatingPoint.hpp>
#include <rb/core/traits/IsIntegral.hpp>

namespace rb::core {
inline namespace traits {

	template <class T>
	inline constexpr bool isArithmetic = isIntegral<T> || isFloatingPoint<T>;

	template <class T>
	using IsArithmetic = Bool<isArithmetic<T>>;

} // namespace traits
} // namespace rb::core
