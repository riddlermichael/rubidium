#pragma once

#include <rb/core/helpers.hpp>
#include <rb/core/traits/remove.hpp>

namespace rb::core {

template <class Q, class R = Q>
struct QuoRem {
	Q quo;
	R rem;
};

template <class Q, class R>
QuoRem(Q, R) -> QuoRem<RemoveRef<Q>, RemoveRef<R>>;

template <class T, class U>
constexpr auto quorem(T const& dividend, U const& divisor)
    RB_NOEXCEPT_LIKE(QuoRem{dividend / divisor, dividend % divisor}) {
	return QuoRem{dividend / divisor, dividend % divisor};
}

} // namespace rb::core
