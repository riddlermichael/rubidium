#include "LongDuration.hpp"

#include <algorithm>

#include <rb/core/slices/primitives.hpp>

using namespace rb::core;
using namespace rb::time;

namespace {

Unit const kHourUnit{LongDuration::kHour, "h"};
Unit const kMinuteUnit{LongDuration::kMinute, "min"};
Unit const kSecondUnit{LongDuration::kSecond, "s"};
Unit const kMillisecondUnit{LongDuration::kMillisecond, "ms"};
Unit const kMicrosecondUnit{LongDuration::kMicrosecond, "us"};
Unit const kNanosecondUnit{LongDuration::kNanosecond, "ns"};

std::vector<Unit> getUnits(LongDuration dur) noexcept {
	dur = abs(dur);
	if (dur < LongDuration::kSecond) {
		if (dur < LongDuration::kMicrosecond) {
			return {kNanosecondUnit};
		}
		if (dur < LongDuration::kMillisecond) {
			return {kMicrosecondUnit};
		}
		return {kMillisecondUnit};
	}
	return {kHourUnit, kMinuteUnit, kSecondUnit};
}

struct UnitComparator {
	constexpr bool operator()(Unit const& lhs, Unit const& rhs) const noexcept {
		auto const cmp = lhs.duration.opCmp(rhs.duration);
		if (cmp) {
			return cmp > 0;
		}

		return lhs.symbol < rhs.symbol;
	}
};

constexpr bool equals(Unit const& lhs, Unit const& rhs) noexcept {
	return lhs.duration == rhs.duration;
}

} // namespace

namespace rb::time {

SplitExpected<u128> split(LongDuration dur, Span<Unit const> units) {
#if RB_ENABLED(DURATION_NAN)
	if (dur.isNaN()) {
		return err(SplitError::kNaN);
	}
	{
		Unit const* const it = std::find_if(units.begin(), units.end(), [](Unit const& unit) {
			return unit.duration.isNaN();
		});
		if (it != units.end()) {
			return err(SplitError::kNaN);
		}
	}
#endif

	if (units.empty()) {
		return err(SplitError::kNoUnits);
	}

	{
		Unit const* const it = std::find_if(units.begin(), units.end(), [](Unit const& unit) {
			return !unit.duration || unit.symbol.empty();
		});
		if (it != units.end()) {
			return err(SplitError::kZeroUnit);
		}
	}

	std::vector v(units.begin(), units.end());
	std::sort(v.begin(), v.end(), UnitComparator{});
	auto const uniqueIt = std::unique(v.begin(), v.end(), equals);
	Span const uniqueUnits{v.begin(), uniqueIt};
	if (uniqueUnits.empty()) {
		return err(SplitError::kNotUnique);
	}

	if (dur.isInf()) {
		SplitResult<u128> result{{}, dur, dur.isPositive(), true};
		return ok(RB_MOVE(result));
	}

	QuantityList<u128> quantities;
	bool const isPositive = dur.isPositive();
	dur = abs(dur);
	quantities.reserve(uniqueUnits.size());
	for (auto const& unit : uniqueUnits) {
		auto const& [quo, rem] = quorem(dur, unit.duration);
		u128 const count{quo};
		dur = rem;
		quantities.push_back({count, unit});
	}

	SplitResult<u128> result{quantities, dur, isPositive, false};
	return ok(RB_MOVE(result));
}

std::ostream& operator<<(std::ostream& os, LongDuration dur) {
#if RB_ENABLED(DURATION_NAN)
	if (dur.isNaN()) {
		return os << "nan";
	}
#endif

	if (dur.isZero()) {
		return os << "0s";
	}

	auto const&& exp = split(dur, getUnits(dur));
	auto const& result = exp.unwrap();
	if (!result.isPositive) {
		os << "-";
	}
	if (result.isInf) {
		return os << "inf";
	}

	Span const quantities{result.quantities};
	for (auto const& [count, unit] : quantities[slice(-1)]) {
		if (count) {
			os << count << unit.symbol;
		}
	}
	// last unit
	auto const& [count, unit] = quantities.back();
	if (result.rem) {
		auto const frac = static_cast<double>(count) + result.rem.div(unit.duration);
		os << frac << unit.symbol;
	} else {
		if (count) {
			os << count << unit.symbol;
		}
	}
	return os;
}

} // namespace rb::time
