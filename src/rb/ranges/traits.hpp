#pragma once

#include <ostream>

#include <rb/core/invoke.hpp>
#include <rb/core/traits/assignable.hpp>
#include <rb/core/traits/requirements.hpp>
#include <rb/ranges/primitives.hpp>

namespace rb::ranges {

namespace impl {

	template <class R>
	using EmptyDetector = decltype(empty(RB_DECLVAL(R)));

	template <class R>
	using FrontDetector = decltype(front(RB_DECLVAL(R)));

	template <class R>
	using PopFrontDetector = decltype(popFront(RB_DECLVAL(R)));

	template <class R>
	using SaveDetector = decltype(save(RB_DECLVAL(R)));

	template <class R>
	using BackDetector = decltype(back(RB_DECLVAL(R)));

	template <class R>
	using PopBackDetector = decltype(popBack(RB_DECLVAL(R)));

	template <class R, class = void>
	struct IsInfiniteByStaticConstEmpty
	    : core::False {
	};

	template <class R>
	struct IsInfiniteByStaticConstEmpty<R, core::Void<core::Bool<R::empty>>>
	    : core::Bool<!R::empty> {
	};

	template <class R, class = void>
	struct IsInfiniteByStaticConstKEmpty
	    : core::False {
	};

	template <class R>
	struct IsInfiniteByStaticConstKEmpty<R, core::Void<core::Bool<R::kEmpty>>>
	    : core::Bool<!R::kEmpty> {
	};

	template <class R>
	using IsInfinite = core::Or<IsInfiniteByStaticConstEmpty<R>, IsInfiniteByStaticConstKEmpty<R>>;

} // namespace impl

template <class R>
using ValueType = core::DetectedOrType<void, impl::FrontDetector, R>;

template <class R>
using HasEmpty = core::IsDetected<impl::EmptyDetector, R>;

template <class R>
using IsInputRangeNonStrict = core::And<
    core::IsBooleanTestable<core::DetectedType<impl::EmptyDetector, core::AddLValueRef<R>>>, // if (!r.empty) {}
    core::Not<core::IsVoid<ValueType<R>>>, // auto x = r.front;
    core::IsDetected<impl::PopFrontDetector, core::AddLValueRef<R>> // r.popFront;
    >;

template <class R>
inline constexpr bool isInputRangeNonStrict = IsInputRangeNonStrict<R>::value;

template <class R>
using IsInputRange = core::And<
    core::IsDefaultConstructible<R>, // R r;
    IsInputRangeNonStrict<R> // if (!r.empty) {}; auto x = r.front; r.popFront;
    >;

template <class R>
inline constexpr bool isInputRange = IsInputRange<R>::value;

template <class R>
using HasSave = core::IsDetected<impl::SaveDetector, R>;

template <class R>
using IsForwardRange = core::And<
    IsInputRange<R>,
    core::IsDetectedExact<core::RemoveCvRef<R>, impl::SaveDetector, R> // R s = r.save;
    >;

template <class R>
inline constexpr bool isForwardRange = IsForwardRange<R>::value;

template <class R>
using HasPopBack = core::IsDetected<impl::PopBackDetector, R>;

template <class R>
using IsBidirectionalRange = core::And<
    IsForwardRange<R>,
    core::IsDetectedExact<ValueType<R>, impl::BackDetector, R>, // typeof(r.back) == typeof(r.front);
    HasPopBack<core::AddLValueRef<R>> // r.popBack();
    >;

template <class R>
inline constexpr bool isBidirectionalRange = IsBidirectionalRange<R>::value;

template <class R>
using IsInfinite = core::And<IsInputRange<R>, impl::IsInfinite<R>>;

template <class R>
inline constexpr bool isInfinite = IsInfinite<R>::value;

template <class R>
using IsRandomAccessRange = core::And<
    core::Or<IsBidirectionalRange<R>, core::And<IsForwardRange<R>, IsInfinite<R>>>, //
    core::IsDetectedExact<ValueType<R>, core::OpIndexDetector, R>, // typeof(r[0]) == typeof(r.front)
    core::Or<core::HasSize<R>, IsInfinite<R>> //
    >;

template <class R>
inline constexpr bool isRandomAccessRange = IsRandomAccessRange<R>::value;

template <class R>
using IsFiniteRandomAccessRange = core::And<core::Not<IsInfinite<R>>, IsRandomAccessRange<R>>;

template <class R>
inline constexpr bool isFiniteRandomAccessRange = IsFiniteRandomAccessRange<R>::value;

namespace impl {

