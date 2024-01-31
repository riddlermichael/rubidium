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
	struct Unsigned {};

	template <class T>
	struct Unsigned<T, true> {
		using Type = typename UnsignedOfSize<sizeof(T)>::Type;
	};

	template <>
	struct Unsigned<bool> {
	};

	template <>
	struct Unsigned<bool const> {
	};

	template <>
	struct Unsigned<bool volatile> {
	};

	template <>
	struct Unsigned<bool const volatile> {
	};

} // namespace impl

inline namespace traits {

	template <class T>
	using Unsigned = impl::Unsigned<T>;

	template <class T>
	using UnsignedType = typename Unsigned<T>::Type;

} // namespace traits
} // namespace rb::core
