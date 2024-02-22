#pragma once

#include <rb/core/iter/primitives.hpp>
#include <rb/core/traits/requirements.hpp>

namespace rb::core {

namespace impl::enumerable {

	using core::begin;

	template <class T>
	using BeginType = decltype(core::begin(RB_DECLVAL(T)));

	using core::end;

	template <class T>
	using EndType = decltype(core::end(RB_DECLVAL(T)));

} // namespace impl::enumerable

namespace impl {

	template <class T, class = void>
	struct IsEnumerableImpl {
		using Type = False;
	};

	template <class T>
	struct IsEnumerableImpl<T,
	    Void<
	        enumerable::BeginType<T>,
	        enumerable::EndType<T>>> {
		using Begin = RemoveRef<enumerable::BeginType<T>>; // auto __begin = begin-expr;
		using End = RemoveRef<enumerable::EndType<T>>; // auto __end = end-expr;
		using Type = And<
		    IsInequalityComparable<Begin, End>, // __begin != __end
		    IsDetected<IncrementDetector, AddLValueRef<Begin>>, // ++__begin
		    IsDetected<DereferenceDetector, Begin>>; // *__begin
	};

	template <class T, bool = false>
	struct EnumerableTraitsImpl {
	};

	template <class T>
	struct EnumerableTraitsImpl<T, true> {
		using Begin = typename IsEnumerableImpl<T>::Begin;
		using End = typename IsEnumerableImpl<T>::End;
		using Value = RemoveRef<DetectedType<DereferenceDetector, Begin>>;
	};

} // namespace impl

inline namespace traits {

	template <class T>
	using IsEnumerable = typename impl::IsEnumerableImpl<AddLValueRef<T>>::Type;

	template <class T>
	inline constexpr bool isEnumerable = IsEnumerable<T>::value;

	template <class T>
	using EnumerableTraits = impl::EnumerableTraitsImpl<AddLValueRef<T>, isEnumerable<T>>;

} // namespace traits

} // namespace rb::core
