#pragma once

#include <cmath>
#include <csignal> // raise
#include <iomanip>
#include <limits>
#include <sstream>

#include <rb/core/assert.hpp>
#include <rb/core/bits.hpp>
#include <rb/core/error/RangeError.hpp>
#include <rb/core/limits.hpp>
#include <rb/core/quorem.hpp>
#include <rb/core/traits/IsSigned.hpp>
#include <rb/core/traits/IsUnsigned.hpp>

template <bool>
class Int128;

namespace rb::core {

[[noreturn]] inline void raiseFpe() {
	(void) std::raise(SIGFPE);
	RB_UNREACHABLE_ASSERT();
}

template <bool kUnsigned>
class alignas(sizeof(usize) * 2) Int128 final {
	template <bool>
	friend class Int128; // type alias `Twin` cannot be used in friend declaration

public:
	using Hi = Conditional<kUnsigned, u64, i64>;
	using Twin = Int128<!kUnsigned>;

	static constexpr Int128 min() noexcept {
		return {core::min<Hi>, 0};
	}

	static constexpr Int128 max() noexcept {
		return {core::max<Hi>, core::max<u64>};
	}

	constexpr Int128() noexcept = default;

	// NOLINTBEGIN(google-explicit-constructor)

	template <class T,
	    RB_REQUIRES(isIntegral<T>)>
	constexpr Int128(T value) noexcept
	    : lo_(static_cast<u64>(value)) {
		if constexpr (isSigned<T>) {
			hi_ = value < 0 ? static_cast<Hi>(core::max<u64>) : 0;
		}
	}

	template <class T,
	    RB_REQUIRES(isFloatingPoint<T>&& kUnsigned)>
	constexpr Int128(T value) {
		bool const inRange = //
		    std::isfinite(value)
		    && (value > -1)
		    && (std::numeric_limits<T>::max_exponent <= 128 || value < std::ldexp(static_cast<T>(1), 128));
		RB_RANGE_ASSERT_MSG(inRange, "Value is NaN or cannot fit into u128");

		if (value >= std::ldexp(static_cast<T>(1), 64)) {
			hi_ = std::ldexp(value, -64);
			lo_ = value - std::ldexp(static_cast<T>(hi_), 64);
		} else {
			lo_ = static_cast<u64>(value);
		}
	}

	template <class T,
	    RB_REQUIRES(isFloatingPoint<T> && !kUnsigned)>
	constexpr Int128(T value) {
		auto const bound = std::ldexp(static_cast<T>(1), 127);
		bool const inRange = //
		    std::isfinite(value)
		    && (std::numeric_limits<T>::max_exponent <= 127 || (value >= -bound && value <= bound));
		RB_RANGE_ASSERT_MSG(inRange, "Value is NaN or cannot fit into i128");

		auto const result = value < 0 ? -Twin(-value) : Twin(value);
		hi_ = result.hi_;
		lo_ = result.lo_;
	}

#ifdef __cpp_conditional_explicit
	constexpr explicit(!kUnsigned) Int128(Twin value) noexcept
	    : Int128(static_cast<Hi>(value.hi_), value.lo_) {
	}
#else
	template <bool _ = true, RB_REQUIRES(_&& kUnsigned)>
	constexpr Int128(Twin value) noexcept
	    : Int128(static_cast<Hi>(value.hi_), value.lo_) {
	}

	template <bool _ = true, RB_REQUIRES(_ && !kUnsigned)>
	constexpr explicit Int128(Twin value) noexcept
	    : Int128(static_cast<Hi>(value.hi_), value.lo_) {
	}
#endif

	// NOLINTEND(google-explicit-constructor)

	constexpr Int128(Int128 const&) noexcept = default;
	constexpr Int128(Int128&&) noexcept = default;
	~Int128() = default;

	constexpr Int128& operator=(Int128 const&) noexcept = default;
	constexpr Int128& operator=(Int128&&) noexcept = default;

	constexpr explicit operator bool() const noexcept {
		return hi_ || lo_;
	}

	constexpr bool operator!() const noexcept {
		return !hi_ && !lo_;
	}

	template <class T,
	    RB_REQUIRES_T(IsIntegral<T>)>
	constexpr explicit operator T() const noexcept {
		return static_cast<T>(lo_);
	}

	template <class T,
	    RB_REQUIRES_T(IsFloatingPoint<T>)>
	constexpr explicit operator T() const noexcept {
		if constexpr (!kUnsigned) {
			if (hi_ < 0 && *this != min()) {
				return -static_cast<T>(-*this);
			}
		}
		return static_cast<T>(lo_) + std::ldexp(static_cast<T>(hi_), 64);
	}

