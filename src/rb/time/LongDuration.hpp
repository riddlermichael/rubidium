#pragma once

#include <ratio>
#include <string_view>
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

class LongDuration final {
public:
	using Tick = core::i128;

	static constexpr u32 kTicksPerSecond = 4'000'000'000U;
	static constexpr u32 kTicksPerNanosecond = 4U;

	static constexpr u32 kNanosecondsPerSecond = 1'000'000'000;
	static constexpr auto kSecondsPerMinute = 60U;
	static constexpr auto kSecondsPerHour = 3600U;

	static LongDuration const kNanosecond;
	static LongDuration const kMicrosecond;
	static LongDuration const kMillisecond;
	static LongDuration const kSecond;
	static LongDuration const kMinute;
	static LongDuration const kHour;

	static LongDuration const kInfinity;
	static LongDuration const kNegativeInfinity;
	static LongDuration const kNaN;

#if RB_ENABLED(DURATION_NAN)
	/// Returns the special value "NaN" LongDuration.
	static constexpr LongDuration nan() noexcept;
#endif

	/// Returns the special value "positive infinity" LongDuration.
	static constexpr LongDuration inf() noexcept;

	/// Returns the maximum finite LongDuration.
	static constexpr LongDuration max() noexcept;

	/// Returns the minimum positive finite LongDuration.
	/// To find the value that has no values less than it, use lowest().
	static constexpr LongDuration min() noexcept;

	/// Returns the zero LongDuration.
	static constexpr LongDuration zero() noexcept;

	/// Returns the lowest finite LongDuration,
	/// that is, a finite value `x` such that there is no other finite value `y` where `y < x`.
	/// Equals `-max()`.
	static constexpr LongDuration lowest() noexcept;

	template <class T,
	    RB_REQUIRES_INTEGRAL(T)>
	static constexpr LongDuration nanoseconds(T value) noexcept {
		return fromInt(value, std::nano{});
	}

	template <class T,
	    RB_REQUIRES_INTEGRAL(T)>
	static constexpr LongDuration microseconds(T value) noexcept {
		return fromInt(value, std::micro{});
	}

	template <class T,
	    RB_REQUIRES_INTEGRAL(T)>
	static constexpr LongDuration milliseconds(T value) noexcept {
		return fromInt(value, std::milli{});
	}

	template <class T,
	    RB_REQUIRES_INTEGRAL(T)>
	static constexpr LongDuration seconds(T value) noexcept {
		return fromInt(value, std::ratio<1>{});
	}

	template <class T,
	    RB_REQUIRES_INTEGRAL(T)>
	static constexpr LongDuration minutes(T value) noexcept {
		return fromInt(value, std::ratio<kSecondsPerMinute>{});
	}

	template <class T,
	    RB_REQUIRES_INTEGRAL(T)>
	static constexpr LongDuration hours(T value) noexcept {
		return fromInt(value, std::ratio<kSecondsPerHour>{});
	}

	constexpr LongDuration() noexcept = default;

	constexpr explicit operator bool() const noexcept;

	constexpr bool operator!() const noexcept;

	constexpr LongDuration operator+() const noexcept;
	constexpr LongDuration operator-() const noexcept;

	constexpr bool operator==(LongDuration rhs) const noexcept;

	constexpr bool operator!=(LongDuration rhs) const noexcept;

	constexpr bool operator<(LongDuration rhs) const noexcept;

	constexpr LongDuration operator+(LongDuration rhs) const noexcept;

	constexpr LongDuration& operator+=(LongDuration rhs) noexcept;

	constexpr LongDuration operator-(LongDuration rhs) const noexcept;

	constexpr LongDuration& operator-=(LongDuration rhs) noexcept;

	constexpr LongDuration operator*(core::i128 value) const noexcept;

	constexpr LongDuration& operator*=(core::i128 value) noexcept;

	template <class T>
	constexpr auto operator*(T value) const noexcept
	    -> core::EnableIf<core::isFloatingPoint<T>, LongDuration>;

