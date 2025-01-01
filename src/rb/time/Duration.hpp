#pragma once

#include <chrono>

#include <rb/core/int128.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/enums.hpp>
#include <rb/core/traits/IsArithmetic.hpp>
#include <rb/time/I64.hpp>

#define RB_REQUIRES_INTEGRAL(T) RB_REQUIRES(rb::core::isIntegral<T> || rb::core::isEnum<T>)
#define RB_REQUIRES_FLOAT(T) RB_REQUIRES(rb::core::isFloatingPoint<T>)

namespace rb::time {

class Duration;

namespace impl {
	constexpr Duration duration(i64 seconds, i64 ticks = 0) noexcept;
} // namespace impl

class Duration final {
public:
	static constexpr u32 kTicksPerSecond = 4'000'000'000U;
	static constexpr u32 kTicksPerNanosecond = 4U;

	static constexpr u32 kNanosecondsPerSecond = 1'000'000'000;
	static constexpr auto kSecondsPerMinute = 60U;
	static constexpr auto kSecondsPerHour = 3600U;

	static Duration const kNanosecond;
	static Duration const kMicrosecond;
	static Duration const kMillisecond;
	static Duration const kSecond;
	static Duration const kMinute;
	static Duration const kHour;

	static Duration const kInfinity;
	static Duration const kNegativeInfinity;
	static Duration const kNaN;

	/// Returns the special value "positive infinity" Duration.
	static constexpr Duration inf() noexcept;

	/// Returns the special value "NaN" Duration.
	static constexpr Duration nan() noexcept;

	/// Returns the maximum finite Duration.
	static constexpr Duration max() noexcept;

	/// Returns the minimum positive finite Duration.
	/// To find the value that has no values less than it, use lowest().
	static constexpr Duration min() noexcept;

	/// Returns the zero Duration.
	static constexpr Duration zero() noexcept;

	/// Returns the lowest finite Duration,
	/// that is, a finite value `x` such that there is no other finite value `y` where `y < x`.
	static constexpr Duration lowest() noexcept;

	constexpr Duration() noexcept = default;

	constexpr explicit operator bool() const noexcept;

	constexpr Duration operator-() const noexcept;

	constexpr bool operator==(Duration rhs) const noexcept;

	constexpr bool operator<(Duration rhs) const noexcept;

	constexpr Duration& operator+=(Duration rhs) noexcept;
	constexpr Duration& operator-=(Duration rhs) noexcept;

	constexpr Duration operator*(i64 value) const noexcept;
	constexpr Duration operator/(i64 value) const noexcept;

	/// Returns true iff `*this` is positive or negative infinity.
	constexpr bool isInf() const noexcept;

	/// Returns true iff `*this` is NaN.
	constexpr bool isNaN() const noexcept;

	constexpr bool isPositive() const noexcept;
	constexpr bool isZero() const noexcept;
	constexpr bool isNegative() const noexcept;

private:
	friend std::ostream& operator<<(std::ostream& os, Duration dur);
	friend constexpr Duration impl::duration(i64 seconds, i64 ticks) noexcept;

	static constexpr u32 kInfTicks = ~0U;
	static constexpr u32 kNaNTicks = kNanosecondsPerSecond;

	constexpr Duration(i64 seconds, u32 ticks) noexcept
	    : seconds_(seconds)
	    , ticks_(ticks) {
	}

	impl::I64 seconds_;
	u32 ticks_ = 0;
};

std::ostream& operator<<(std::ostream& os, Duration dur);

} // namespace rb::time

namespace rb::time::impl {

// ReSharper disable once CppDFAUnreachableFunctionCall
constexpr Duration duration(i64 seconds, i64 ticks) noexcept {
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
	return duration(seconds, ticks);
}

constexpr Duration fromInt64(i64 value, std::ratio<Duration::kSecondsPerMinute> /*unused*/) {
	if (value <= core::max<i64> / Duration::kSecondsPerMinute && //
	    value >= core::min<i64> / Duration::kSecondsPerMinute) {
		return duration(value * Duration::kSecondsPerMinute);
	}
	return value > 0 ? Duration::kInfinity : Duration::kNegativeInfinity;
}

constexpr Duration fromInt64(i64 value, std::ratio<Duration::kSecondsPerHour> /*unused*/) {
	if (value <= core::max<i64> / Duration::kSecondsPerHour && //
	    value >= core::min<i64> / Duration::kSecondsPerHour) {
		return duration(value * Duration::kSecondsPerHour);
	}
	return value > 0 ? Duration::kInfinity : Duration::kNegativeInfinity;
}

} // namespace rb::time::impl

