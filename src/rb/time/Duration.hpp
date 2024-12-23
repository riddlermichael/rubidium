#pragma once

#include <chrono>

#include <rb/core/int128.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/enums.hpp>
#include <rb/core/traits/IsArithmetic.hpp>

#define RB_REQUIRES_INTEGRAL(T) RB_REQUIRES(rb::core::isIntegral<T> || rb::core::isEnum<T>)
#define RB_REQUIRES_FLOAT(T) RB_REQUIRES(rb::core::isFloatingPoint<T>)

namespace rb::time {

class Duration;

namespace impl {
	constexpr Duration makeDuration(i64 seconds, i64 ticks = 0) noexcept;
} // namespace impl

class Duration final {
public:
	static constexpr i64 kNanosecondsPerSecond = 1'000'000'000;
	static constexpr auto kSecondsPerMinute = 60U;
	static constexpr auto kSecondsPerHour = 3600U;

	static constexpr u32 kTicksPerSecond = 4'000'000'000U;
	static constexpr unsigned kTicksPerNanosecond = 4U;

	static Duration const kNanosecond;
	static Duration const kMicrosecond;
	static Duration const kMillisecond;
	static Duration const kSecond;
	static Duration const kMinute;
	static Duration const kHour;
	static Duration const kInfinity;
	static Duration const kNegativeInfinity;

	static constexpr Duration inf() noexcept;

	constexpr Duration() noexcept = default;
	constexpr Duration(Duration const&) noexcept = default;
	constexpr Duration(Duration&&) noexcept = default;
	~Duration() = default;

	constexpr Duration& operator=(Duration const&) noexcept = default;
	constexpr Duration& operator=(Duration&&) noexcept = default;

	constexpr explicit operator bool() const noexcept;

	constexpr Duration operator-() const noexcept;

	constexpr bool operator==(Duration rhs) const noexcept;

	constexpr bool operator<(Duration rhs) const noexcept;

	constexpr Duration& operator+=(Duration rhs) noexcept;
	constexpr Duration& operator-=(Duration rhs) noexcept;

	constexpr bool isInf() const noexcept;
	constexpr bool isNeg() const noexcept;

	// for internal use
	constexpr i64 secs() const noexcept;
	constexpr u32 ticks() const noexcept;

private:
	friend constexpr Duration impl::makeDuration(i64 seconds, i64 ticks) noexcept;

	static constexpr u32 kInfTicks = ~0U;

	constexpr Duration(i64 seconds, u32 ticks) noexcept
	    : seconds_(seconds)
	    , ticks_(ticks) {
	}

	i64 seconds_ = 0;
	u32 ticks_ = 0;
};

inline std::ostream& operator<<(std::ostream& os, Duration dur) {
	return os << "Duration{seconds: " << dur.secs() << ", ticks: " << dur.ticks() << "}";
}

} // namespace rb::time

namespace rb::time::impl {

constexpr Duration makeDuration(i64 seconds, i64 ticks) noexcept {
	return {seconds, static_cast<u32>(ticks)};
}

template <i64 n>
constexpr Duration fromInt64(i64 value, std::ratio<1, n> /*unused*/) noexcept {
	static_assert(n <= Duration::kNanosecondsPerSecond, "Unsupported ratio");

	i64 seconds = value / n;
	i64 ticks = value % n * Duration::kTicksPerSecond / n;
	if (ticks < 0) {
		--seconds;
		ticks += Duration::kTicksPerSecond;
	}
	return makeDuration(seconds, ticks);
}

constexpr Duration fromInt64(i64 value, std::ratio<Duration::kSecondsPerMinute> /*unused*/) {
	if (value <= core::max<i64> / Duration::kSecondsPerMinute && //
	    value >= core::min<i64> / Duration::kSecondsPerMinute) {
		return makeDuration(value * Duration::kSecondsPerMinute);
	}
	return value > 0 ? Duration::kInfinity : Duration::kNegativeInfinity;
}

constexpr Duration fromInt64(i64 value, std::ratio<Duration::kSecondsPerHour> /*unused*/) {
	if (value <= core::max<i64> / Duration::kSecondsPerHour && //
	    value >= core::min<i64> / Duration::kSecondsPerHour) {
		return makeDuration(value * Duration::kSecondsPerHour);
	}
	return value > 0 ? Duration::kInfinity : Duration::kNegativeInfinity;
}

} // namespace rb::time::impl

