#pragma once

#include <memory>

#include <rb/core/traits/detection.hpp>
#include <rb/core/traits/IsFunction.hpp>

namespace rb::core {

namespace impl {
	template <class T>
	using ToAddressDetector = decltype(std::pointer_traits<T>::to_address(RB_DECLVAL(T const&)));

	template <class T>
	using HasToAddress = IsDetected<ToAddressDetector, T>;

	template <class T>
	inline constexpr bool hasToAddress = HasToAddress<T>::value;
} // namespace impl

inline namespace memory {

	/// @return @p ptr unmodified if `T` is not a function type
	template <class T>
	constexpr T* toAddress(T* ptr) noexcept {
		static_assert(!isFunction<T>);
		return ptr;
	}

	/// If the expression @c std::pointer_traits<T>::to_address(ptr) is well-formed,
	/// returns the result of that expression.
	/// Otherwise, returns `std::to_address(ptr.operator->())`.
	template <class T>
	constexpr auto toAddress(T const& ptr) noexcept {
		if constexpr (impl::hasToAddress<T>) {
			return std::pointer_traits<T>::to_address(ptr);
		} else {
			return toAddress(ptr.operator->());
		}
	}

} // namespace memory

} // namespace rb::core
