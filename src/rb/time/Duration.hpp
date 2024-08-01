#pragma once

#include <ratio>
#include <vector>

#include <rb/core/features.hpp>
#if RB_ENABLED(DURATION_NAN)
	#include <limits>
#endif

#include <rb/core/Expected.hpp>
#include <rb/core/int128.hpp>
#include <rb/core/Span.hpp>

#define RB_REQUIRES_INTEGRAL(T) RB_REQUIRES(rb::core::isIntegral<T> || rb::core::isEnum<T>)
#define RB_REQUIRES_FLOAT(T) RB_REQUIRES(rb::core::isFloatingPoint<T>)

namespace rb::time {

class Duration;

namespace impl {
	constexpr Duration duration(core::i128 ticks) noexcept;
} // namespace impl

class Duration final {
public:
	using Tick = core::i128;

	static constexpr u32 kTicksPerSecond = 4'000'000'000U;
	static constexpr auto kTicksPerNanosecond = 4U;
	static constexpr auto kSecondsPerMinute = 60U;
	static constexpr auto kSecondsPerHour = 3600U;

	static Duration const kNanosecond;
	static Duration const kMicrosecond;
	static Duration const kMillisecond;
	static Duration const kSecond;
	static Duration const kMinute;
	static Duration const kHour;

#if RB_ENABLED(DURATION_NAN)
	/// Returns the special value "NaN" Duration.
	static constexpr Duration nan() noexcept;
#endif

	/// Returns the special value "positive infinity" Duration.
	static constexpr Duration inf() noexcept;

	/// Returns the maximum finite Duration.
	static constexpr Duration max() noexcept;

	/// Returns the minimum positive finite Duration.
	/// To find the value that has no values less than it, use lowest().
	static constexpr Duration min() noexcept;

	/// Returns the zero Duration.
	static constexpr Duration zero() noexcept;

	/// Returns the lowest finite Duration,
	/// that is, a finite value `x` such that there is no other finite value `y` where `y < x`.
	/// Equals `-max()`.
	static constexpr Duration lowest() noexcept;

	constexpr Duration() noexcept = default;

	constexpr explicit operator bool() const noexcept;

	constexpr bool operator!() const noexcept;

	constexpr Duration operator-() const noexcept;

	constexpr bool operator==(Duration rhs) const noexcept;

	constexpr bool operator!=(Duration rhs) const noexcept;

	constexpr bool operator<(Duration rhs) const noexcept;

	constexpr Duration operator+(Duration rhs) const noexcept;

	constexpr Duration& operator+=(Duration rhs) noexcept;

	constexpr Duration operator-(Duration rhs) const noexcept;

	constexpr Duration& operator-=(Duration rhs) noexcept;

	constexpr Duration operator*(core::i128 value) const noexcept;

	constexpr Duration& operator*=(core::i128 value) noexcept;

	template <class T>
	constexpr auto operator*(T value) const noexcept
	    -> core::EnableIf<core::isFloatingPoint<T>, Duration>;

	constexpr Duration operator/(core::i128 value) const noexcept;

	constexpr Duration& operator/=(core::i128 value) noexcept;

	constexpr core::i128 operator/(Duration rhs) const noexcept;

	constexpr Duration operator%(Duration rhs) const noexcept;

	constexpr int opCmp(Duration rhs) const noexcept;

	constexpr bool isNaN() const noexcept;

	/// Returns true iff `*this` is positive or negative infinity.
	constexpr bool isInf() const noexcept;

	constexpr bool isPositive() const noexcept;
	constexpr bool isZero() const noexcept;
	constexpr bool isNegative() const noexcept;

	template <class T = double>
	constexpr auto div(Duration rhs) const noexcept
	    -> core::EnableIf<core::isFloatingPoint<T>, T>;

private:
	friend constexpr Duration impl::duration(Tick ticks) noexcept;

	constexpr explicit Duration(Tick ticks) noexcept
	    : ticks_(ticks) {
	}

	Tick ticks_;
};

std::ostream& operator<<(std::ostream& os, Duration dur);

namespace impl {

	constexpr Duration duration(Duration::Tick ticks) noexcept {
		return Duration{ticks};
	}