namespace rb::time {

constexpr Duration abs(Duration dur) noexcept {
	return dur.isNegative() ? -dur : dur;
}

// Duration methods
constexpr Duration Duration::inf() noexcept {
	return kInfinity;
}

constexpr Duration Duration::nan() noexcept {
	return kNaN;
}

constexpr Duration Duration::max() noexcept {
	return {core::max<i64>, kTicksPerSecond - 1};
}

constexpr Duration Duration::min() noexcept {
	return {0, 1};
}

constexpr Duration Duration::zero() noexcept {
	return {};
}

constexpr Duration Duration::lowest() noexcept {
	return {core::min<i64>, 0};
}

constexpr bool Duration::isInf() const noexcept {
	return ticks_ == kInfTicks;
}

constexpr bool Duration::isNaN() const noexcept {
	return ticks_ == kNaNTicks;
}

constexpr bool Duration::isPositive() const noexcept {
	if (RB_UNLIKELY(isNaN())) {
		return false;
	}
	return seconds_ ? (seconds_ > 0) : (ticks_ > 0);
}

constexpr bool Duration::isZero() const noexcept {
	if (RB_UNLIKELY(isNaN())) {
		return false;
	}
	return !static_cast<bool>(*this);
}

constexpr bool Duration::isNegative() const noexcept {
	if (RB_UNLIKELY(isNaN())) {
		return false;
	}
	return seconds_ < 0;
}

constexpr Duration::operator bool() const noexcept {
	return seconds_ || ticks_;
}

constexpr Duration Duration::operator-() const noexcept {
	if (RB_UNLIKELY(isNaN())) {
		return kNaN;
	}

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
	if (RB_UNLIKELY(isNaN() || rhs.isNaN())) {
		return false;
	}

	return seconds_ == rhs.seconds_ && ticks_ == rhs.ticks_;
}

constexpr bool operator!=(Duration lhs, Duration rhs) noexcept {
	return !(lhs == rhs);
}

constexpr bool Duration::operator<(Duration rhs) const noexcept {
	if (RB_UNLIKELY(isNaN() || rhs.isNaN())) {
		return false;
	}

	if (seconds_ != rhs.seconds_) {
		return seconds_ < rhs.seconds_;
	}

	if (seconds_ == core::min<i64>) {
		return ticks_ + 1 < rhs.ticks_ + 1;
	}

	return ticks_ < rhs.ticks_;
}

constexpr bool operator>(Duration lhs, Duration rhs) noexcept {
	if (RB_UNLIKELY(lhs.isNaN() || rhs.isNaN())) {
		return false;
	}
	return rhs < lhs;
}

constexpr bool operator>=(Duration lhs, Duration rhs) noexcept {
	if (RB_UNLIKELY(lhs.isNaN() || rhs.isNaN())) {
		return false;
	}
	return !(lhs < rhs);
}

constexpr bool operator<=(Duration lhs, Duration rhs) noexcept {
	if (RB_UNLIKELY(lhs.isNaN() || rhs.isNaN())) {
		return false;
	}
	// ReSharper disable once CppRedundantComplexityInComparison
	return !(rhs < lhs);
}

constexpr Duration& Duration::operator+=(Duration rhs) noexcept {
	if (RB_UNLIKELY(
	        isNaN()
	        || rhs.isNaN()
	        || *this == kInfinity && rhs == kNegativeInfinity
	        || *this == kNegativeInfinity && rhs == kInfinity)) {
		return *this = kNaN;
	}

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
	if (RB_UNLIKELY(
	        isNaN()
	        || rhs.isNaN()
	        || *this == kInfinity && rhs == kInfinity
	        || *this == kNegativeInfinity && rhs == kNegativeInfinity)) {
		return *this = kNaN;
	}

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

constexpr Duration Duration::operator*(i64 value) const noexcept {
	if (RB_UNLIKELY(isNaN())) {
		return kNaN;
	}

	bool const isNeg = isNegative() != (value < 0);
	Duration const inf = isNeg ? kNegativeInfinity : kInfinity;
	if (isInf()) {
		if (!value) {
			return kNaN;
		}
		return inf;
	}

	if (!value) {
		return {};
	}

	Duration const dur = abs(*this);
	u64 const v = value < 0 ? -value : value;
	core::u128 seconds = static_cast<i64>(dur.seconds_);
	seconds *= v;
	core::u128 ticks = dur.ticks_;
	ticks *= v;
	core::u128 const addSecs = ticks / kTicksPerSecond;
	ticks %= kTicksPerSecond;
	if (seconds > core::u128::max() - addSecs) {
		return inf;
	}

	seconds += addSecs;
	if (seconds > core::max<i64>) {
		return inf;
	}

	Duration const ans{static_cast<i64>(seconds), static_cast<u32>(ticks)};
	return isNeg ? -ans : ans;
}

constexpr Duration Duration::operator/(i64 value) const noexcept {
	if (isInf()) {
		return *this;
	}

	return *this;
}

constexpr Duration operator+(Duration lhs, Duration rhs) noexcept {
	return lhs += rhs;
}

constexpr Duration operator-(Duration lhs, Duration rhs) noexcept {
	return lhs -= rhs;
}

constexpr Duration operator*(i64 lhs, Duration rhs) noexcept {
	return rhs * lhs;
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
constexpr Duration Duration::kNaN = {0, kNaNTicks};

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
