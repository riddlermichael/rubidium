#pragma once

#include <rb/ranges/traits.hpp>

namespace rb::ranges {

template <class S, class E,
    RB_REQUIRES_T(core::IsWritableTo<E, S>)>
class OutputRange final {
public:
	constexpr explicit OutputRange(S& s) noexcept
	    : s_(s) {
	}

	constexpr void put(E const& e) RB_NOEXCEPT_LIKE(s_ << e) {
		s_ << e;
	}

private:
	S& s_;
};

template <class E, class S,
    RB_REQUIRES(!isOutputRange<S, E> && core::isWritableTo<E, S>)>
constexpr auto range(S& s) noexcept {
	return OutputRange<S, E>{s};
}

template <class E, class R,
    RB_REQUIRES(isOutputRange<R, E>&& core::isCopyConstructible<R>)>
constexpr R range(R const& r) noexcept {
	return r;
}

} // namespace rb::ranges