	template <class Ratio>
	constexpr Duration fromInt(Duration::Tick value, Ratio /*unused*/) noexcept {
		static_assert(Duration::kTicksPerSecond % Ratio::den == 0);
		using TicksPerSecond = std::ratio<Duration::kTicksPerSecond>;
		using TicksPerUnit = std::ratio_multiply<TicksPerSecond, Ratio>;
		return duration(value * TicksPerUnit::num / TicksPerUnit::den);
	}

	template <class T, class U>
	constexpr Duration inf(T const& lhs, U const& rhs) noexcept {
		return (lhs < 0) != (rhs < 0) ? -Duration::inf() : Duration::inf();
	}

} // namespace impl

///
RB_CONST constexpr Duration abs(Duration dur) noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (RB_UNLIKELY(dur.isNaN())) {
		return Duration::nan();
	}
#endif
	return dur.isPositive() ? dur : -dur;
}

/// Truncates the @p dur (toward zero) to a multiple of a non-zero @p unit.
RB_CONST constexpr Duration trunc(Duration dur, Duration unit = Duration::kSecond) noexcept {
	return dur - (dur % unit);
}

#if RB_ENABLED(DURATION_NAN)
constexpr Duration Duration::nan() noexcept {
	return Duration{Tick::min()};
}
#endif

constexpr Duration Duration::inf() noexcept {
	return Duration{Tick::max()};
}

constexpr Duration Duration::max() noexcept {
	return Duration{Tick::max() - 1};
}

constexpr Duration Duration::lowest() noexcept {
	return -max();
}

constexpr Duration Duration::min() noexcept {
	return Duration{1};
}

constexpr Duration Duration::zero() noexcept {
	return {};
}

constexpr Duration::operator bool() const noexcept {
	return static_cast<bool>(ticks_);
}

constexpr bool Duration::operator!() const noexcept {
	return !ticks_;
}

constexpr Duration Duration::operator-() const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()) {
		return nan();
	}
#endif
	return Duration{-ticks_};
}

constexpr bool Duration::operator==(Duration rhs) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN() || rhs.isNaN()) {
		return false;
	}
#endif
	return ticks_ == rhs.ticks_;
}

constexpr bool Duration::operator!=(Duration rhs) const noexcept {
	return ticks_ != rhs.ticks_;
}

constexpr bool Duration::operator<(Duration rhs) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN() || rhs.isNaN()) {
		return false;
	}
#endif
	return ticks_ < rhs.ticks_;
}

constexpr bool operator>(Duration lhs, Duration rhs) noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (lhs.isNaN() || rhs.isNaN()) {
		return false;
	}
#endif
	return rhs < lhs;
}

constexpr bool operator>=(Duration lhs, Duration rhs) noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (lhs.isNaN() || rhs.isNaN()) {
		return false;
	}
#endif
	return !(lhs < rhs);
}

constexpr bool operator<=(Duration lhs, Duration rhs) noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (lhs.isNaN() || rhs.isNaN()) {
		return false;
	}
#endif
	return !(rhs < lhs);
}

constexpr Duration Duration::operator+(Duration rhs) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()
	    || rhs.isNaN()
	    || *this == Duration::inf() && rhs == -Duration::inf()
	    || *this == -Duration::inf() && rhs == Duration::inf()) //
	{
		return nan();
	}
#endif

	if (isInf()) {
		return *this;
	}

	if (rhs.isInf()) {
		return rhs;
	}

	auto const cmp = rhs.ticks_.opCmp(0);
	if (cmp == 0) {
		return *this;
	}

	if (cmp > 0 && ticks_ >= Tick::max() - rhs.ticks_) {
		return inf();
	}

	if (cmp < 0 && ticks_ <= -Tick::max() - rhs.ticks_) {
		return -inf();
	}

	return Duration{ticks_ + rhs.ticks_};
}

constexpr Duration& Duration::operator+=(Duration rhs) noexcept {
	return *this = *this + rhs;
}

constexpr Duration Duration::operator-(Duration rhs) const noexcept {
	return *this + (-rhs);
}

constexpr Duration& Duration::operator-=(Duration rhs) noexcept {
	return *this = *this - rhs;
}

constexpr Duration Duration::operator*(core::i128 value) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN() || isInf() && !value) {
		return nan();
	}