	constexpr explicit operator Twin() const noexcept {
		return *this;
	}

	constexpr Int128 operator+() const noexcept {
		return *this;
	}

	constexpr Int128 operator-() const noexcept {
		return {~hi_ + (lo_ == 0), ~lo_ + 1};
	}

	constexpr Int128 operator~() const noexcept {
		return {~hi_, ~lo_};
	}

	constexpr Int128& operator++() noexcept {
		return *this += 1;
	}

	constexpr Int128 operator++(int) noexcept {
		Int128 result{*this};
		++result;
		return result;
	}

	constexpr Int128& operator--() noexcept {
		return *this -= 1;
	}

	constexpr Int128 operator--(int) noexcept {
		Int128 result{*this};
		--result;
		return result;
	}

	constexpr bool operator==(Int128 rhs) const noexcept {
		return hi_ == rhs.hi_ && lo_ == rhs.lo_;
	}

	constexpr bool operator!=(Int128 rhs) const noexcept {
		return !(*this == rhs);
	}

	constexpr Int128 operator+(Int128 rhs) const noexcept {
		return addResult({hi_ + rhs.hi_, lo_ + rhs.lo_}, *this);
	}

	constexpr Int128& operator+=(Int128 rhs) noexcept {
		return *this = *this + rhs;
	}

	constexpr Int128 operator-(Int128 rhs) const noexcept {
		return subtractResult({hi_ - rhs.hi_, lo_ - rhs.lo_}, *this, rhs);
	}

	constexpr Int128& operator-=(Int128 rhs) noexcept {
		return *this = *this - rhs;
	}

	constexpr Int128 operator<<(int amount) const noexcept {
		if (!amount) {
			return *this;
		}
		if (amount >= 64) {
			return {static_cast<Hi>(lo_ << (amount - 64)), 0};
		}
		return {(hi_ << amount) | static_cast<Hi>(lo_ >> (64 - amount)), lo_ << amount};
	}

	constexpr Int128& operator<<=(int amount) noexcept {
		return *this = *this << amount;
	}

	constexpr Int128 operator>>(int amount) const noexcept {
		if (!amount) {
			return *this;
		}
		if (amount >= 64) {
			if constexpr (kUnsigned) {
				return {0, hi_ >> (amount - 64)};
			} else {
				return {(hi_ >> 32) >> 32, u64(hi_ >> (amount - 64))};
			}
		}
		return {hi_ >> amount, (lo_ >> amount) | (static_cast<u64>(hi_) << (64 - amount))};
	}

	constexpr Int128& operator>>=(int amount) noexcept {
		return *this = *this >> amount;
	}

	template <bool _ = true, RB_REQUIRES(_&& kUnsigned)>
	constexpr Int128 operator*(Int128 rhs) const noexcept {
		auto const a32 = lo_ >> 32;
		auto const a00 = lo_ & 0xffff'ffff;
		auto const b32 = rhs.lo_ >> 32;
		auto const b00 = rhs.lo_ & 0xffff'ffff;
		Int128 result{hi_ * rhs.lo_ + lo_ * rhs.hi_ + a32 * b32, a00 * b00};
		result += Int128{a32 * b00} << 32;
		result += Int128{a00 * b32} << 32;
		return result;
	}

	template <bool _ = true, RB_REQUIRES(_ && !kUnsigned)>
	constexpr Int128 operator*(Int128 rhs) const noexcept {
		return Int128{Twin{*this} * Twin{rhs}};
	}

	constexpr Int128& operator*=(Int128 rhs) noexcept {
		return *this = *this * rhs;
	}

	template <bool _ = true, RB_REQUIRES(_&& kUnsigned)>
	constexpr Int128 operator/(Int128 rhs) const noexcept {
		Int128 quo;
		Int128 rem;
		divMod(*this, rhs, quo, rem);
		return quo;
	}

	template <bool _ = true, RB_REQUIRES(_ && !kUnsigned)>
	constexpr Int128 operator/(Int128 rhs) const noexcept {
		auto const [quo, rem] = quorem(*this, rhs);
		return quo;
	}

	constexpr Int128& operator/=(Int128 rhs) noexcept {
		return *this = *this / rhs;
	}

	template <bool _ = true, RB_REQUIRES(_&& kUnsigned)>
	constexpr Int128 operator%(Int128 rhs) const noexcept {
		Int128 quo;
		Int128 rem;
		divMod(*this, rhs, quo, rem);
		return rem;
	}

