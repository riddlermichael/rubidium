#pragma once

#include <rb/core/traits/IsSame.hpp>

namespace rb::core {
inline namespace traits {

	template <class T>
	struct IsVoid : IsSame<void, RemoveCv<T>> {};

	template <class T>
	inline constexpr bool isVoid = IsVoid<T>::value;

} // namespace traits
} // namespace rb::core