#endif

	if (isZero() || !value) {
		return zero();
	}

	Duration inf = impl::inf(ticks_, value);
	if (isInf() || value == core::i128::min()) {
		return inf;
	}

	// abs() now defined for both *this and value
	auto const ticks = abs(ticks_);
	auto const val = abs(value);
	if (ticks > core::u128{max().ticks_} / val) {
		return inf;
	}

	return Duration{ticks_ * value};
}

constexpr Duration operator*(core::i128 value, Duration dur) noexcept {
	return dur * value;
}

constexpr Duration& Duration::operator*=(core::i128 value) noexcept {
	return *this = *this * value;
}

template <class T>
constexpr auto Duration::operator*(T value) const noexcept
    -> core::EnableIf<core::isFloatingPoint<T>, Duration> {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()
	    || std::isnan(value)
	    || isZero() && std::isinf(value)
	    || isInf() && !value) //
	{
		return nan();
	}
#endif

	if (isZero() || !value) {
		return Duration::zero();
	}

	bool const isNeg = isNegative() != (value < 0);
	if (isInf() || std::isinf(value)) {
		return isNeg ? -Duration::inf() : Duration::inf();
	}

	auto const ticks = abs(ticks_);
	auto const f = std::abs(value);
	if (f > static_cast<T>(core::u128{max().ticks_} / ticks)) {
		return isNeg ? -Duration::inf() : Duration::inf();
	}

	Duration const result{Tick{ticks * f}};
	return isNeg ? -result : result;
}

constexpr Duration Duration::operator/(core::i128 value) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN() || isZero() && !value) {
		return nan();
	}
#endif

	if (isInf() || !value) {
		return impl::inf(ticks_, value);
	}
	return Duration{ticks_ / value};
}

constexpr Duration& Duration::operator/=(core::i128 value) noexcept {
	return *this = *this / value;
}

constexpr core::i128 Duration::operator/(Duration rhs) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()
	    || rhs.isNaN()
	    || isZero() && rhs.isZero()
	    || isInf() && rhs.isInf()) {
		return Tick::min(); // repr of NaN
	}
#endif

	if (isInf() || !rhs) {
		bool const isNeg = (ticks_ < 0) != (rhs.ticks_ < 0);
		return isNeg ? Tick::min() : Tick::max();
	}
	return ticks_ / rhs.ticks_;
}

constexpr Duration Duration::operator%(Duration rhs) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN() || rhs.isNaN() || rhs.isZero()) {
		return nan();
	}
#endif

	if (isInf() || !rhs) {
		return impl::inf(ticks_, rhs.ticks_);
	}

	if (rhs.isInf()) {
		return *this;
	}

	return Duration{ticks_ % rhs.ticks_};
}

constexpr int Duration::opCmp(Duration rhs) const noexcept {
	// TODO add NaN case
	return ticks_.opCmp(rhs.ticks_);
}

constexpr bool Duration::isNaN() const noexcept {
#if RB_ENABLED(DURATION_NAN)
	return ticks_ == Tick::min();
#else
	return false;
#endif
}

constexpr bool Duration::isInf() const noexcept {
	return *this == inf() || *this == -inf();
}

constexpr bool Duration::isPositive() const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()) {
		return false;
	}
#endif
	return ticks_ > 0;
}

constexpr bool Duration::isZero() const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()) {
		return false;
	}
#endif
	return !ticks_;
}

constexpr bool Duration::isNegative() const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()) {
		return false;
	}
#endif
	return ticks_ < 0;
}

template <class T>
constexpr auto Duration::div(Duration rhs) const noexcept
    -> core::EnableIf<core::isFloatingPoint<T>, T> {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()
	    || rhs.isNaN()
	    || isZero() && rhs.isZero()
	    || isInf() && rhs.isInf()) //
	{
		return std::numeric_limits<T>::quiet_NaN();
	}
#endif

	if (isInf() || rhs.isZero()) {
		bool const isNeg = isNegative() != rhs.isNegative();
		constexpr T inf = core::inf<T>;
		return isNeg ? -inf : inf;
	}

	if (rhs.isInf()) {
		return 0;
	}

	return static_cast<T>(ticks_) / static_cast<T>(rhs.ticks_);
}

