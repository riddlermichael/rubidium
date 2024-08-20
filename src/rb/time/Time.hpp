#pragma once

#include <rb/time/Duration.hpp>

namespace rb::time {

class Time;

namespace impl {
	constexpr Duration toUnixDuration(Time t) noexcept;
} // namespace impl

class Time final {
public:
	static Time deadlineFromTimeout(Duration timeout) noexcept;
	static Time now() noexcept;

	/// Returns the Unix epoch.
	/// However, those reading your code may not know or expect the Unix epoch as the default value,
	/// so make your code more readable by explicitly initializing all instances before use.
	constexpr Time() noexcept = default;
	constexpr Time(Time const&) noexcept = default;
	constexpr Time(Time&&) noexcept = default;
	~Time() = default;

	constexpr Time& operator=(Time const&) noexcept = default;
	constexpr Time& operator=(Time&&) noexcept = default;

	constexpr Duration operator-(Time rhs) const noexcept;
	constexpr bool operator==(Time rhs) const noexcept;
	constexpr bool operator<(Time rhs) const noexcept;
	constexpr Time& operator+=(Duration d) noexcept;
	constexpr Time& operator-=(Duration d) noexcept;

private:
	friend constexpr Duration impl::toUnixDuration(Time t) noexcept;

	constexpr explicit Time(Duration rep) noexcept
	    : rep_(rep) {
	}

	Duration rep_;
};

std::ostream& operator<<(std::ostream& os, Time t);

namespace impl {
	constexpr Duration toUnixDuration(Time t) noexcept {
		return t.rep_;
	}
} // namespace impl

constexpr Duration Time::operator-(Time rhs) const noexcept {
	return rep_ - rhs.rep_;
}

constexpr bool Time::operator==(Time rhs) const noexcept {
	return rep_ == rhs.rep_;
}

constexpr bool Time::operator<(Time rhs) const noexcept {
	return rep_ < rhs.rep_;
}

constexpr Time& Time::operator+=(Duration d) noexcept {
	rep_ += d;
	return *this;
}

constexpr Time& Time::operator-=(Duration d) noexcept {
	rep_ -= d;
	return *this;
}

// synthesized operators

constexpr bool operator>(Time lhs, Time rhs) noexcept {
	return rhs < lhs;
}

constexpr bool operator>=(Time lhs, Time rhs) noexcept {
	return !(lhs < rhs);
}

constexpr bool operator<=(Time lhs, Time rhs) noexcept {
	return !(rhs < lhs);
}

constexpr bool operator!=(Time lhs, Time rhs) noexcept {
	return !(lhs == rhs);
}

constexpr Time operator+(Time lhs, Duration rhs) noexcept {
	return lhs += rhs;
}

constexpr Time operator+(Duration lhs, Time rhs) noexcept {
	return rhs += lhs;
}

constexpr Time operator-(Time lhs, Duration rhs) noexcept {
	return lhs -= rhs;
}

} // namespace rb::time
