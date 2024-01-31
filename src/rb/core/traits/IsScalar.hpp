#pragma once

#include <rb/core/traits/enums.hpp>
#include <rb/core/traits/IsArithmetic.hpp>
#include <rb/core/traits/IsPointer.hpp>
#include <rb/core/traits/members.hpp>

namespace rb::core {
inline namespace traits {

	template <class T>
	using IsNullPointer = IsSame<decltype(nullptr), RemoveCv<T>>;

	template <class T>
	inline constexpr bool isNullPointer = IsNullPointer<T>::value;

	template <class T>
	using IsScalar = Or<IsArithmetic<T>, IsEnum<T>, IsPointer<T>, IsMemberPointer<T>, IsNullPointer<T>>;

	template <class T>
	inline constexpr bool isScalar = IsScalar<T>::value;

} // namespace traits
} // namespace rb::core
