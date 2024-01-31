#pragma once

#include <rb/core/traits/Decay.hpp>
#include <rb/core/traits/declval.hpp>
#include <rb/core/traits/IsSame.hpp>

namespace rb::core {

namespace impl {

	template <class T, class U>
	using ConditionalResult = decltype(false ? RB_DECLVAL(T) : RB_DECLVAL(U));

	template <class T, class U, class = void>
	struct DecayConditionalResult {
	};

	template <class T, class U>
	struct DecayConditionalResult<T, U, Void<ConditionalResult<T, U>>> {
		using Type = Decay<ConditionalResult<T, U>>;
	};

	template <class T, class U, class = void>
	struct Common2 {
		using Type = DecayConditionalResult<T const&, U const&>;
	};

	template <class T, class U>
	struct Common2<T, U, Void<ConditionalResult<T, U>>> {
		using Type = DecayConditionalResult<T, U>;
	};

} // namespace impl

inline namespace traits {

	template <class... Ts>
	struct Common {
	};

	template <class... Ts>
	using CommonType = typename Common<Ts...>::Type::Type;

	template <class T>
	struct Common<T> : Common<T, T> {};

	template <class T, class U>
	struct Common<T, U>
	    : Conditional<isSame<T, Decay<T>> && isSame<U, Decay<U>>,
	          impl::Common2<T, U>,
	          Common<Decay<T>, Decay<U>>> {
	};

} // namespace traits

namespace impl {

	template <class AlwaysVoid, class T, class U, class... Ts>
	struct Common3 {
	};

	template <class T, class U, class... Ts>
	struct Common3<Void<CommonType<T, U>>, T, U, Ts...>
	    : Common<CommonType<T, U>, Ts...> {
	};

} // namespace impl

inline namespace traits {

	template <class T, class U, class... Ts>
	struct Common<T, U, Ts...>
	    : impl::Common3<void, T, U, Ts...> {
	};

} // namespace traits

} // namespace rb::core
