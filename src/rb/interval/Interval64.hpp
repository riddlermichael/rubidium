#pragma once

#include <iosfwd>

// ReSharper disable CppUnusedIncludeDirective
#include <rb/core/builtins.hpp>
#include <rb/core/export.hpp>
#include <rb/core/limits.hpp>
#include <rb/math/float.hpp>

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
		if (RB_UNLIKELY(lo_ > hi_)) {
			*this = empty();
		}

		if (RB_UNLIKELY(isNaN())) {
			*this = nan();
		}
	}

	constexpr Self operator-() const noexcept {
		return Self{Tag{}, -hi_, -lo_};
	}

	constexpr bool operator==(Self const& rhs) const noexcept {
		// TODO -0.0
		return lo_ == rhs.lo_
		    && hi_ == rhs.hi_;
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

	constexpr bool isEmpty() const noexcept {
		return *this == empty();
	}

	constexpr bool isNaN() const noexcept {
		return math::isNaN(lo_) || math::isNaN(hi_);
	}

	constexpr bool isWhole() const noexcept {
		return *this == whole();
	}

	constexpr f64 hi() const noexcept {
		return hi_;
	}

	constexpr f64 lo() const noexcept {
		return lo_;
	}
};

inline Interval64 operator+(f64 value, Interval64 interval) noexcept {
	return Interval64{value} + interval;
}

std::ostream& operator<<(std::ostream& os, Interval64 const& value);

} // namespace rb::interval
