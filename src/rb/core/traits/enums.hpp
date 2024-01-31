#pragma once

#include <rb/core/traits/IsConvertible.hpp>

// TODO use `decay`-ed T

namespace rb::core {

inline namespace traits {

	template <class T>
	using IsEnum = Bool<__is_enum(T)>;

	template <class T>
	inline constexpr bool isEnum = IsEnum<T>::value;

} // namespace traits

namespace impl {

	template <class T, bool = isEnum<T>>
	struct Underlying {
		using Type = __underlying_type(T);
	};

	template <class T>
	struct Underlying<T, false> {
		// SFINAE-friendly since C++20
	};

	template <class T, bool = isEnum<T>>
	struct IsScopedEnum : False {};

	template <class T>
	struct IsScopedEnum<T, true> : Bool<!isConvertible<T, typename Underlying<T>::Type>> {};

	template <class T, bool = isEnum<T>>
	struct IsUnscopedEnum : False {};

	template <class T>
	struct IsUnscopedEnum<T, true> : IsConvertible<T, typename Underlying<T>::Type> {};

} // namespace impl

inline namespace traits {

	template <class T>
	using UnderlyingType = typename impl::Underlying<T>::Type;

	template <class T>
	using IsScopedEnum = impl::IsScopedEnum<T>;

	template <class T>
	inline constexpr bool isScopedEnum = IsScopedEnum<T>::value;

	template <class T>
	using IsUnscopedEnum = impl::IsUnscopedEnum<T>;

	template <class T>
	inline constexpr bool isUnscopedEnum = IsUnscopedEnum<T>::value;

} // namespace traits

} // namespace rb::core
