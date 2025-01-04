#pragma once

#include <rb/time/Duration.hpp>

namespace rb::time {

class SystemClock final {
public:
	static constexpr bool kIsSteady = false;

	static SystemClock const kUnixEpoch;

	static constexpr SystemClock from(std::time_t t) noexcept;
	static constexpr SystemClock from(std::timespec ts) noexcept;

	static SystemClock now() noexcept;

	constexpr SystemClock() noexcept = default;

	constexpr std::time_t toTimeT() const noexcept;
	constexpr std::timespec toTimespec() const noexcept;

private:
	friend std::ostream& operator<<(std::ostream& os, SystemClock clock);

	constexpr explicit SystemClock(Duration rep) noexcept
	    : rep_{rep} {
	}

	Duration rep_;
};

std::ostream& operator<<(std::ostream& os, SystemClock clock);

constexpr SystemClock SystemClock::kUnixEpoch;

constexpr SystemClock SystemClock::from(std::time_t t) noexcept {
	return SystemClock{seconds(t)};
}

constexpr SystemClock SystemClock::from(std::timespec ts) noexcept {
	return SystemClock{Duration::from(ts)};
}

constexpr std::time_t SystemClock::toTimeT() const noexcept {
	return toTimespec().tv_sec;
}

constexpr std::timespec SystemClock::toTimespec() const noexcept {
	return rep_.toTimespec();
}

} // namespace rb::time
