#pragma once

#include <rb/core/traits/Bool.hpp>

namespace rb::core {
inline namespace traits {

	template <class T>
	struct IsLValueRef : False {};

	template <class T>
	struct IsLValueRef<T&> : True {};

	template <class T>
	inline constexpr bool isLValueRef = IsLValueRef<T>::value;

	template <class T>
	struct IsRValueRef : False {};

	template <class T>
	struct IsRValueRef<T&&> : True {};

	template <class T>
	inline constexpr bool isRValueRef = IsRValueRef<T>::value;

	template <class T>
	struct IsRef : False {};

	template <class T>
	struct IsRef<T&> : True {};

	template <class T>
	struct IsRef<T&&> : True {};

	template <class T>
	inline constexpr bool isRef = IsRef<T>::value;

} // namespace traits
} // namespace rb::core
