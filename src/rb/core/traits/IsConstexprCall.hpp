#pragma once

#include <rb/core/traits/Bool.hpp>

namespace rb::core {

namespace impl {

	template <auto f, auto... args, class = decltype(f(args...))> // SFINAE-friendly
	constexpr bool constexprCaller() {
		static_cast<void>(f(args...));
		return true;
	}

	// we use good old approach with ellipsis because we work with values, not types
	template <auto f, auto... args>
	constexpr bool isConstexprCall(...) { // NOLINT(cert-dcl50-cpp, *-avoid-variadic-functions)
		return false;
	}

	template <auto f, auto... args, bool = constexprCaller<f, args...>()>
	constexpr bool isConstexprCall(int /*unused*/) {
		return true;
	}

} // namespace impl

inline namespace traits {

	/**
	 * Check that `f(args...)` is `constexpr` expression.
	 *
	 * Let we have code:
	 * @code
	 * void foo() {}
	 * void bar() {}
	 * constexpr void bar(int) {}
	 * constexpr void baz() {}
	 * @endcode
	 * Then
	 * @code
	 * static_assert(!isConstexprCall&lt;foo&gt;);
	 * static_assert(isConstexprCall&lt;baz&gt;);
	 * @endcode
	 * For using with overloaded functions, introduce `constexpr` variable with desired type
	 * for select right overload or use `static_cast`:
	 * @code
	 * constexpr void (*f1)() = bar;
	 * static_assert(!isConstexprCall&lt;f1&gt;);
	 * static_assert(!isConstexprCall&lt;static_cast&lt;void (*)()&gt;(bar)&gt;);
	 *
	 * constexpr void (*f2)(int) = bar;
	 * static_assert(isConstexprCall&lt;f2, 0&gt;);
	 * static_assert(isConstexprCall&lt;static_cast&lt;void (*)(int)&gt;(bar), 0&gt;);
	 * @endcode
	 * This trait is SFINAE-friendly, i.e.
	 * @code
	 * static_assert(!isConstexprCall&lt;baz, 42&gt;);
	 * @endcode
	 */
	template <auto f, auto... args>
	using IsConstexprCall = Bool<impl::isConstexprCall<f, args...>(0)>;

	template <auto f, auto... args>
	inline constexpr bool isConstexprCall = IsConstexprCall<f, args...>::value;

} // namespace traits
} // namespace rb::core
