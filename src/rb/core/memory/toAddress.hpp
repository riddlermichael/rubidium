#pragma once

#include <rb/core/memory/PointerTraits.hpp>
#include <rb/core/traits/IsFunction.hpp>

namespace rb::core {

namespace impl {
	template <class T>
	using ToAddressDetector = decltype(PointerTraits<T>::toAddress(RB_DECLVAL(T const&)));

	template <class T>
	using HasToAddress = IsDetected<ToAddressDetector, T>;
} // namespace impl

inline namespace memory {

	/// @return @p ptr unmodified if `T` is not a function type
	template <class T>
	constexpr T* toAddress(T* ptr) noexcept {
		static_assert(!isFunction<T>);
		return ptr;
	}

	/// If the expression <tt>core::PointerTraits<T>::toAddress(ptr)</tt> is well-formed,
	/// returns the result of that expression.
	/// Otherwise, returns <tt>toAddress(ptr.operator->())</tt>.
	template <class P>
	constexpr auto toAddress(P const& ptr) noexcept {
		if constexpr (impl::HasToAddress<P>::value) {
			return PointerTraits<P>::toAddress(ptr);
		} else {
			return toAddress(ptr.operator->());
		}
	}

} // namespace memory

} // namespace rb::core
