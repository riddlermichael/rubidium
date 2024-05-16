#pragma once

#include <rb/core/Span.hpp>
#include <rb/ranges/traits.hpp>

namespace rb::ranges {

namespace impl {

	template <class T>
	using RangeMethodDetector = decltype(RB_DECLVAL(T).range());

} // namespace impl

template <class T>
using HasRangeMethod = core::IsDetected<impl::RangeMethodDetector, T>;

template <class T>
inline constexpr bool hasRangeMethod = HasRangeMethod<T>::value;

template <class R,
    RB_REQUIRES(isInputRange<core::RemoveRef<R>>)>
[[nodiscard]] constexpr R&& range(R&& r) noexcept {
	return r;
}

template <class C,
    RB_REQUIRES(!isInputRange<C> && hasRangeMethod<C>)>
[[nodiscard]] constexpr decltype(auto) range(C& c) RB_NOEXCEPT_LIKE(c.range()) {
	return c.range();
}

template <class C,
    RB_REQUIRES(!isInputRange<C> && hasRangeMethod<C const>)>
[[nodiscard]] constexpr decltype(auto) range(C const& c) RB_NOEXCEPT_LIKE(c.range()) {
	return c.range();
}

template <class T, usize n>
[[nodiscard]] constexpr core::Span<T> range(T (&a)[n]) noexcept {
	return a;
}

template <class T>
[[nodiscard]] constexpr core::Span<T const> range(std::initializer_list<T> il) noexcept {
	return {il.begin(), il.size()};
}

template <class C>
[[nodiscard]] constexpr decltype(auto) crange(C const& c) RB_NOEXCEPT_LIKE(ranges::range(c)) {
	return ranges::range(c);
}

} // namespace rb::ranges