	template <class R, class E>
	using PutMemberDetector = decltype(RB_DECLVAL(R).put(RB_DECLVAL(E)));

	template <class R, class E>
	using HasPutMember = core::IsDetected<PutMemberDetector, R, E>;

	template <class R, class E>
	using CanDoPut = core::Or<
	    HasPutMember<R, E>,
	    core::And<IsInputRange<R>, core::IsAssignable<ValueType<R>, E>>,
	    core::IsInvocable<R, E>>;

	template <class R, class E>
	constexpr bool isDoPutNothrow() noexcept {
		if constexpr (HasPutMember<R, E>::value) {
			return noexcept(RB_DECLVAL(R).put(RB_DECLVAL(E)));
		} else if constexpr (isInputRange<R> && core::isAssignable<ValueType<R>, E>) {
			return noexcept(front(RB_DECLVAL(R)) = RB_FWD(RB_DECLVAL(E))) //
			    && noexcept(popFront(RB_DECLVAL(R)));
		} else if constexpr (core::isInvocable<R, E>) {
			return core::isNothrowInvocable<R, E>;
		} else {
			return false;
		}
	}

	template <class R, class E,
	    RB_REQUIRES_T(CanDoPut<R, E>)>
	constexpr void doPut(R& r, E&& e) noexcept(isDoPutNothrow<R, E>()) {
		auto&& x = RB_FWD(e);
		if constexpr (HasPutMember<R, E>::value) {
			r.put(x);
		} else if constexpr (isInputRange<R> && core::isAssignable<ValueType<R>, E>) {
			front(r) = x;
			popFront(r);
		} else if constexpr (core::isInvocable<R, E>) {
			core::invoke(r, x);
		} else {
			static_assert(core::alwaysFalse<R>);
		}
	}

	template <class R, class E>
	using DoPutDetector = decltype(doPut(RB_DECLVAL(core::AddLValueRef<R>), RB_DECLVAL(E)));

	template <class R, class E>
	using HasDoPut = core::IsDetected<DoPutDetector, R, E>;

	template <class R, class E>
	inline constexpr bool hasDoPut = HasDoPut<R, E>::value;

} // namespace impl

template <class R, class E,
    RB_REQUIRES_T(impl::HasDoPut<R, E>)>
constexpr void put(R& r, E&& e) noexcept(impl::isDoPutNothrow<R, E>()) {
	impl::doPut(r, RB_FWD(e));
}

namespace impl {
	template <class R, class E>
	using PutDetector = decltype(put(RB_DECLVAL(core::AddLValueRef<R>), RB_DECLVAL(E)));

	template <class R, class E>
	inline constexpr bool isPutNothrow = //
	    hasDoPut<R, E> && isDoPutNothrow<R, E>()
	    || !hasDoPut<R, E>
	           && noexcept(put(RB_DECLVAL(R&), RB_DECLVAL(ValueType<E>)))
	           && noexcept(empty(RB_DECLVAL(E)))
	           && noexcept(popFront(RB_DECLVAL(E)));
} // namespace impl

template <class R, class E>
struct IsOutputRange : core::IsDetected<impl::PutDetector, R, E> {};

template <class Char, class Traits, class E>
struct IsOutputRange<std::basic_ostream<Char, Traits>, E> : core::False {};

template <class R, class E>
inline constexpr bool isOutputRange = IsOutputRange<R, E>::value;

template <class R, class E,
    RB_REQUIRES(!impl::hasDoPut<R, E> && isInputRange<core::RemoveRef<E>> && isOutputRange<R, ValueType<E>>)>
constexpr void put(R& r, E&& e) noexcept(impl::isPutNothrow<R, E>) {
	auto&& x = RB_FWD(e);
	for (; !empty(x); popFront(x)) {
		// TODO add checks for core::size
		put(r, front(x));
	}
}

template <class R,
    RB_REQUIRES(isInputRange<R>&& core::isCopyConstructible<R>)>
constexpr R range(R const& r) noexcept(core::isNothrowCopyConstructible<R>) {
	return r;
}

} // namespace rb::ranges
