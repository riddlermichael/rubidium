#pragma once

#include <rb/core/traits/Bool.hpp>

namespace rb::core {

namespace impl {

	template <class T>
	struct IsPointerImpl : False {};

	template <class T>
	struct IsPointerImpl<T*> : True {};

} // namespace impl

inline namespace traits {

	template <class T>
	using IsPointer = impl::IsPointerImpl<RemoveCv<T>>;

	template <class T>
	inline constexpr bool isPointer = IsPointer<T>::value;

} // namespace traits

} // namespace rb::core