	template <bool _ = true, RB_REQUIRES(_ && !kUnsigned)>
	constexpr Int128 operator%(Int128 rhs) const noexcept {
		auto const [quo, rem] = quorem(*this, rhs);
		return rem;
	}

	constexpr Int128& operator%=(Int128 rhs) noexcept {
		return *this = *this % rhs;
	}

	constexpr Int128 operator|(Int128 rhs) const noexcept {
		return {hi_ | rhs.hi_, lo_ | rhs.lo_};
	}

	constexpr Int128& operator|=(Int128 rhs) noexcept {
		return *this = *this | rhs;
	}

	constexpr Int128 operator&(Int128 rhs) const noexcept {
		return {hi_ & rhs.hi_, lo_ & rhs.lo_};
	}

	constexpr Int128& operator&=(Int128 rhs) noexcept {
		return *this = *this & rhs;
	}

	constexpr Int128 operator^(Int128 rhs) const noexcept {
		return {hi_ ^ rhs.hi_, lo_ ^ rhs.lo_};
	}

	constexpr Int128& operator^=(Int128 rhs) noexcept {
		return *this = *this ^ rhs;
	}

	constexpr int opCmp(Int128 rhs) const noexcept {
		if (hi_ == rhs.hi_) {
			if (lo_ == rhs.lo_) {
				return 0;
			}
			return lo_ < rhs.lo_ ? -1 : 1;
		}
		return hi_ < rhs.hi_ ? -1 : 1;
	}

	constexpr bool operator<(Int128 rhs) const noexcept {
		return opCmp(rhs) < 0;
	}

	constexpr bool operator<=(Int128 rhs) const noexcept {
		return opCmp(rhs) <= 0;
	}

	constexpr bool operator>(Int128 rhs) const noexcept {
		return opCmp(rhs) > 0;
	}

	constexpr bool operator>=(Int128 rhs) const noexcept {
		return opCmp(rhs) >= 0;
	}

	template <bool _ = true, RB_REQUIRES(_&& kUnsigned)>
	std::string toFormattedString(std::ios_base::fmtflags flags) const {
		// Select a divisor which is the largest power of the base < 2^64.
		Int128 div{10'000'000'000'000'000'000ULL}; // 10^19
		int divBaseLog{19};
		switch (flags & std::ios::basefield) {
			case std::ios::hex:
				div = 0x1000'0000'0000'0000; // 16^15
				divBaseLog = 15;
				break;

			case std::ios::oct:
				div = 01000000000000000000000; // 8^21
				divBaseLog = 21;
				break;

			default:
				break;
		}

		// Now piece together the uint128 representation from three chunks of the original value,
		// each less than "div" and therefore representable as an u64
		std::ostringstream os;
		auto const copyMask = std::ios::basefield | std::ios::showbase | std::ios::uppercase;
		os.setf(flags & copyMask, copyMask);
		Int128 high = *this;
		Int128 low;
		divMod(high, div, high, low);
		Int128 mid;
		divMod(high, div, high, mid);
		if (high.lo_) {
			os << high.lo_;
			os << std::noshowbase << std::setfill('0') << std::setw(divBaseLog);
			os << mid.lo_;
			os << std::setw(divBaseLog);
		} else if (mid.lo_) {
			os << mid.lo_;
			os << std::noshowbase << std::setfill('0') << std::setw(divBaseLog);
		}
		os << low.lo_;
		return os.str();
	}

private:
	static constexpr Int128 addResult(Int128 result, Int128 lhs) noexcept {
		return (result.lo_ < lhs.lo_) ? Int128{result.hi_ + 1, result.lo_} : result;
	}

	static constexpr Int128 subtractResult(Int128 result, Int128 lhs, Int128 rhs) noexcept {
		return (lhs.lo_ < rhs.lo_) ? Int128{result.hi_ - 1, result.lo_} : result;
	}

#if defined(RB_IS_LITTLE_ENDIAN)
	constexpr Int128(Hi hi, u64 lo) noexcept
	    : lo_(lo)
	    , hi_(hi) {
	}

	u64 lo_ = 0;
	Hi hi_ = 0;
#elif defined(RB_IS_BIG_ENDIAN)
	constexpr Int128(Hi hi, u64 lo) noexcept
	    : hi_(hi)
	    , lo_(lo) {
	}

