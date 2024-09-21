#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <rb/core/compiler.hpp>
#include <rb/core/traits/arrays.hpp>
#include <rb/core/traits/IsComplete.hpp>
#include <rb/core/traits/Void.hpp>

namespace rb::core {

namespace impl {

	template <class T, class = void>
	struct IsDestructibleImpl : False {};

	template <class T>
	struct IsDestructibleImpl<T&> : True {};

	template <class T>
	struct IsDestructibleImpl<T&&> : True {};

	template <class T>
	struct IsDestructibleImpl<T, Void<decltype(declval<T&>().~T())>> : True {};

	template <class T, class = void>
	struct IsNothrowDestructibleImpl : False {};

	template <class T>
	struct IsNothrowDestructibleImpl<T&> : True {};

	template <class T>
	struct IsNothrowDestructibleImpl<T&&> : True {};

	template <class T>
	struct IsNothrowDestructibleImpl<T, Void<decltype(declval<T>().~T())>>
	    : Bool<noexcept(declval<T>().~T())> {};

	template <class T>
	constexpr bool hasVirtualDestructorImpl() noexcept {
		if constexpr (IsComplete<T>::value) {
			return __has_virtual_destructor(T);
		} else {
			return false;
		}
	}

} // namespace impl

inline namespace traits {

	template <class T>
	using IsDestructible = impl::IsDestructibleImpl<RemoveAllExtents<T>>;

	template <class T>
	inline constexpr bool isDestructible = IsDestructible<T>::value;

	template <class T>
	using IsNothrowDestructible = impl::IsNothrowDestructibleImpl<RemoveAllExtents<T>>;

	template <class T>
	inline constexpr bool isNothrowDestructible = IsNothrowDestructible<T>::value;

	template <class T>
	inline constexpr bool isTriviallyDestructible = //
	    isDestructible<T> &&
#if defined(RB_COMPILER_CLANG)
	    __is_trivially_destructible(T)
#else
	    __has_trivial_destructor(T)
#endif
	    ;

	template <class T>
	struct IsTriviallyDestructible : Bool<isTriviallyDestructible<T>> {};

	template <class T>
	using HasVirtualDestructor = Bool<impl::hasVirtualDestructorImpl<T>()>;

	template <class T>
	inline constexpr bool hasVirtualDestructor = HasVirtualDestructor<T>::value;

} // namespace traits

} // namespace rb::core
