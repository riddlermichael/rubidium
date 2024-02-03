#pragma once

#include <rb/core/helpers.hpp>
#include <rb/core/move.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/Decay.hpp>
#include <rb/core/traits/detection.hpp>
#include <rb/core/traits/IsBaseOf.hpp>
#include <rb/core/traits/IsConvertible.hpp>
#include <rb/core/traits/IsObject.hpp>
#include <rb/core/traits/members.hpp>

namespace rb::core {

namespace impl {

	// ReSharper disable CppFunctionIsNotImplemented

	template <class T>
	struct InvokeSelector {
		template <class F, class... Args>
		static auto call(F&& f, Args&&... args) RB_NOEXCEPT_LIKE(RB_FWD(f)(RB_FWD(args)...))
		    -> decltype(RB_FWD(f)(RB_FWD(args)...));
	};

	template <class C, class P>
	struct InvokeSelector<P C::*> {
		template <class Arg,
		    RB_REQUIRES(isBaseOf<C, Decay<Arg>>)>
		static auto get(Arg&& arg) noexcept
		    -> Arg&&;

		template <class Arg,
		    RB_REQUIRES(!isBaseOf<C, Decay<Arg>>)>
		static auto get(Arg&& arg) RB_NOEXCEPT_LIKE(*RB_FWD(arg))
		    -> decltype(*RB_FWD(arg));

		template <class Arg, class... Args, class F,
		    RB_REQUIRES(isFunction<F>)>
		static auto call(F C::*f, Arg&& arg, Args&&... args) RB_NOEXCEPT_LIKE((get(RB_FWD(arg)).*f)(RB_FWD(args)...))
		    -> decltype((get(RB_FWD(arg)).*f)(RB_FWD(args)...));

		template <class Arg>
		static auto call(P C::*f, Arg&& arg) RB_NOEXCEPT_LIKE(get(RB_FWD(arg)).*f)
		    -> decltype(get(RB_FWD(arg)).*f);
	};

	template <class C, class P, class Arg, class... Args>
	constexpr decltype(auto) invokeMemberPointer(P C::*f, Arg&& arg, Args&&... args) {
		if constexpr (isFunction<P>) {
			if constexpr (isBaseOf<C, Decay<Arg>>) {
				return (RB_FWD(arg).*f)(RB_FWD(args)...);
			} else {
				return ((*RB_FWD(arg)).*f)(RB_FWD(args)...);
			}
		} else {
			static_assert(isObject<P> && !sizeof...(Args));
			if constexpr (isBaseOf<C, Decay<Arg>>) {
				return RB_FWD(arg).*f;
			} else {
				return (*RB_FWD(arg)).*f;
			}
		}
	}

	template <bool isInvocable, class F, class... Args>
	struct IsNothrowInvocableImpl : False {
	};

	template <class F, class... Args>
	struct IsNothrowInvocableImpl<true, F, Args...>
	    : Bool<noexcept(InvokeSelector<Decay<F>>::call(RB_DECLVAL(F), RB_DECLVAL(Args)...))> {
	};

} // namespace impl

template <class F, class... Args>
using InvokeResult = decltype(impl::InvokeSelector<Decay<F>>::call(RB_DECLVAL(F), RB_DECLVAL(Args)...));

template <class F, class... Args>
using IsInvocable = IsDetected<InvokeResult, F, Args...>;

template <class F, class... Args>
inline constexpr bool isInvocable = IsInvocable<F, Args...>::value;

template <class F, class... Args>
using IsNothrowInvocable = impl::IsNothrowInvocableImpl<isInvocable<F, Args...>, F, Args...>;

template <class F, class... Args>
inline constexpr bool isNothrowInvocable = IsNothrowInvocable<F, Args...>::value;

template <class F, class... Args,
    RB_REQUIRES(isInvocable<F, Args...>)>
constexpr decltype(auto) invoke(F&& f, Args&&... args) noexcept(isNothrowInvocable<F, Args...>) {
	if constexpr (isMemberPointer<Decay<F>>) {
		return impl::invokeMemberPointer(RB_FWD(f), RB_FWD(args)...);
	} else {
		return RB_FWD(f)(RB_FWD(args)...);
	}
}

namespace impl {

	template <class R, bool isVoid, class F, class... Args>
	struct IsInvocableRImpl {
		using Type = IsDetectedConvertible<R, InvokeResult, F, Args...>;
	};

	template <class R, class F, class... Args>
	struct IsInvocableRImpl<R, true, F, Args...> {
		using Type = IsDetected<InvokeResult, F, Args...>;
	};

	template <bool isInvocableR, class R, class F, class... Args>
	struct IsNothrowInvocableRImpl : False {
	};

	template <class R, class F, class... Args>
	struct IsNothrowInvocableRImpl<true, R, F, Args...>
	    : Bool<isNothrowInvocable<F, Args...> && (isVoid<R> || isNothrowConvertible<InvokeResult<F, Args...>, R>)> {
	};

} // namespace impl

template <class R, class F, class... Args>
using IsInvocableR = typename impl::IsInvocableRImpl<R, isVoid<R>, F, Args...>::Type;

template <class R, class F, class... Args>
inline constexpr bool isInvocableR = IsInvocableR<R, F, Args...>::value;

template <class R, class F, class... Args>
using IsNothrowInvocableR = impl::IsNothrowInvocableRImpl<isInvocableR<R, F, Args...>, R, F, Args...>;

template <class R, class F, class... Args>
inline constexpr bool isNothrowInvocableR = IsNothrowInvocableR<R, F, Args...>::value;

template <class R, class F, class... Args,
    RB_REQUIRES(isInvocableR<R, F, Args...>)>
constexpr R invokeR(F&& f, Args&&... args) noexcept(isNothrowInvocableR<R, F, Args...>) {
	if constexpr (isVoid<R>) {
		RB_UNUSED(invoke(RB_FWD(f), RB_FWD(args)...));
		return;
	} else {
		return invoke(RB_FWD(f), RB_FWD(args)...);
	}
}

} // namespace rb::core
