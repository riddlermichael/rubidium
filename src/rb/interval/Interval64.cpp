#include "Interval64.hpp"

#include <cfenv>
#include <ostream>

#include <rb/core/assert.hpp>
#include <rb/fmt/StreamStateSaver.hpp>
#include <rb/interval/RoundSaver.hpp>

#ifndef RB_COMPILER_MSVC
	#pragma STDC FENV_ACCESS ON
#endif

using namespace rb::interval;
using namespace rb::math;

Interval64 Interval64::fromCenter(f64 center, f64 radius) noexcept {
	if (RB_UNLIKELY(math::isNaN(radius))) {
		return nan();
	}

	RB_ASSERT(!isNegative(radius));
	return center + Self{Tag{}, -radius, radius};
}

Interval64 Interval64::operator+(Self const& rhs) const noexcept {
	// TODO isEmpty, isNaN
	RoundSaver const _;
	std::fesetround(FE_DOWNWARD);
	f64 const lo = lo_ + rhs.lo_;
	std::fesetround(FE_UPWARD);
	f64 const hi = hi_ + rhs.hi_;
	return {lo, hi};
}

std::ostream& rb::interval::operator<<(std::ostream& os, Interval64 const& value) {
	if (RB_UNLIKELY(value.isEmpty())) {
		return os << "[]";
	}

	if (RB_UNLIKELY(value.isNaN())) {
		return os << "nan";
	}

	fmt::StreamStateSaver const _{os};
	os.precision(18);
	return os
	    << "["
	    << value.lo()
	    << ", "
	    << value.hi()
	    << "]";
}
