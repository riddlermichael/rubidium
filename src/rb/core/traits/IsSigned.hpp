#pragma once

#include <rb/core/traits/IsArithmetic.hpp>

namespace rb::core {

namespace impl {

	template <class T, bool = isArithmetic<T>>
	struct IsSigned : False {};

	template <class T>
	struct IsSigned<T, true> : Bool<(T(-1) < T(0))> {};

} // namespace impl

inline namespace traits {

	template <class T>
	using IsSigned = impl::IsSigned<T>;

	template <class T>
	inline constexpr bool isSigned = IsSigned<T>::value;

} // namespace traits
} // namespace rb::core
