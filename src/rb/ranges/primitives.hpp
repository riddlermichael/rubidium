#pragma once

#include <rb/core/iter/primitives.hpp>
#include <rb/core/move.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/declval.hpp>
#include <rb/core/traits/detection.hpp>

namespace rb::ranges {

namespace impl {

	template <class T>
	using FrontMemberDetector = decltype(RB_DECLVAL(T).front());

	template <class T>
	using HasFrontMember = core::IsDetected<FrontMemberDetector, T>;

	template <class T>
	using PopFrontMemberDetector = decltype(RB_DECLVAL(T).popFront());

	template <class T>
	using HasPopFrontMember = core::IsDetected<PopFrontMemberDetector, T>;

	template <class T>
	using SaveMemberDetector = decltype(RB_DECLVAL(T).save());

	template <class T>
	using HasSaveMember = core::IsDetected<SaveMemberDetector, T>;

	template <class T>
	using BackMemberDetector = decltype(RB_DECLVAL(T).back());

	template <class T>
	using HasBackMember = core::IsDetected<BackMemberDetector, T>;

	template <class T>
	using PopBackMemberDetector = decltype(RB_DECLVAL(T).popBack());

	template <class T>
	using HasPopBackMember = core::IsDetected<PopBackMemberDetector, T>;

} // namespace impl

template <class R,
    RB_REQUIRES_T(core::impl::HasEmptyMember<R>)>
constexpr decltype(auto) empty(R&& r) RB_NOEXCEPT_LIKE(RB_FWD(r).empty()) {
	return RB_FWD(r).empty();
}

template <class R,
    RB_REQUIRES_T(impl::HasFrontMember<R>)>
constexpr decltype(auto) front(R&& r) RB_NOEXCEPT_LIKE(RB_FWD(r).front()) {
	return RB_FWD(r).front();
}

template <class R,
    RB_REQUIRES_T(impl::HasPopFrontMember<R>)>
constexpr void popFront(R&& r) RB_NOEXCEPT_LIKE(RB_FWD(r).popFront()) {
	RB_FWD(r).popFront();
}

template <class R,
    RB_REQUIRES_T(impl::HasSaveMember<R>)>
constexpr decltype(auto) save(R&& r) RB_NOEXCEPT_LIKE(RB_FWD(r).save()) {
	return RB_FWD(r).save();
}

template <class R,
    RB_REQUIRES_T(impl::HasBackMember<R>)>
constexpr decltype(auto) back(R&& r) RB_NOEXCEPT_LIKE(RB_FWD(r).back()) {
	return RB_FWD(r).back();
}

template <class R,
    RB_REQUIRES_T(impl::HasPopBackMember<R>)>
constexpr void popBack(R&& r) RB_NOEXCEPT_LIKE(RB_FWD(r).popBack()) {
	RB_FWD(r).popBack();
}

template <class R,
    RB_REQUIRES_T(core::impl::HasSizeMember<R>)>
[[nodiscard]] constexpr decltype(auto) size(R&& r) RB_NOEXCEPT_LIKE(RB_FWD(r).size()) {
	return RB_FWD(r).size();
}

template <class R,
    RB_REQUIRES_T(core::And<
        core::Not<core::IsBoundedArray<core::RemoveRef<R>>>,
        core::Not<core::impl::HasSizeMember<R>>>)>
[[nodiscard]] constexpr usize size(R&& r)
    noexcept(noexcept(!empty(RB_FWD(r)) && noexcept(popFront(RB_FWD(r))))) //
{
	usize size = 0;
	for (; !empty(RB_FWD(r)); popFront(RB_FWD(r))) {
		++size;
	}
	return size;
}

template <class T, usize n>
[[nodiscard]] constexpr usize size(T const (&a)[n]) noexcept {
	RB_UNUSED(a);
	return n;
}

struct FromRange {
	constexpr explicit FromRange() = default;
};

inline constexpr FromRange kFromRange;

} // namespace rb::ranges
