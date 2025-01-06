#pragma once

#include <rb/time/Duration.hpp>

namespace rb::time {

/// A measurement of the system's real-time clock.
///
/// A clock is *monotonic* if the value returned by a first call to now() is always less than or equal to the value
/// that is returned by a later call to now().
/// The SystemTime isn't monotonic because the system time may be adjusted between calls to now().
///
/// A clock is *steady* if the time between clock ticks is constant.
class SystemTime final {
public:
	using Result = core::Expected<Duration, Duration>;

	static SystemTime const kUnixEpoch;

	static constexpr SystemTime from(std::time_t t) noexcept;
	static constexpr SystemTime from(std::timespec ts) noexcept;

	static SystemTime now() noexcept;

	constexpr SystemTime() noexcept = default;

	constexpr bool operator==(SystemTime rhs) const noexcept;
	constexpr bool operator<(SystemTime rhs) const noexcept;

	constexpr SystemTime& operator+=(Duration dur) noexcept;
	constexpr SystemTime& operator-=(Duration dur) noexcept;

	constexpr std::time_t toTimeT() const noexcept;
	constexpr std::timespec toTimespec() const noexcept;

	constexpr bool isInf() const noexcept;
	constexpr Result since(SystemTime rhs) const noexcept;

	Result elapsed() const noexcept;

private:
	friend std::ostream& operator<<(std::ostream& os, SystemTime clock);

	constexpr explicit SystemTime(Duration rep) noexcept
	    : rep_{rep} {
	}

	Duration rep_;
};

std::ostream& operator<<(std::ostream& os, SystemTime clock);

constexpr SystemTime SystemTime::kUnixEpoch;

constexpr SystemTime SystemTime::from(std::time_t t) noexcept {
	return SystemTime{seconds(t)};
}

constexpr SystemTime SystemTime::from(std::timespec ts) noexcept {
	return SystemTime{Duration::from(ts)};
}

constexpr bool SystemTime::operator==(SystemTime rhs) const noexcept {
	return rep_ == rhs.rep_;
}

constexpr bool SystemTime::operator<(SystemTime rhs) const noexcept {
	return rep_ < rhs.rep_;
}

constexpr SystemTime& SystemTime::operator+=(Duration dur) noexcept {
	rep_ += dur;
	return *this;
}

constexpr SystemTime& SystemTime::operator-=(Duration dur) noexcept {
	rep_ -= dur;
	return *this;
}

constexpr std::time_t SystemTime::toTimeT() const noexcept {
	return toTimespec().tv_sec;
}

constexpr std::timespec SystemTime::toTimespec() const noexcept {
	return rep_.toTimespec();
}

constexpr bool SystemTime::isInf() const noexcept {
	return rep_.isInf();
}

constexpr SystemTime::Result SystemTime::since(SystemTime rhs) const noexcept {
	auto const duration = rep_ - rhs.rep_;
	if (duration.isNegative()) {
		return core::err(-duration);
	}
	return core::ok(duration);
}

#pragma region synthesized operators

constexpr bool operator>(SystemTime lhs, SystemTime rhs) noexcept {
	return rhs < lhs;
}

constexpr bool operator>=(SystemTime lhs, SystemTime rhs) noexcept {
	return !(lhs < rhs);
}

constexpr bool operator<=(SystemTime lhs, SystemTime rhs) noexcept {
	return !(rhs < lhs);
}

constexpr bool operator!=(SystemTime lhs, SystemTime rhs) noexcept {
	return !(lhs == rhs);
}

constexpr SystemTime operator+(SystemTime lhs, Duration rhs) noexcept {
	return lhs += rhs;
}

constexpr SystemTime operator+(Duration lhs, SystemTime rhs) noexcept {
	return rhs += lhs;
}

constexpr SystemTime operator-(SystemTime lhs, Duration rhs) noexcept {
	return lhs -= rhs;
}

#pragma endregion synthesized operators

} // namespace rb::time
