#pragma once

#include <rb/core/traits/add.hpp>

namespace rb::core {
inline namespace traits {

	template <class T, class U>
	using IsAssignable = Bool<__is_assignable(T, U)>;

	template <class T, class U>
	inline constexpr bool isAssignable = IsAssignable<T, U>::value;

	template <class T, class U>
	using IsTriviallyAssignable = Bool<__is_trivially_assignable(T, U)>;

	template <class T, class U>
	inline constexpr bool isTriviallyAssignable = IsTriviallyAssignable<T, U>::value;

	template <class T, class U>
	using IsNothrowAssignable = Bool<__is_nothrow_assignable(T, U)>;

	template <class T, class U>
	inline constexpr bool isNothrowAssignable = IsNothrowAssignable<T, U>::value;

	template <class T>
	using IsCopyAssignable = IsAssignable<AddLValueRef<T>, AddLValueRef<T const>>;

	template <class T>
	inline constexpr bool isCopyAssignable = IsCopyAssignable<T>::value;

	template <class T>
	using IsTriviallyCopyAssignable = IsTriviallyAssignable<AddLValueRef<T>, AddLValueRef<T const>>;

	template <class T>
	inline constexpr bool isTriviallyCopyAssignable = IsTriviallyCopyAssignable<T>::value;

	template <class T>
	using IsNothrowCopyAssignable = IsNothrowAssignable<AddLValueRef<T>, AddLValueRef<T const>>;

	template <class T>
	inline constexpr bool isNothrowCopyAssignable = IsNothrowCopyAssignable<T>::value;

	template <class T>
	using IsMoveAssignable = IsAssignable<AddLValueRef<T>, T>;

	template <class T>
	inline constexpr bool isMoveAssignable = IsMoveAssignable<T>::value;

	template <class T>
	using IsTriviallyMoveAssignable = IsTriviallyAssignable<AddLValueRef<T>, T>;

	template <class T>
	inline constexpr bool isTriviallyMoveAssignable = IsTriviallyMoveAssignable<T>::value;

	template <class T>
	using IsNothrowMoveAssignable = IsNothrowAssignable<AddLValueRef<T>, T>;

	template <class T>
	inline constexpr bool isNothrowMoveAssignable = IsNothrowMoveAssignable<T>::value;

} // namespace traits
} // namespace rb::core