	Hi hi_ = 0;
	u64 lo_ = 0;
#else
	#error "Unsupported byte order"
#endif
};

using i128 = Int128<false>; // NOLINT(readability-identifier-naming)
using u128 = Int128<true>; // NOLINT(readability-identifier-naming)

template <class T, bool _,
    RB_REQUIRES(isArithmetic<T>)>
constexpr Int128<_> operator*(T lhs, Int128<_> rhs) noexcept {
	return rhs * lhs;
}

inline namespace literals {

	constexpr i128 operator""_i128(unsigned long long value) noexcept {
		return i128{value};
	}

	constexpr u128 operator""_u128(unsigned long long value) noexcept {
		return u128{value};
	}

} // namespace literals

/// Returns the 0-based position of the last set bit (i.e., most significant bit) in the given u128
constexpr unsigned fls128(u128 value) noexcept {
	RB_DEBUG_ASSERT(value);
	if (u64 hi = static_cast<u64>(value >> 64)) {
		return 127 - countLeadingZeroes(hi);
	}
	return 63 - countLeadingZeroes(static_cast<u64>(value));
}

constexpr void divMod(u128 dividend, u128 divisor, u128& quo, u128& rem) noexcept {
	if (divisor > dividend) {
		quo = 0;
		rem = dividend;
		return;
	}

	if (divisor == dividend) {
		quo = 1;
		rem = 0;
		return;
	}

	u128 denominator = divisor;
	u128 quotient;
	// left aligns the MSB of the denominator and the dividend
	auto const shift = fls128(dividend) - fls128(denominator);
	denominator <<= static_cast<int>(shift);
	// Uses shift-subtract algorithm to divide dividend by denominator.
	// The remainder will be left in dividend
	for (unsigned i = 0; i <= shift; ++i) {
		quotient <<= 1;
		if (dividend >= denominator) {
			dividend -= denominator;
			quotient |= 1;
		}
		denominator >>= 1;
	}

	quo = quotient;
	rem = dividend;
}

template <bool kUnsigned>
constexpr u128 abs(Int128<kUnsigned> const& value) noexcept {
	if constexpr (kUnsigned) {
		return value;
	} else {
		return value < 0 ? -u128{value} : u128{value};
	}
}

inline std::ostream& operator<<(std::ostream& os, u128 const& value) {
	auto const flags = os.flags();
	std::string rep = value.toFormattedString(flags);
	auto const width = os.width(0);
	if (static_cast<usize>(width) > rep.size()) {
		usize const count = static_cast<usize>(width) - rep.size();
		auto const adjustField = flags & std::ios::adjustfield;
		if (adjustField == std::ios::left) {
			rep.append(count, os.fill());
		} else if (value
		           && adjustField == std::ios::internal
		           && (flags & std::ios::showbase)
		           && (flags & std::ios::basefield) == std::ios::hex) {
			rep.insert(2_UZ, count, os.fill());
		} else {
			rep.insert(0_UZ, count, os.fill());
		}
	}
	return os << rep;
}

inline std::ostream& operator<<(std::ostream& os, i128 const& value) {
	auto const flags = os.flags();
	std::string rep;

	// add the sign if needed
	bool const printAsDecimal = //
	    (flags & std::ios::basefield) == std::ios::dec
	    || (flags & std::ios::basefield) == std::ios_base::fmtflags();
	if (printAsDecimal) {
		if (value < 0) {
			rep = "-";
		} else if (flags & std::ios::showpos) {
			rep = "+";
		}
		rep.append(abs(value).toFormattedString(os.flags()));
	} else {
		rep.append(u128{value}.toFormattedString(os.flags()));
	}

	// add the requisite padding
	auto const width = os.width(0);
	if (static_cast<usize>(width) > rep.size()) {
		usize const count = static_cast<usize>(width) - rep.size();
		switch (flags & std::ios::adjustfield) {
			case std::ios::left:
				rep.append(count, os.fill());
				break;

			case std::ios::internal:
				if (printAsDecimal && (rep[0] == '+' || rep[0] == '-')) {
					rep.insert(1_UZ, count, os.fill());
				} else if ((flags & std::ios::basefield) == std::ios::hex && (flags & std::ios::showbase) && value) {
					rep.insert(2_UZ, count, os.fill());
				} else {
					rep.insert(0_UZ, count, os.fill());
				}
				break;

			default: // std::ios::right
				rep.insert(0_UZ, count, os.fill());
				break;
		}
	}
	return os << rep;
}

template <class T, bool kUnsigned,
    RB_REQUIRES(isIntegral<T>&& isUnsigned<T>)>
constexpr Int128<kUnsigned> pow(Int128<kUnsigned> const& value, T p) noexcept {
	using I = Int128<kUnsigned>;

	if (p == 0) {
		return I::kOne;
	}

	if (p == 1) {
		return value;
	}

	I result = I::kOne;
	I base = value;
	while (p) {
		if (p % 2) {
			result *= base;
		}
		p /= 2;
		base *= base;
	}
	return result;
}

template <class T, bool kUnsigned,
    RB_REQUIRES(isIntegral<T>&& isSigned<T>)>
constexpr Int128<kUnsigned> pow(Int128<kUnsigned> const& x, T value) noexcept {
	using I = Int128<kUnsigned>;

	if (value >= 0) {
		return pow(x, static_cast<Unsigned<T>>(value));
	}

	if (x == I::kOne) {
		return I::kOne;
	}

	if constexpr (!kUnsigned) {
		if (x == I::kMinusOne) {
			return value % 2 ? I::kMinusOne : I::kOne;
		}
	}

	if (x) {
		return I{};
	}

	raiseFpe();
}

template <>
inline constexpr i128 max<i128> = i128::max();

template <>
inline constexpr u128 max<u128> = u128::max();

template <>
inline constexpr i128 min<i128> = i128::min();

template <>
inline constexpr u128 min<u128> = u128::min();

constexpr QuoRem<u128> quorem(u128 dividend, u128 divisor) noexcept {
	u128 quo;
	u128 rem;
	divMod(dividend, divisor, quo, rem);
	return {quo, rem};
}

constexpr QuoRem<i128> quorem(i128 dividend, i128 divisor) noexcept {
	if (dividend == i128::min() && divisor == -1) {
		raiseFpe();
	}

	u128 quo;
	u128 rem;
	divMod(abs(dividend), abs(divisor), quo, rem);
	if ((dividend < 0) != (divisor < 0)) {
		quo = -quo;
	}
	if (dividend < 0) {
		rem = -rem;
	}
	return {i128{quo}, i128{rem}};
}

constexpr QuoRem<u128> quorem(i128 dividend, u128 divisor) noexcept {
	return quorem(u128{dividend}, divisor);
}

constexpr QuoRem<u128> quorem(u128 dividend, i128 divisor) noexcept {
	return quorem(dividend, u128{divisor});
}

} // namespace rb::core

