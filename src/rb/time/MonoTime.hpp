#pragma once

#include <rb/time/TimePoint.hpp>

namespace rb::time {

/// A measurement of a monotonically nondecreasing clock.
///
/// A clock is *monotonic* if the value returned by a first call to now() is always less than or equal to the value
/// that is returned by a later call to now().
///
/// A clock is *steady* if the time between clock ticks is constant.
/// MonoTime is **not** guaranteed to be steady.
class MonoTime final {
	using Impl = TimePoint<Clock<ClockId::kMonotonic>>;

public:
	static MonoTime now() noexcept {
		return MonoTime{Impl::now()};
	}

	constexpr MonoTime() noexcept = default;

	constexpr bool operator==(MonoTime rhs) const noexcept {
		return impl_ == rhs.impl_;
	}

	constexpr bool operator<(MonoTime rhs) const noexcept {
		return impl_ < rhs.impl_;
	}

	constexpr MonoTime& operator+=(Duration dur) noexcept {
		impl_ += dur;
		return *this;
	}

	constexpr MonoTime& operator-=(Duration dur) noexcept {
		impl_ -= dur;
		return *this;
	}

	constexpr Duration operator-(MonoTime rhs) const noexcept {
		return since(rhs);
	}

	constexpr bool isInf() const noexcept {
		return impl_.isInf();
	}

	/// @return the amount of time elapsed from @p rhs to *this, or zero duration if @p rhs is later than *this.
	constexpr Duration since(MonoTime rhs) const noexcept {
		auto const duration = impl_ - rhs.impl_;
		return duration.isNegative() ? Duration::zero() : duration;
	}

	Duration elapsed() const noexcept {
		return impl_.elapsed();
	}

private:
	friend std::ostream& operator<<(std::ostream& os, MonoTime monoTime) {
		return os << "Instant{" << monoTime.impl_.sinceEpoch() << "}";
	}

	constexpr explicit MonoTime(Impl impl) noexcept
	    : impl_{impl} {
	}

	Impl impl_;
};

using Instant = MonoTime;

#pragma region synthesized operators

constexpr bool operator>(MonoTime lhs, MonoTime rhs) noexcept {
	return rhs < lhs;
}

constexpr bool operator>=(MonoTime lhs, MonoTime rhs) noexcept {
	return !(lhs < rhs);
}

constexpr bool operator<=(MonoTime lhs, MonoTime rhs) noexcept {
	return !(rhs < lhs);
}

constexpr bool operator!=(MonoTime lhs, MonoTime rhs) noexcept {
	return !(lhs == rhs);
}

constexpr MonoTime operator+(MonoTime lhs, Duration rhs) noexcept {
	return lhs += rhs;
}

constexpr MonoTime operator+(Duration lhs, MonoTime rhs) noexcept {
	return rhs += lhs;
}

constexpr MonoTime operator-(MonoTime lhs, Duration rhs) noexcept {
	return lhs -= rhs;
}

#pragma endregion synthesized operators

} // namespace rb::time
