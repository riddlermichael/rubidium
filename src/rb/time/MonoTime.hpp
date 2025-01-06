#pragma once

#include <rb/time/Duration.hpp>

namespace rb::time {

/// A measurement of a monotonically nondecreasing clock.
///
/// A clock is *monotonic* if the value returned by a first call to now() is always less than or equal to the value
/// that is returned by a later call to now().
///
/// A clock is *steady* if the time between clock ticks is constant.
///
/// MonoTime is **not** guaranteed to be steady. However, on all supported platforms, monotonic clocks are steady now.
class MonoTime final {
public:
	static MonoTime now() noexcept;

	constexpr MonoTime() noexcept = default;

	constexpr bool operator==(MonoTime rhs) const noexcept;
	constexpr bool operator<(MonoTime rhs) const noexcept;

	constexpr MonoTime& operator+=(Duration dur) noexcept;
	constexpr MonoTime& operator-=(Duration dur) noexcept;

	/// @return the amount of time elapsed from @p rhs to *this, or zero duration if @p rhs is later than *this.
	constexpr Duration operator-(MonoTime rhs) const noexcept;

	constexpr bool isInf() const noexcept;
	constexpr Duration since(MonoTime rhs) const noexcept;

	Duration elapsed() const noexcept;

private:
	friend std::ostream& operator<<(std::ostream& os, MonoTime clock);

	constexpr explicit MonoTime(Duration rep) noexcept
	    : rep_{rep} {
	}

	Duration rep_;
};

constexpr bool MonoTime::operator==(MonoTime rhs) const noexcept {
	return rep_ == rhs.rep_;
}

constexpr bool MonoTime::operator<(MonoTime rhs) const noexcept {
	return rep_ < rhs.rep_;
}

constexpr MonoTime& MonoTime::operator+=(Duration dur) noexcept {
	rep_ += dur;
	return *this;
}

constexpr MonoTime& MonoTime::operator-=(Duration dur) noexcept {
	rep_ -= dur;
	return *this;
}

constexpr Duration MonoTime::operator-(MonoTime rhs) const noexcept {
	return since(rhs);
}

constexpr bool MonoTime::isInf() const noexcept {
	return rep_.isInf();
}

constexpr Duration MonoTime::since(MonoTime rhs) const noexcept {
	auto const duration = rep_ - rhs.rep_;
	return duration.isNegative() ? Duration::zero() : duration;
}

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

std::ostream& operator<<(std::ostream& os, MonoTime clock);

} // namespace rb::time