// NOLINTBEGIN(readability-identifier-naming)

namespace std {

template <bool kUnsigned>
struct numeric_limits<rb::core::Int128<kUnsigned>> { // NOLINT(cert-dcl58-cpp)
	using Int128 = rb::core::Int128<kUnsigned>;

	static constexpr bool is_specialized = true;

	static constexpr Int128 min() noexcept {
		return Int128::min();
	}

	static constexpr Int128 max() noexcept {
		return Int128::max();
	}

	static constexpr Int128 lowest() noexcept {
		return min();
	}

	static constexpr int digits = 128 - !kUnsigned;
	static constexpr int digits10 = 38;
	static constexpr int max_digits10 = 0;
	static constexpr bool is_signed = !kUnsigned;
	static constexpr bool is_integer = true;
	static constexpr bool is_exact = true;
	static constexpr int radix = 2;

	static constexpr Int128 epsilon() noexcept {
		return 0;
	}

	static constexpr Int128 round_error() noexcept {
		return 0;
	}

	static constexpr int min_exponent = 0;
	static constexpr int min_exponent10 = 0;
	static constexpr int max_exponent = 0;
	static constexpr int max_exponent10 = 0;

	static constexpr bool has_infinity = false;
	static constexpr bool has_quiet_NaN = false;
	static constexpr bool has_signaling_NaN = false;
	static constexpr float_denorm_style has_denorm = denorm_absent;
	static constexpr bool has_denorm_loss = false;

	static constexpr Int128 infinity() noexcept {
		return 0;
	}

	static constexpr Int128 quiet_NaN() noexcept {
		return 0;
	}

	static constexpr Int128 signaling_NaN() noexcept {
		return 0;
	}

	static constexpr Int128 denorm_min() noexcept {
		return 0;
	}

	static constexpr bool is_iec559 = false;
	static constexpr bool is_bounded = true;
	static constexpr bool is_modulo = true;
	static constexpr bool traps = true;
	static constexpr bool tinyness_before = false;
	static constexpr float_round_style round_style = round_toward_zero;
};

} // namespace std

// NOLINTEND(readability-identifier-naming)
