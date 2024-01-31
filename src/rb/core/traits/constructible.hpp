#pragma once

#include <rb/core/traits/add.hpp>

namespace rb::core {
inline namespace traits {

	template <class T, class... Args>
	inline constexpr bool isConstructible = __is_constructible(T, Args...);

	template <class T, class... Args>
	using IsConstructible = Bool<isConstructible<T, Args...>>;

	template <class T, class... Args>
	inline constexpr bool isTriviallyConstructible = __is_trivially_constructible(T, Args...);

	template <class T, class... Args>
	using IsTriviallyConstructible = Bool<isTriviallyConstructible<T, Args...>>;

	template <class T>
	using IsDefaultConstructible = IsConstructible<T>;

	template <class T>
	inline constexpr bool isDefaultConstructible = IsDefaultConstructible<T>::value;

	template <class T, class... Args>
	using IsNothrowConstructible = Bool<__is_nothrow_constructible(T, Args...)>;

	template <class T, class... Args>
	inline constexpr bool isNothrowConstructible = IsNothrowConstructible<T, Args...>::value;

	template <class T>
	using IsNothrowDefaultConstructible = IsNothrowConstructible<T>;

	template <class T>
	inline constexpr bool isNothrowDefaultConstructible = IsNothrowDefaultConstructible<T>::value;

	template <class T>
	using IsTriviallyDefaultConstructible = IsTriviallyConstructible<T>;

	template <class T>
	inline constexpr bool isTriviallyDefaultConstructible = IsTriviallyDefaultConstructible<T>::value;

	template <class T>
	using IsCopyConstructible = IsConstructible<T, AddLValueRef<AddConst<T>>>;

	template <class T>
	inline constexpr bool isCopyConstructible = IsCopyConstructible<T>::value;

	template <class T>
	using IsTriviallyCopyConstructible = IsTriviallyConstructible<T, AddLValueRef<AddConst<T>>>;

	template <class T>
	inline constexpr bool isTriviallyCopyConstructible = IsTriviallyCopyConstructible<T>::value;

	template <class T>
	using IsNothrowCopyConstructible = IsNothrowConstructible<T, AddLValueRef<AddConst<T>>>;

	template <class T>
	inline constexpr bool isNothrowCopyConstructible = IsNothrowCopyConstructible<T>::value;

	template <class T>
	using IsMoveConstructible = IsConstructible<T, AddRValueRef<T>>;

	template <class T>
	inline constexpr bool isMoveConstructible = IsMoveConstructible<T>::value;

	template <class T>
	using IsTriviallyMoveConstructible = IsTriviallyConstructible<T, AddRValueRef<T>>;

	template <class T>
	inline constexpr bool isTriviallyMoveConstructible = IsTriviallyMoveConstructible<T>::value;

	template <class T>
	using IsNothrowMoveConstructible = IsNothrowConstructible<T, AddRValueRef<T>>;

	template <class T>
	inline constexpr bool isNothrowMoveConstructible = IsNothrowMoveConstructible<T>::value;

} // namespace traits
} // namespace rb::core