	constexpr LongDuration operator/(core::i128 value) const noexcept;

	constexpr LongDuration& operator/=(core::i128 value) noexcept;

	constexpr core::i128 operator/(LongDuration rhs) const noexcept;

	constexpr LongDuration operator%(LongDuration rhs) const noexcept;

	constexpr int opCmp(LongDuration rhs) const noexcept;

	constexpr bool isNaN() const noexcept;

	/// Returns true iff `*this` is positive or negative infinity.
	constexpr bool isInf() const noexcept;

	constexpr bool isPositive() const noexcept;
	constexpr bool isZero() const noexcept;
	constexpr bool isNegative() const noexcept;

	template <class T = double>
	constexpr auto div(LongDuration rhs) const noexcept
	    -> core::EnableIf<core::isFloatingPoint<T>, T>;

private:
	static constexpr Tick kInfTicks = Tick::max();
	static constexpr Tick kNaNTicks = Tick::min();

	template <class Ratio>
	static constexpr LongDuration fromInt(Tick value, Ratio /*unused*/) noexcept {
		static_assert(kTicksPerSecond % Ratio::den == 0);
		using TicksPerSecond = std::ratio<kTicksPerSecond>;
		using TicksPerUnit = std::ratio_multiply<TicksPerSecond, Ratio>;
		return LongDuration{value * TicksPerUnit::num / TicksPerUnit::den};
	}

	constexpr explicit LongDuration(Tick ticks) noexcept
	    : ticks_(ticks) {
	}

	Tick ticks_;
};

std::ostream& operator<<(std::ostream& os, LongDuration dur);

namespace impl {

	template <class T, class U>
	constexpr LongDuration inf(T const& lhs, U const& rhs) noexcept {
		return (lhs < 0) != (rhs < 0) ? -LongDuration::kInfinity : LongDuration::kInfinity;
	}

} // namespace impl

///
RB_CONST constexpr LongDuration abs(LongDuration dur) noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (RB_UNLIKELY(dur.isNaN())) {
		return LongDuration::kNaN;
	}
#endif
	return dur.isPositive() ? dur : -dur;
}

/// Truncates the @p dur (toward zero) to a multiple of a non-zero @p unit.
RB_CONST constexpr LongDuration trunc(LongDuration dur, LongDuration unit = LongDuration::kSecond) noexcept {
	return dur - (dur % unit);
}

#if RB_ENABLED(DURATION_NAN)
constexpr LongDuration LongDuration::nan() noexcept {
	return LongDuration{kNaNTicks};
}
#endif

constexpr LongDuration LongDuration::inf() noexcept {
	return LongDuration{kInfTicks};
}

constexpr LongDuration LongDuration::max() noexcept {
	return LongDuration{kInfTicks - 1};
}

constexpr LongDuration LongDuration::lowest() noexcept {
	return -max();
}

constexpr LongDuration LongDuration::min() noexcept {
	return LongDuration{1};
}

constexpr LongDuration LongDuration::zero() noexcept {
	return {};
}

constexpr LongDuration::operator bool() const noexcept {
	return static_cast<bool>(ticks_);
}

constexpr bool LongDuration::operator!() const noexcept {
	return !ticks_;
}

constexpr LongDuration LongDuration::operator+() const noexcept {
	return *this;
}

constexpr LongDuration LongDuration::operator-() const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()) {
		return kNaN;
	}
#endif
	return LongDuration{-ticks_};
}

constexpr bool LongDuration::operator==(LongDuration rhs) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN() || rhs.isNaN()) {
		return false;
	}
#endif
	return ticks_ == rhs.ticks_;
}

constexpr bool LongDuration::operator!=(LongDuration rhs) const noexcept {
	return ticks_ != rhs.ticks_;
}

constexpr bool LongDuration::operator<(LongDuration rhs) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN() || rhs.isNaN()) {
		return false;
	}
#endif
	return ticks_ < rhs.ticks_;
}

constexpr bool operator>(LongDuration lhs, LongDuration rhs) noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (lhs.isNaN() || rhs.isNaN()) {
		return false;
	}
