#pragma once

#include <rb/core/traits/Bool.hpp>
#include <rb/core/types.hpp>

namespace rb::core {

/**
 * The type InPlace can be used in the constructor's parameter list to match the intended #kInPlace tag.
 */
struct InPlace {
	explicit InPlace() = default;
};

/**
 * #kInPlace is disambiguation tag that can be passed to the constructors of Option
 * to indicate that the contained object should be constructed in-place.
 */
inline constexpr InPlace kInPlace;

/**
 * The type InPlaceType can be used in the constructor's parameter list to match the intended #inPlaceType tag.
 */
template <class T>
struct InPlaceType {
	explicit InPlaceType() = default;
};

/**
 * #InPlaceType is disambiguation tag that can be passed to the constructors of Any
 * to indicate that the contained object should be constructed in-place,
 * and the type of the object to be constructed.
 */
template <class T>
inline constexpr InPlaceType<T> kInPlaceType;

template <usize idx>
struct InPlaceIndex {
	explicit InPlaceIndex() = default;
};

template <usize idx>
inline constexpr InPlaceIndex<idx> kInPlaceIndex;

namespace impl {

	template <class>
	struct IsInPlaceType : False {
	};

	template <class T>
	struct IsInPlaceType<InPlaceType<T>> : True {
		using Type = T;
	};

	template <class T>
	inline constexpr bool isInPlaceType = IsInPlaceType<T>::value;

} // namespace impl

} // namespace rb::core
