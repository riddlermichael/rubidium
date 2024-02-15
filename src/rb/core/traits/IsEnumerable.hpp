#pragma once

#include <rb/core/iter/primitives.hpp>
#include <rb/core/traits/requirements.hpp>

namespace rb::core {

namespace impl {
	template <class T>
	using BeginType = DetectedType<BeginDetector, T>;

	template <class T>
	using EndType = DetectedType<EndDetector, T>;

	template <class T, class = void>
	struct IsEnumerableImpl {
		using Type = False;
	};

	template <class T>
	struct IsEnumerableImpl<T, Void<BeginType<T>, EndType<T>, LValueRefDetector<BeginType<T>>>> {
		using Begin = BeginType<T>;
		using End = EndType<T>;
		using Type = And<
		    IsInequalityComparable<Begin, End>,
		    IsDetected<IncrementDetector, AddLValueRef<RemoveCvRef<Begin>>>,
		    IsDetected<DereferenceDetector, Begin>>;
	};
} // namespace impl

inline namespace traits {

	template <class T>
	using IsEnumerable = typename impl::IsEnumerableImpl<AddLValueRef<T>>::Type;

	template <class T>
	inline constexpr bool isEnumerable = IsEnumerable<T>::value;

} // namespace traits

} // namespace rb::core