#endif
	return rhs < lhs;
}

constexpr bool operator>=(LongDuration lhs, LongDuration rhs) noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (lhs.isNaN() || rhs.isNaN()) {
		return false;
	}
#endif
	return !(lhs < rhs);
}

constexpr bool operator<=(LongDuration lhs, LongDuration rhs) noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (lhs.isNaN() || rhs.isNaN()) {
		return false;
	}
#endif
	// ReSharper disable once CppRedundantComplexityInComparison
	return !(rhs < lhs);
}

constexpr LongDuration LongDuration::operator+(LongDuration rhs) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()
	    || rhs.isNaN()
	    || *this == kInfinity && rhs == -kInfinity
	    || *this == -kInfinity && rhs == kInfinity) //
	{
		return kNaN;
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

	if (cmp > 0 && ticks_ >= kInfTicks - rhs.ticks_) {
		return kInfinity;
	}

	if (cmp < 0 && ticks_ <= -kInfTicks - rhs.ticks_) {
		return kNegativeInfinity;
	}

	return LongDuration{ticks_ + rhs.ticks_};
}

constexpr LongDuration& LongDuration::operator+=(LongDuration rhs) noexcept {
	return *this = *this + rhs;
}

constexpr LongDuration LongDuration::operator-(LongDuration rhs) const noexcept {
	return *this + (-rhs);
}

constexpr LongDuration& LongDuration::operator-=(LongDuration rhs) noexcept {
	return *this = *this - rhs;
}

constexpr LongDuration LongDuration::operator*(core::i128 value) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN() || isInf() && !value) {
		return kNaN;
	}
#endif

	if (isZero() || !value) {
		return zero();
	}

	LongDuration const inf = impl::inf(ticks_, value);
	if (isInf() || value == kNaNTicks) { // kNaNTicks < -kInfTicks
		return inf;
	}

	// abs() now defined for both *this and value
	auto const ticks = abs(ticks_);
	auto const val = abs(value);
	if (ticks > core::u128{max().ticks_} / val) {
		return inf;
	}

	return LongDuration{ticks_ * value};
}

constexpr LongDuration operator*(core::i128 value, LongDuration dur) noexcept {
	return dur * value;
}

constexpr LongDuration& LongDuration::operator*=(core::i128 value) noexcept {
	return *this = *this * value;
}

template <class T>
constexpr auto LongDuration::operator*(T value) const noexcept
    -> core::EnableIf<core::isFloatingPoint<T>, LongDuration> {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()
	    || std::isnan(value)
	    || isZero() && std::isinf(value)
	    || isInf() && !value) //
	{
		return kNaN;
	}
#endif

	if (isZero() || !value) {
		return zero();
	}

	bool const isNeg = isNegative() != (value < 0);
	if (isInf() || std::isinf(value)) {
		return isNeg ? kNegativeInfinity : kInfinity;
	}

	auto const ticks = abs(ticks_);
	auto const f = std::abs(value);
	// FIXME
	if (f > static_cast<T>(core::u128{max().ticks_} / ticks)) {
		return isNeg ? kNegativeInfinity : kInfinity;
	}

	LongDuration const result{Tick{ticks * f}};
	return isNeg ? -result : result;
}

constexpr LongDuration LongDuration::operator/(core::i128 value) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN() || isZero() && !value) {
		return kNaN;
	}
#endif

	if (isInf() || !value) {
		return impl::inf(ticks_, value);
	}
	return LongDuration{ticks_ / value};
}

constexpr LongDuration& LongDuration::operator/=(core::i128 value) noexcept {
	return *this = *this / value;
}

constexpr core::i128 LongDuration::operator/(LongDuration rhs) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()
	    || rhs.isNaN()
	    || isZero() && rhs.isZero()
	    || isInf() && rhs.isInf()) {
		return kNaNTicks;
	}
#endif

	if (isInf() || !rhs) {
		bool const isNeg = (ticks_ < 0) != (rhs.ticks_ < 0);
		return isNeg ? kNaNTicks : kInfTicks;
	}
	return ticks_ / rhs.ticks_;
}

