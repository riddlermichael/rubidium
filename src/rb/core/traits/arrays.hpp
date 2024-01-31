#pragma once

#include <rb/core/traits/Bool.hpp>
#include <rb/core/types.hpp>

namespace rb::core {

namespace impl {

	template <class T>
	struct RemoveExtentImpl {
		using Type = T;
	};

	template <class T>
	struct RemoveExtentImpl<T[]> {
		using Type = T;
	};

	template <class T, usize _>
	struct RemoveExtentImpl<T[_]> {
		using Type = T;
	};

	template <class T>
	struct RemoveAllExtentsImpl {
		using Type = T;
	};

	template <class T>
	struct RemoveAllExtentsImpl<T[]> {
		using Type = typename RemoveAllExtentsImpl<T>::Type;
	};

	template <class T, usize _>
	struct RemoveAllExtentsImpl<T[_]> {
		using Type = typename RemoveAllExtentsImpl<T>::Type;
	};

	template <class T, usize n>
	struct Extent : Constant<0_UZ> {};

	template <class T>
	struct Extent<T[], 0> : Constant<0_UZ> {};

	template <class T, usize n>
	struct Extent<T[], n> : Extent<T, n - 1> {};

	template <class T, usize idx>
	struct Extent<T[idx], 0> : Constant<idx> {};

	template <class T, usize idx, usize n>
	struct Extent<T[idx], n> : Extent<T, n - 1> {};

} // namespace impl

inline namespace traits {

	template <class T>
	struct IsBoundedArray : False {};

	template <class T, usize _>
	struct IsBoundedArray<T[_]> : True {};

	template <class T>
	inline constexpr bool isBoundedArray = IsBoundedArray<T>::value;

	template <class T>
	struct IsUnboundedArray : False {};

	template <class T>
	struct IsUnboundedArray<T[]> : True {};

	template <class T>
	inline constexpr bool isUnboundedArray = IsUnboundedArray<T>::value;

	template <class T>
	inline constexpr bool isArray = isBoundedArray<T> || isUnboundedArray<T>;

	template <class T>
	using IsArray = Bool<isArray<T>>;

	template <class T>
	struct Rank : Constant<0_UZ> {};

	template <class T>
	struct Rank<T[]> : Constant<Rank<T>::value + 1> {};

	template <class T, usize _>
	struct Rank<T[_]> : Constant<Rank<T>::value + 1> {};

	template <class T>
	using RemoveExtent = typename impl::RemoveExtentImpl<T>::Type;

	template <class T>
	using RemoveAllExtents = typename impl::RemoveAllExtentsImpl<T>::Type;

	template <class T, usize n = 0>
	using Extent = impl::Extent<T, n>;

} // namespace traits
} // namespace rb::core