// math-like functions

/// Floors the @p dur using the passed duration @p unit to its largest value not greater than the @p dur.
RB_CONST constexpr Duration floor(Duration dur, Duration unit = Duration::kSecond) noexcept {
	auto const t = trunc(dur, unit);
	return t <= dur ? t : t - abs(unit);
}

/// Returns the ceiling of the @p dur using the passed duration @p unit to its smallest value not less than the @p dur.
RB_CONST constexpr Duration ceil(Duration dur, Duration unit = Duration::kSecond) noexcept {
	auto const t = trunc(dur, unit);
	return t >= dur ? t : t + abs(unit);
}

// factory functions

///
template <class T,
    RB_REQUIRES_INTEGRAL(T)>
constexpr Duration nanoseconds(T value) noexcept {
	return impl::fromInt(value, std::nano{});
}

///
template <class T,
    RB_REQUIRES_INTEGRAL(T)>
constexpr Duration microseconds(T value) noexcept {
	return impl::fromInt(value, std::micro{});
}

///
template <class T,
    RB_REQUIRES_INTEGRAL(T)>
constexpr Duration milliseconds(T value) noexcept {
	return impl::fromInt(value, std::milli{});
}

///
template <class T,
    RB_REQUIRES_INTEGRAL(T)>
constexpr Duration seconds(T value) noexcept {
	return impl::fromInt(value, std::ratio<1>{});
}

///
template <class T,
    RB_REQUIRES_INTEGRAL(T)>
constexpr Duration minutes(T value) noexcept {
	return impl::fromInt(value, std::ratio<Duration::kSecondsPerMinute>{});
}

///
template <class T,
    RB_REQUIRES_INTEGRAL(T)>
constexpr Duration hours(T value) noexcept {
	return impl::fromInt(value, std::ratio<Duration::kSecondsPerHour>{});
}

// FIXME cannot be constexpr due to MSVC
inline const Duration Duration::kNanosecond = nanoseconds(1);
inline const Duration Duration::kMicrosecond = microseconds(1);
inline const Duration Duration::kMillisecond = milliseconds(1);
inline const Duration Duration::kSecond = seconds(1);
inline const Duration Duration::kMinute = minutes(1);
inline const Duration Duration::kHour = hours(1);

inline namespace literals {

	///
	constexpr Duration operator""_ns(unsigned long long value) noexcept {
		return nanoseconds(value);
	}

	///
	constexpr Duration operator""_us(unsigned long long value) noexcept {
		return microseconds(value);
	}

	///
	constexpr Duration operator""_ms(unsigned long long value) noexcept {
		return milliseconds(value);
	}

	///
	constexpr Duration operator""_s(unsigned long long value) noexcept {
		return seconds(value);
	}

	///
	constexpr Duration operator""_min(unsigned long long value) noexcept {
		return minutes(value);
	}

	///
	constexpr Duration operator""_h(unsigned long long value) noexcept {
		return hours(value);
	}

} // namespace literals

// units

struct Unit {
	Duration duration;
	std::string symbol;
};

template <class T>
struct Quantity {
	T count;
	Unit unit;
};

template <class T>
Quantity(T, Unit) -> Quantity<core::RemoveCvRef<T>>;

template <class T>
using QuantityList = std::vector<Quantity<T>>;

template <class T>
struct SplitResult {
	QuantityList<T> quantities;
	Duration rem;
	bool isPositive = true;
	bool isInf = false;
};

enum class SplitError {
	kOk [[maybe_unused]], ///< Ok
	kNoUnits, ///< `Unit` list is empty
	kZeroUnit, ///< There are `Unit`s with zero `Duration` or empty `symbol`
	kNotUnique, ///< There are `Unit`s with the same `Duration`
#if RB_ENABLED(DURATION_NAN)
	kNaN, ///< There is NaN among `Unit`s or argument
#endif
};

template <class T>
using SplitExpected = core::Expected<SplitResult<T>, SplitError>;

SplitExpected<core::u128> split(Duration dur, core::Span<Unit const> units);

} // namespace rb::time

#undef RB_REQUIRES_INTEGRAL
#undef RB_REQUIRES_FLOAT
