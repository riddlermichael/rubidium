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
	if (RB_UNLIKELY(math::isNaN(center) || math::isNaN(radius))) {
		return kNaN;
	}

	RB_ASSERT(!math::isNegative(radius));
	return center + Self{Tag{}, -radius, radius};
}

Interval64 Interval64::operator+(Self const& rhs) const noexcept {
	if (RB_UNLIKELY(isNaN() || rhs.isNaN())) {
		return kNaN;
	}

	if (RB_UNLIKELY(isEmpty() || rhs.isEmpty())) {
		return kEmpty;
	}

	RoundSaver const _;
	std::fesetround(FE_DOWNWARD);
	f64 const lo = lo_ + rhs.lo_;
	std::fesetround(FE_UPWARD);
	f64 const hi = hi_ + rhs.hi_;
	return {lo, hi};
}

Interval64 Interval64::operator*(Self const& rhs) const noexcept {
	if (RB_UNLIKELY(isNaN() || rhs.isNaN())) {
		return kNaN;
	}

	if (RB_UNLIKELY(isEmpty() || rhs.isEmpty())) {
		return kEmpty;
	}

	RoundSaver const _;

	std::fesetround(FE_DOWNWARD);
	f64 p1 = lo_ * rhs.lo_;
	f64 p2 = lo_ * rhs.hi_;
	f64 p3 = hi_ * rhs.lo_;
	f64 p4 = hi_ * rhs.hi_;
	f64 const lo = min(p1, p2, p3, p4);

	std::fesetround(FE_UPWARD);
	p1 = lo_ * rhs.lo_;
	p2 = lo_ * rhs.hi_;
	p3 = hi_ * rhs.lo_;
	p4 = hi_ * rhs.hi_;
	f64 const hi = max(p1, p2, p3, p4);

	return {lo, hi};
}

// TODO x * value == x * [value, value]
Interval64 Interval64::operator*(f64 value) const noexcept {
	if (RB_UNLIKELY(isNaN() || math::isNaN(value))) {
		return kNaN;
	}

	if (RB_UNLIKELY(isEmpty())) {
		return kEmpty;
	}

	if (value == 0.0) {
		return kZero;
	}

	if (math::isInf(value)) {
		if (lo_ == 0.0 && hi_ == 0) {
			return kNaN;
		}

		if (lo_ < 0.0 && hi_ > 0.0) {
			return kWhole;
		}

		bool const isPositive = lo_ >= 0 && value > 0.0 || hi_ <= 0 && value < 0.0;
		return isPositive ? kPositiveRay : kNegativeRay;
	}

	RoundSaver const _;
	std::fesetround(FE_DOWNWARD);
	f64 const lo = (value > 0.0 ? lo_ : hi_) * value;
	std::fesetround(FE_UPWARD);
	f64 const hi = (value > 0.0 ? hi_ : lo_) * value;
	return Self{Tag{}, lo, hi};
}

f64 Interval64::center() const noexcept {
	if (RB_UNLIKELY(isEmptyOrNaN())
	    || isWhole()) {
		return core::nan<f64>;
	}

	RoundSaver const _;
	std::fesetround(FE_TONEAREST);
	f64 const r = (hi_ - lo_) / 2;
	return lo_ + r;
}

f64 Interval64::radius() const noexcept {
	if (RB_UNLIKELY(isEmptyOrNaN())) {
		return core::nan<f64>;
	}

	if (isWhole()) {
		return core::inf<f64>;
	}

	RoundSaver const _;
	std::fesetround(FE_UPWARD);
	return (hi_ - lo_) / 2;
}

f64 Interval64::width() const noexcept {
	if (RB_UNLIKELY(isEmptyOrNaN())) {
		return core::nan<f64>;
	}

	if (isWhole()) {
		return core::inf<f64>;
	}

	RoundSaver const _;
	std::fesetround(FE_UPWARD);
	return hi_ - lo_;
}

std::ostream& rb::interval::operator<<(std::ostream& os, Interval64 interval) {
	if (RB_UNLIKELY(interval.isEmpty())) {
		return os << "[]";
	}

	if (RB_UNLIKELY(interval.isNaN())) {
		return os << "nan";
	}

	fmt::StreamStateSaver const _{os};
	os.precision(18);
	return os
	    << "["
	    << interval.lo()
	    << ", "
	    << interval.hi()
	    << "]";
}
