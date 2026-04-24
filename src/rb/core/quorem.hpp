#pragma once

#include <rb/core/traits/remove.hpp>
#include <rb/core/warnings.hpp>

namespace rb::core {

RB_WARNING_PUSH
RB_WARNING_PADDING

template <class Q, class R = Q>
struct QuoRem {
	Q quo;
	R rem;
};

RB_WARNING_POP

template <class Q, class R>
QuoRem(Q, R) -> QuoRem<RemoveRef<Q>, RemoveRef<R>>;

template <class T, class U>
constexpr auto quorem(T const& dividend, U const& divisor)
    RB_NOEXCEPT_LIKE(QuoRem{dividend / divisor, dividend % divisor}) {
	return QuoRem{dividend / divisor, dividend % divisor};
}

} // namespace rb::core
