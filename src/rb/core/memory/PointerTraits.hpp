#pragma once

#include <rb/core/memory/addressOf.hpp>
#include <rb/core/meta/TypeSeq.hpp>

namespace rb::core {

namespace impl {

	template <class T>
	using FirstArgOf = typename TypeArgsOf<T>::template Front<>;

	template <class T>
	using DifferenceDetector = typename T::Difference;

	template <class Template, class To>
	struct ReplaceFirst;

	template <template <class, class...> class Template, class From, class... Args, class To>
	struct ReplaceFirst<Template<From, Args...>, To> {
		using Type = Template<To, Args...>;
	};

	template <class Template, class To>
	using ReplaceFirstType = typename ReplaceFirst<Template, To>::Type;

	template <class P, class U, class = void, class = void>
	struct RebindImpl {};

	template <class P, class U>
	struct RebindImpl<P, U, Void<typename P::template Rebind<U>>, void> {
		using Type = typename P::template Rebind<U>;
	};

	template <class P, class U, class _>
	struct RebindImpl<P, U, _, Void<ReplaceFirstType<P, U>>> {
		using Type = ReplaceFirstType<P, U>;
	};

	template <class P, class E>
	struct PointerTraitsBase {
		using Pointer = P;
		using Element = E;
		using Difference = DetectedOrType<isize, DifferenceDetector, P>;

		template <class U>
		using Rebind = typename RebindImpl<P, U>::Type;

		[[nodiscard]] static P pointerTo(E& ref) RB_NOEXCEPT_LIKE(P::pointerTo(ref)) {
			return P::pointerTo(ref);
		}
	};

	template <class, class = void, class = void>
	struct PointerTraitsImpl {};

	template <class P>
	struct PointerTraitsImpl<P, Void<typename P::Element>, void>
	    : PointerTraitsBase<P, typename P::Element> {};

	template <class P, class _>
	struct PointerTraitsImpl<P, _, Void<FirstArgOf<P>>>
	    : PointerTraitsBase<P, FirstArgOf<P>> {};

} // namespace impl

inline namespace memory {

	template <class P>
	struct PointerTraits : impl::PointerTraitsImpl<P> {};

	template <class T>
	struct PointerTraits<T*> {
		using Pointer = T*;
		using Element = T;
		using Difference = isize;

		template <class U>
		using Rebind = U*;

		[[nodiscard]] static constexpr T* pointerTo(T& ref) noexcept {
			return addressOf(ref);
		}
	};

} // namespace memory

} // namespace rb::core
