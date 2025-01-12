#pragma once

#include <rb/time/Clock.hpp>

namespace rb::time {

template <class Clock,
    RB_REQUIRES_T(IsClock<Clock>)>
class TimePoint final {
public:
	// it should be a package method
	static constexpr TimePoint from(Duration dur) noexcept {
		return TimePoint{dur};
	}

	static TimePoint now() noexcept {
		return TimePoint{Clock::now()};
	}

	constexpr TimePoint() noexcept = default;

	constexpr bool operator==(TimePoint rhs) const noexcept {
		return rep_ == rhs.rep_;
	}

	constexpr bool operator<(TimePoint rhs) const noexcept {
		return rep_ < rhs.rep_;
	}

	constexpr TimePoint& operator+=(Duration dur) noexcept {
		rep_ += dur;
		return *this;
	}

	constexpr TimePoint& operator-=(Duration dur) noexcept {
		rep_ -= dur;
		return *this;
	}

	template <bool _ = true, RB_REQUIRES(_&& Clock::kIsMonotonic)>
	constexpr Duration operator-(TimePoint rhs) const noexcept {
		return rep_ - rhs.rep_;
	}

	constexpr bool isInf() const noexcept {
		return rep_.isInf();
	}

	constexpr Duration since(TimePoint rhs) const noexcept {
		return rep_ - rhs.rep_;
	}

	constexpr Duration sinceEpoch() const noexcept {
		return rep_;
	}

	Duration elapsed() const noexcept {
		return now().since(*this);
	}

private:
	friend std::ostream& operator<<(std::ostream& os, TimePoint timePoint) {
		return os << "TimePoint{" << timePoint.rep_ << "}";
	}

	constexpr explicit TimePoint(Duration rep) noexcept
	    : rep_{rep} {
	}

	Duration rep_;
};

#pragma region synthesized operators

template <class Clock>
constexpr bool operator>(TimePoint<Clock> lhs, TimePoint<Clock> rhs) noexcept {
	return rhs < lhs;
}

template <class Clock>
constexpr bool operator>=(TimePoint<Clock> lhs, TimePoint<Clock> rhs) noexcept {
	return !(lhs < rhs);
}

template <class Clock>
constexpr bool operator<=(TimePoint<Clock> lhs, TimePoint<Clock> rhs) noexcept {
	return !(rhs < lhs);
}

template <class Clock>
constexpr bool operator!=(TimePoint<Clock> lhs, TimePoint<Clock> rhs) noexcept {
	return !(lhs == rhs);
}

template <class Clock>
constexpr TimePoint<Clock> operator+(TimePoint<Clock> lhs, Duration rhs) noexcept {
	return lhs += rhs;
}

template <class Clock>
constexpr TimePoint<Clock> operator+(Duration lhs, TimePoint<Clock> rhs) noexcept {
	return rhs += lhs;
}

template <class Clock>
constexpr TimePoint<Clock> operator-(TimePoint<Clock> lhs, Duration rhs) noexcept {
	return lhs -= rhs;
}

#pragma endregion synthesized operators

using MonotonicTimePoint = TimePoint<Clock<ClockId::kMonotonic>>;
using MonotonicFastTimePoint = TimePoint<Clock<ClockId::kMonotonicFast>>;
using RealtimeTimePoint = TimePoint<Clock<ClockId::kRealtime>>;
using RealtimeFastTimePoint = TimePoint<Clock<ClockId::kRealtimeFast>>;
using UptimeTimePoint = TimePoint<Clock<ClockId::kUptime>>;
using ProcessCpuTimePoint = TimePoint<Clock<ClockId::kProcessCpuTime>>;
using ThreadCpuTimePoint = TimePoint<Clock<ClockId::kThreadCpuTime>>;
#ifndef RB_OS_WIN
using TaiTimePoint = TimePoint<Clock<ClockId::kTai>>;
#endif

} // namespace rb::time