namespace rb::time {

// Duration methods
constexpr Duration Duration::inf() noexcept {
	return kInfinity;
}

constexpr i64 Duration::secs() const noexcept {
	return seconds_;
}

constexpr u32 Duration::ticks() const noexcept {
	return ticks_;
}

constexpr bool Duration::isInf() const noexcept {
	return ticks_ == kInfTicks;
}

constexpr bool Duration::isNeg() const noexcept {
	return seconds_ < 0;
}

constexpr Duration::operator bool() const noexcept {
	return seconds_ || ticks_;
}

constexpr Duration Duration::operator-() const noexcept {
	// if ticks_ is zero, we have it easy; it's safe to negate seconds_,
	// we're dealing with an integral number of seconds,
	// and the only special case is the maximum negative finite duration, which can't be negated
	if (ticks_ == 0) {
		return seconds_ == core::min<i64>
		         ? kInfinity
		         : Duration{-seconds_, 0};
	}

	// infinities stay infinite and just change a direction
	if (isInf()) {
		return (seconds_ < 0) ? kInfinity : kNegativeInfinity;
	}

	// finally, we're in the case where ticks_ is non-zero,
	// and we can borrow a second's worth of ticks and avoid overflow
	return Duration{~seconds_, kTicksPerSecond - ticks_};
}

constexpr bool Duration::operator==(Duration rhs) const noexcept {
	return seconds_ == rhs.seconds_ && ticks_ == rhs.ticks_;
}

constexpr bool operator!=(Duration lhs, Duration rhs) noexcept {
	return !(lhs == rhs);
}

constexpr bool Duration::operator<(Duration rhs) const noexcept {
	if (seconds_ != rhs.seconds_) {
		return seconds_ < rhs.seconds_;
	}

	if (seconds_ == core::min<i64>) {
		return ticks_ + 1 < rhs.ticks_ + 1;
	}

	return ticks_ < rhs.ticks_;
}

constexpr bool operator>(Duration lhs, Duration rhs) noexcept {
	return rhs < lhs;
}

constexpr bool operator>=(Duration lhs, Duration rhs) noexcept {
	return !(lhs < rhs);
}

constexpr bool operator<=(Duration lhs, Duration rhs) noexcept {
	return !(rhs < lhs);
}

constexpr Duration& Duration::operator+=(Duration rhs) noexcept {
	if (isInf()) {
		return *this;
	}

	if (rhs.isInf()) {
		return *this = rhs;
	}

	auto const origSeconds = seconds_;
	seconds_ += rhs.seconds_;
	if (ticks_ >= kTicksPerSecond - rhs.ticks_) {
		++seconds_;
		ticks_ -= kTicksPerSecond;
	}
	ticks_ += rhs.ticks_;

	if (rhs.seconds_ < 0 ? seconds_ > origSeconds : seconds_ < origSeconds) {
		return *this = rhs.seconds_ < 0 ? kNegativeInfinity : kInfinity;
	}
	return *this;
}

constexpr Duration& Duration::operator-=(Duration rhs) noexcept {
	if (isInf()) {
		return *this;
	}

	if (rhs.isInf()) {
		return *this = (rhs.seconds_ > 0 ? kNegativeInfinity : kInfinity);
	}

	auto const origSeconds = seconds_;
	seconds_ -= rhs.seconds_;
	if (ticks_ < rhs.ticks_) {
		--seconds_;
		ticks_ += kTicksPerSecond;
	}
	ticks_ -= rhs.ticks_;

	if (rhs.seconds_ < 0 ? seconds_ < origSeconds : seconds_ > origSeconds) {
		return *this = rhs.seconds_ >= 0 ? kNegativeInfinity : kInfinity;
	}
	return *this;
}

constexpr Duration operator+(Duration lhs, Duration rhs) noexcept {
	return lhs += rhs;
}

constexpr Duration operator-(Duration lhs, Duration rhs) noexcept {
	return lhs -= rhs;
}

// Factory functions
template <class T,
    RB_REQUIRES_INTEGRAL(T)>
constexpr Duration nanoseconds(T value) noexcept {
	return impl::fromInt64(value, std::nano{});
}

template <class T,
    RB_REQUIRES_INTEGRAL(T)>
constexpr Duration microseconds(T value) noexcept {
	return impl::fromInt64(value, std::micro{});
}

template <class T,
    RB_REQUIRES_INTEGRAL(T)>
constexpr Duration milliseconds(T value) noexcept {
	return impl::fromInt64(value, std::milli{});
}

template <class T,
    RB_REQUIRES_INTEGRAL(T)>
constexpr Duration seconds(T value) noexcept {
	return impl::fromInt64(value, std::ratio<1>{});
}

template <class T,
    RB_REQUIRES_INTEGRAL(T)>
constexpr Duration minutes(T value) noexcept {
	return impl::fromInt64(value, std::ratio<Duration::kSecondsPerMinute>{});
}

template <class T,
    RB_REQUIRES_INTEGRAL(T)>
constexpr Duration hours(T value) noexcept {
	return impl::fromInt64(value, std::ratio<Duration::kSecondsPerHour>{});
}

constexpr Duration Duration::kNanosecond = nanoseconds(1);
constexpr Duration Duration::kMicrosecond = microseconds(1);
constexpr Duration Duration::kMillisecond = milliseconds(1);
constexpr Duration Duration::kSecond = seconds(1);
constexpr Duration Duration::kMinute = minutes(1);
constexpr Duration Duration::kHour = hours(1);
constexpr Duration Duration::kInfinity = {core::max<i64>, kInfTicks};
constexpr Duration Duration::kNegativeInfinity = {core::min<i64>, kInfTicks};

inline namespace literals {

	constexpr Duration operator""_ns(unsigned long long value) noexcept {
		return nanoseconds(value);
	}

	constexpr Duration operator""_us(unsigned long long value) noexcept {
		return microseconds(value);
	}

	constexpr Duration operator""_ms(unsigned long long value) noexcept {
		return milliseconds(value);
	}

	constexpr Duration operator""_s(unsigned long long value) noexcept {
		return seconds(value);
	}

	constexpr Duration operator""_min(unsigned long long value) noexcept {
		return minutes(value);
	}

	constexpr Duration operator""_h(unsigned long long value) noexcept {
		return hours(value);
	}

} // namespace literals

} // namespace rb::time

#undef RB_REQUIRES_FLOAT
#undef RB_REQUIRES_INTEGRAL
