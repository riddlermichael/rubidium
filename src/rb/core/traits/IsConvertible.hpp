#pragma once

#include <rb/core/traits/declval.hpp>
#include <rb/core/traits/IsVoid.hpp>
#include <rb/core/traits/ops.hpp>

namespace rb::core {

namespace impl {
	template <class T>
	auto testReturnable(int) -> decltype(void(static_cast<T (*)()>(nullptr)), True{});

	template <class>
	[[maybe_unused]] auto testReturnable(...) -> False;

	template <class From, class To>
	auto testImplicitlyConvertible(int)
	    -> decltype(void(RB_DECLVAL(void (&)(To))(RB_DECLVAL(From))), True{});

	template <class, class>
	[[maybe_unused]] auto testImplicitlyConvertible(...) -> False;

	template <class From, class To>
	auto testImplicitlyConvertibleNoexcept(int)
	    -> Bool<noexcept(RB_DECLVAL(void (&)(To) noexcept)(RB_DECLVAL(From)))>;

	template <class, class>
	[[maybe_unused]] auto testImplicitlyConvertibleNoexcept(...) -> False;
} // namespace impl

inline namespace traits {

	template <class From, class To>
	struct IsConvertible
	    : Or<
	          And<decltype(impl::testReturnable<To>(0)), decltype(impl::testImplicitlyConvertible<From, To>(0))>,
	          And<IsVoid<From>, IsVoid<To>>> {};

	template <class From, class To>
	inline constexpr bool isConvertible = IsConvertible<From, To>::value;

	template <class From, class To>
	struct IsNothrowConvertible
	    : Or<
	          And<decltype(impl::testReturnable<To>(0)), decltype(impl::testImplicitlyConvertibleNoexcept<From, To>(0))>,
	          And<IsVoid<From>, IsVoid<To>>> {};

	template <class From, class To>
	inline constexpr bool isNothrowConvertible = IsNothrowConvertible<From, To>::value;

} // namespace traits
} // namespace rb::core
