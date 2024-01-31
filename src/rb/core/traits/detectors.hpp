#pragma once

#include <rb/core/traits/declval.hpp>

namespace rb::core {
inline namespace traits {

	template <class T>
	using DereferenceDetector = decltype(*RB_DECLVAL(T));

	template <class T>
	using IncrementDetector = decltype(++RB_DECLVAL(T));

	template <class T>
	using NegationDetector = decltype(!RB_DECLVAL(T));

	template <class T>
	using LValueRefDetector = T&;

	template <class T>
	using RValueRefDetector = T&&;

	template <class T, class U = T>
	using OpEqualDetector = decltype(RB_DECLVAL(T) == RB_DECLVAL(U));

	template <class T, class U = T>
	using OpNotEqualDetector = decltype(RB_DECLVAL(T) != RB_DECLVAL(U));

	template <class T, class U>
	using OpLessThanDetector = decltype(RB_DECLVAL(T) < RB_DECLVAL(U));

	template <class T, class U>
	using OpLeftShiftDetector = decltype(RB_DECLVAL(T) << RB_DECLVAL(U));

	template <class T, class U>
	using OpRightShiftDetector = decltype(RB_DECLVAL(T) >> RB_DECLVAL(U));

	template <class T>
	using OpBitwiseNotDetector = decltype(~RB_DECLVAL(T));

	template <class T, class U = T>
	using OpBitwiseAndDetector = decltype(RB_DECLVAL(T) & RB_DECLVAL(U));

	template <class T, class U = T>
	using OpBitwiseOrDetector = decltype(RB_DECLVAL(T) | RB_DECLVAL(U));

	template <class T, class U = T>
	using OpBitwiseXorDetector = decltype(RB_DECLVAL(T) ^ RB_DECLVAL(U));

	template <class T, class U = T>
	using OpAssignBitwiseAndDetector = decltype(RB_DECLVAL(T) &= RB_DECLVAL(U));

	template <class T, class U = T>
	using OpAssignBitwiseOrDetector = decltype(RB_DECLVAL(T) |= RB_DECLVAL(U));

	template <class T, class U = T>
	using OpAssignBitwiseXorDetector = decltype(RB_DECLVAL(T) ^= RB_DECLVAL(U));

	template <class T, class Index = decltype(sizeof(0))>
	using OpIndexDetector = decltype(RB_DECLVAL(T)[RB_DECLVAL(Index)]);

} // namespace traits
} // namespace rb::core
