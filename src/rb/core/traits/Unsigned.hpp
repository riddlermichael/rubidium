#pragma once

#include <rb/core/traits/enums.hpp>
#include <rb/core/traits/IsIntegral.hpp>
#include <rb/core/types.hpp>

namespace rb::core {

namespace impl {

	template <unsigned>
	struct UnsignedOfSize;

	template <>
	struct UnsignedOfSize<1> {
		using Type = u8;
	};

	template <>
	struct UnsignedOfSize<2> {
		using Type = u16;
	};

	template <>
	struct UnsignedOfSize<4> {
		using Type = u32;
	};

	template <>
	struct UnsignedOfSize<8> {
		using Type = u64;
	};

	template <class T, bool = isIntegral<T> || isEnum<T>>
	struct UnsignedImpl {};

	template <class T>
	struct UnsignedImpl<T, true> {
		using Type = typename UnsignedOfSize<sizeof(T)>::Type;
	};

	template <>
	struct UnsignedImpl<bool> {
	};

	template <>
	struct UnsignedImpl<bool const> {
	};

	template <>
	struct UnsignedImpl<bool volatile> {
	};

	template <>
	struct UnsignedImpl<bool const volatile> {
	};

} // namespace impl

inline namespace traits {

	template <class T>
	using Unsigned = typename impl::UnsignedImpl<T>::Type;

} // namespace traits
} // namespace rb::core
