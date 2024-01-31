#pragma once

#include <rb/core/traits/enums.hpp>
#include <rb/core/traits/IsIntegral.hpp>
#include <rb/core/types.hpp>

namespace rb::core {

namespace impl {

	template <unsigned>
	struct SignedOfSize;

	template <>
	struct SignedOfSize<1> {
		using Type = i8;
	};

	template <>
	struct SignedOfSize<2> {
		using Type = i16;
	};

	template <>
	struct SignedOfSize<4> {
		using Type = i32;
	};

	template <>
	struct SignedOfSize<8> {
		using Type = i64;
	};

	template <class T, bool = isIntegral<T> || isEnum<T>>
	struct Signed {};

	template <class T>
	struct Signed<T, true> {
		using Type = typename SignedOfSize<sizeof(T)>::Type;
	};

	template <>
	struct Signed<bool> {
	};

	template <>
	struct Signed<bool const> {
	};

	template <>
	struct Signed<bool volatile> {
	};

	template <>
	struct Signed<bool const volatile> {
	};

} // namespace impl

inline namespace traits {

	template <class T>
	using Signed = impl::Signed<T>;

	template <class T>
	using SignedType = typename Signed<T>::Type;

} // namespace traits
} // namespace rb::core
