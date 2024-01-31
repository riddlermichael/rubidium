#pragma once

#include <rb/core/traits/detection.hpp>
#include <rb/core/traits/detectors.hpp>
#include <rb/core/traits/ops.hpp>
#include <rb/core/traits/TypeIdentity.hpp>

// named requirements
// https://en.cppreference.com/w/cpp/named_req

namespace rb::core {
inline namespace traits {

	template <class T>
	using IsBooleanTestable = And<
	    IsDetectedConvertible<bool, TypeIdentity, T>,
	    IsDetectedConvertible<bool, NegationDetector, T>>;

	template <class T, class U = T>
	using IsEqualityComparable = IsDetectedConvertible<bool, OpEqualDetector, T, U>;

	template <class T, class U = T>
	inline constexpr bool isEqualityComparable = IsEqualityComparable<T, U>::value;

	template <class T, class U = T>
	using IsInequalityComparable = IsDetectedConvertible<bool, OpNotEqualDetector, T, U>;

	template <class T, class U = T>
	using IsEqualityComparableStrict = And<IsEqualityComparable<T, U>, IsInequalityComparable<T, U>>;

	template <class T, class U>
	using IsLessThanComparable = IsDetectedConvertible<bool, OpLessThanDetector, T, U>;

	template <class T, class U>
	using IsWritableTo = IsDetectedExact<AddLValueRef<U>, OpLeftShiftDetector, AddLValueRef<U>, T>;

	template <class T, class U>
	inline constexpr bool isWritableTo = IsWritableTo<T, U>::value;

	template <class T, class U>
	using IsReadableFrom = IsDetectedExact<AddLValueRef<U>, OpRightShiftDetector, AddLValueRef<U>, AddLValueRef<T>>;

	template <class T, class U>
	inline constexpr bool isReadableFrom = IsReadableFrom<T, U>::value;

	template <class T, class F, class... Args>
	using MethodDetector = decltype((RB_DECLVAL(T).*RB_DECLVAL(F))(RB_DECLVAL(Args)...));

	template <class T, class F, class... Args>
	using HasMethod = IsDetected<MethodDetector, T, F, Args...>;

} // namespace traits
} // namespace rb::core
