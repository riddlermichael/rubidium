#pragma once

#include <rb/core/traits/detection.hpp>
#include <rb/core/traits/ops.hpp>
#include <rb/core/traits/remove.hpp>
#include <rb/core/types.hpp>

namespace rb::core {
namespace impl {

	template <class T>
	using TupleElementDetector = std::tuple_element_t<usize{0}, T>;

	template <class T>
	using TupleSizeDetector = decltype(std::tuple_size_v<T>);

	template <class T>
	using ClassSpecificGetDetector = decltype(RB_DECLVAL(T).template get<usize{0}>());

	// ReSharper disable once CppFunctionIsNotImplemented
	template <usize>
	void get();

	template <class T>
	using GlobalGetDetector = decltype(get<usize{0}>(RB_DECLVAL(T)));

	template <class T>
	using HasGet = Or<IsDetected<ClassSpecificGetDetector, T>, IsDetected<GlobalGetDetector, T>>;

	template <class T, bool = true>
	struct IsTupleLikeImpl {
		using Type = True;
	};

	template <class T>
	struct IsTupleLikeImpl<T, false> {
		using Type = And<IsDetected<TupleElementDetector, T>, HasGet<T>>;
	};

	template <class T, class = void>
	struct IsTupleLike {
		using Type = False;
	};

	template <class T>
	struct IsTupleLike<T, Void<TupleSizeDetector<T>>> {
		using Type = typename IsTupleLikeImpl<T, std::tuple_size_v<T> == 0>::Type;
	};

} // namespace impl

inline namespace traits {

	template <class T>
	using IsTupleLike = typename impl::IsTupleLike<RemoveCvRef<T>>::Type;

	template <class T>
	inline constexpr bool isTupleLike = IsTupleLike<T>::value;

} // namespace traits
} // namespace rb::core
