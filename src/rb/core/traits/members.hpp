#pragma once

#include <rb/core/traits/IsFunction.hpp>

namespace rb::core {

namespace impl {

	template <class T>
	struct IsMemberPointer : False {};

	template <class T, class U>
	struct IsMemberPointer<T U::*> : True {};

	template <class T>
	struct IsMemberFunctionPointer : False {};

	template <class T, class U>
	struct IsMemberFunctionPointer<T U::*> : IsFunction<T> {};

} // namespace impl

inline namespace traits {

	template <class T>
	using IsMemberPointer = impl::IsMemberPointer<RemoveCv<T>>;

	template <class T>
	inline constexpr bool isMemberPointer = IsMemberPointer<T>::value;

	template <class T>
	using IsMemberFunctionPointer = impl::IsMemberFunctionPointer<RemoveCv<T>>;

	template <class T>
	inline constexpr bool isMemberFunctionPointer = IsMemberFunctionPointer<T>::value;

	template <class T>
	inline constexpr bool isMemberObjectPointer = isMemberPointer<T> && !isMemberFunctionPointer<T>;

	template <class T>
	using IsMemberObjectPointer = Bool<isMemberObjectPointer<T>>;

} // namespace traits
} // namespace rb::core