constexpr LongDuration LongDuration::operator%(LongDuration rhs) const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN() || rhs.isNaN() || rhs.isZero()) {
		return kNaN;
	}
#endif

	if (isInf() || !rhs) {
		return impl::inf(ticks_, rhs.ticks_);
	}

	if (rhs.isInf()) {
		return *this;
	}

	return LongDuration{ticks_ % rhs.ticks_};
}

constexpr int LongDuration::opCmp(LongDuration rhs) const noexcept {
	// TODO add NaN case
	return ticks_.opCmp(rhs.ticks_);
}

constexpr bool LongDuration::isNaN() const noexcept {
#if RB_ENABLED(DURATION_NAN)
	return ticks_ == kNaNTicks;
#else
	return false;
#endif
}

constexpr bool LongDuration::isInf() const noexcept {
	return *this == inf() || *this == kNegativeInfinity;
}

constexpr bool LongDuration::isPositive() const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()) {
		return false;
	}
#endif
	return ticks_ > 0;
}

constexpr bool LongDuration::isZero() const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()) {
		return false;
	}
#endif
	return !ticks_;
}

constexpr bool LongDuration::isNegative() const noexcept {
#if RB_ENABLED(DURATION_NAN)
	if (isNaN()) {
		return false;
	}
#endif
	return ticks_ < 0;
}

template <class T>
constexpr auto LongDuration::div(LongDuration rhs) const noexcept
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
RB_CONST constexpr LongDuration floor(LongDuration dur, LongDuration unit = LongDuration::kSecond) noexcept {
	auto const t = trunc(dur, unit);
	return t <= dur ? t : t - abs(unit);
}

/// Returns the ceiling of the @p dur using the passed duration @p unit to its smallest value not less than the @p dur.
RB_CONST constexpr LongDuration ceil(LongDuration dur, LongDuration unit = LongDuration::kSecond) noexcept {
	auto const t = trunc(dur, unit);
	return t >= dur ? t : t + abs(unit);
}

// FIXME cannot be constexpr due to MSVC
inline LongDuration const LongDuration::kNanosecond = nanoseconds(1);
inline LongDuration const LongDuration::kMicrosecond = microseconds(1);
inline LongDuration const LongDuration::kMillisecond = milliseconds(1);
inline LongDuration const LongDuration::kSecond = seconds(1);
inline LongDuration const LongDuration::kMinute = minutes(1);
inline LongDuration const LongDuration::kHour = hours(1);

inline LongDuration const LongDuration::kInfinity = inf();
inline LongDuration const LongDuration::kNegativeInfinity = -inf();
#if RB_ENABLED(DURATION_NAN)
inline LongDuration const LongDuration::kNaN = nan();
#endif

// units

struct Unit {
	LongDuration duration;
	std::string_view symbol;
};

template <class T>
struct Quantity {
	T count;
	Unit unit;
};

template <class T>
Quantity(T, Unit) -> Quantity<core::RemoveCvRef<T>>;

template <class T>
using QuantityList = std::vector<Quantity<T>>; // TODO use containers::Vector

template <class T>
struct SplitResult {
	QuantityList<T> quantities;
	LongDuration rem;
	bool isPositive = true;
	bool isInf = false;
};

enum class SplitError {
	kOk [[maybe_unused]], ///< Ok
	kNoUnits, ///< `Unit` list is empty
	kZeroUnit, ///< There are `Unit`s with zero `LongDuration` or empty `symbol`
	kNotUnique, ///< There are `Unit`s with the same `LongDuration`
#if RB_ENABLED(DURATION_NAN)
	kNaN, ///< There is NaN among `Unit`s or argument
#endif
};

template <class T>
using SplitExpected = core::Expected<SplitResult<T>, SplitError>;

SplitExpected<core::u128> split(LongDuration dur, core::Span<Unit const> units);

} // namespace rb::time

#undef RB_REQUIRES_INTEGRAL
#undef RB_REQUIRES_FLOAT
