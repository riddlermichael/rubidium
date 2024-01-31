#pragma once

#include <rb/core/traits/IsArithmetic.hpp>

namespace rb::core {

namespace impl {

	template <class T, bool = isArithmetic<T>>
	struct IsUnsigned : False {};

	template <class T>
	struct IsUnsigned<T, true> : Bool<(T(0) < T(-1))> {};

} // namespace impl

inline namespace traits {

	template <class T>
	using IsUnsigned = impl::IsUnsigned<T>;

	template <class T>
	inline constexpr bool isUnsigned = IsUnsigned<T>::value;

} // namespace traits
} // namespace rb::core
