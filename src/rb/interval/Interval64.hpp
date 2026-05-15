#pragma once

#include <iosfwd>

// ReSharper disable CppUnusedIncludeDirective
#include <rb/core/builtins.hpp>
#include <rb/core/export.hpp>
#include <rb/core/limits.hpp>
#include <rb/math/float.hpp>
#include <rb/math/max.hpp>

namespace rb::interval {

class RB_EXPORT [[nodiscard]] Interval64 {
	struct Tag {};

	f64 lo_;
	f64 hi_;

	constexpr explicit Interval64(Tag /*unused*/, f64 lo, f64 hi) noexcept
	    : lo_{lo}
	    , hi_{hi} {
	}

public:
	using Self = Interval64;

	static Self const kEmpty;
	static Self const kNaN;
	static Self const kNegativeRay;
	static Self const kPositiveRay;
	static Self const kWhole;
	static Self const kZero;

	static constexpr Self empty() noexcept {
		return {};
	}

	static Self fromCenter(f64 center, f64 radius) noexcept;

	static constexpr Self nan() noexcept {
		return Self{Tag{}, core::nan<f64>, core::nan<f64>};
	}

	static constexpr Self whole() noexcept {
		return Self{Tag{}, -core::inf<f64>, core::inf<f64>};
	}

	static constexpr Self zero() noexcept {
		return Self{Tag{}, 0, 0};
	}

	constexpr Interval64() noexcept
	    : lo_{core::inf<f64>}
	    , hi_{-core::inf<f64>} {
	}

	// ReSharper disable once CppNonExplicitConvertingConstructor
	constexpr Interval64(f64 value) noexcept // NOLINT(*-explicit-constructor)
	    : Self{Tag{}, value, value} {
	}

	constexpr Interval64(f64 lo, f64 hi) noexcept
	    : lo_{lo}
	    , hi_{hi} {
		if (RB_UNLIKELY(math::isNaN(lo_) || math::isNaN(hi_))) {
			*this = nan();
		} else if (RB_UNLIKELY(lo_ > hi_)) {
			*this = empty();
		}
	}

	constexpr Self operator+() const noexcept {
		return *this;
	}

	constexpr Self operator-() const noexcept {
		return Self{Tag{}, -hi_, -lo_};
	}

	constexpr bool operator==(Self const& rhs) const noexcept {
		return lo_ == rhs.lo_ && hi_ == rhs.hi_;
	}

	constexpr bool operator!=(Self const& rhs) const noexcept {
		return !(*this == rhs);
	}

	Self operator+(Self const& rhs) const noexcept;

	Self operator+(f64 value) const noexcept {
		return *this + Self{value};
	}

	Self operator-(Self const& rhs) const noexcept {
		return *this + -rhs;
	}

	Self operator-(f64 value) const noexcept {
		return *this + Self{-value};
	}

	Self operator*(Self const& rhs) const noexcept;

	Self operator*(f64 value) const noexcept;

	constexpr f64 hi() const noexcept {
		return hi_;
	}

	constexpr f64 lo() const noexcept {
		return lo_;
	}

	constexpr f64 magnitude() const noexcept {
		if (RB_UNLIKELY(isEmptyOrNaN())) {
			return core::nan<f64>;
		}
		return math::max(-lo_, hi_);
	}

	constexpr f64 mignitude() const noexcept {
		if (RB_UNLIKELY(isEmptyOrNaN())) {
			return core::nan<f64>;
		}
		return math::max(0.0, lo_, -hi_);
	}

	f64 center() const noexcept;
	f64 radius() const noexcept;
	f64 width() const noexcept;

	constexpr bool containsZero() const noexcept {
		if (RB_UNLIKELY(isEmptyOrNaN())) {
			return false;
		}
		return lo_ <= 0.0 && 0.0 <= hi_;
	}

	constexpr bool isEmpty() const noexcept {
		return *this == empty();
	}

	constexpr bool isEmptyOrNaN() const noexcept {
		return isEmpty() || isNaN();
	}

	constexpr bool isInf() const noexcept {
		if (RB_UNLIKELY(isEmpty())) {
			return false;
		}
		return math::isInf(lo_) || math::isInf(hi_);
	}

	constexpr bool isNaN() const noexcept {
		return math::isNaN(lo_) || math::isNaN(hi_);
	}

	constexpr bool isWhole() const noexcept {
		return *this == whole();
	}
};

constexpr Interval64 Interval64::kEmpty = empty();
constexpr Interval64 Interval64::kNaN = nan();
constexpr Interval64 Interval64::kNegativeRay{Tag{}, -core::inf<f64>, 0.0};
constexpr Interval64 Interval64::kPositiveRay{Tag{}, 0.0, core::inf<f64>};
constexpr Interval64 Interval64::kWhole = whole();
constexpr Interval64 Interval64::kZero = zero();

constexpr bool operator<(Interval64 interval, f64 value) noexcept {
	if (RB_UNLIKELY(interval.isEmptyOrNaN())) {
		return false;
	}
	return interval.hi() < value;
}

constexpr bool operator<=(Interval64 interval, f64 value) noexcept {
	if (RB_UNLIKELY(interval.isEmptyOrNaN())) {
		return false;
	}
	return interval.hi() <= value;
}

constexpr bool operator>(Interval64 interval, f64 value) noexcept {
	if (RB_UNLIKELY(interval.isEmptyOrNaN())) {
		return false;
	}
	return interval.lo() > value;
}

constexpr bool operator>=(Interval64 interval, f64 value) noexcept {
	if (RB_UNLIKELY(interval.isEmptyOrNaN())) {
		return false;
	}
	return interval.lo() >= value;
}

constexpr bool operator<(f64 value, Interval64 interval) noexcept {
	return interval > value;
}

constexpr bool operator<=(f64 value, Interval64 interval) noexcept {
	return interval >= value;
}

constexpr bool operator>(f64 value, Interval64 interval) noexcept {
	return interval < value;
}

constexpr bool operator>=(f64 value, Interval64 interval) noexcept {
	return interval <= value;
}

inline Interval64 operator+(f64 value, Interval64 interval) noexcept {
	return Interval64{value} + interval;
}

inline Interval64 operator-(f64 value, Interval64 interval) noexcept {
	return Interval64{value} - interval;
}

std::ostream& operator<<(std::ostream& os, Interval64 interval);

} // namespace rb::interval

// NaN, empty [+inf, -inf], whole
// NaN, ±inf, ±0

// TODO std::get
// TODO std::hash
// TODO PartialEq, PartialOrd
