#pragma once

#include <rb/core/traits/Bool.hpp>

namespace rb::core {

namespace impl {

	template <class T>
	struct IsFloatingPoint : False {};

	template <>
	struct IsFloatingPoint<float> : True {};

	template <>
	struct IsFloatingPoint<double> : True {};

	template <>
	struct IsFloatingPoint<long double> : True {}; // NOLINT(*-runtime-float)

} // namespace impl

inline namespace traits {

	template <class T>
	using IsFloatingPoint = impl::IsFloatingPoint<RemoveCv<T>>;

	template <class T>
	inline constexpr bool isFloatingPoint = IsFloatingPoint<T>::value;

} // namespace traits

} // namespace rb::core
