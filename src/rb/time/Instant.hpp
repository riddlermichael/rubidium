#pragma once

#include <rb/time/TimePoint.hpp>

namespace rb::time {

/// A measurement of a monotonically nondecreasing clock.
///
/// A clock is *monotonic* if the value returned by a first call to now() is always less than or equal to the value
/// that is returned by a later call to now().
///
/// A clock is *steady* if the time between clock ticks is constant.
/// Instant is **not** guaranteed to be steady.
class Instant final {
	using Impl = MonotonicTimePoint;

public:
	static Instant now() noexcept {
		return Instant{Impl::now()};
	}

	constexpr Instant() noexcept = default;

	constexpr bool operator==(Instant rhs) const noexcept {
		return impl_ == rhs.impl_;
	}

	constexpr bool operator<(Instant rhs) const noexcept {
		return impl_ < rhs.impl_;
	}

	constexpr Instant& operator+=(Duration dur) noexcept {
		impl_ += dur;
		return *this;
	}

	constexpr Instant& operator-=(Duration dur) noexcept {
		impl_ -= dur;
		return *this;
	}

	constexpr Duration operator-(Instant rhs) const noexcept {
		return since(rhs);
	}

	constexpr bool isInf() const noexcept {
		return impl_.isInf();
	}

	/// @return the amount of time elapsed from @p rhs to *this, or zero duration if @p rhs is later than *this.
	constexpr Duration since(Instant rhs) const noexcept {
		auto const duration = impl_ - rhs.impl_;
		return duration.isNegative() ? Duration::zero() : duration;
	}

	Duration elapsed() const noexcept {
		return impl_.elapsed();
	}

private:
	friend std::ostream& operator<<(std::ostream& os, Instant instant) {
		return os << "Instant{" << instant.impl_.sinceEpoch() << "}";
	}

	constexpr explicit Instant(Impl impl) noexcept
	    : impl_{impl} {
	}

	Impl impl_;
};

#pragma region synthesized operators

constexpr bool operator>(Instant lhs, Instant rhs) noexcept {
	return rhs < lhs;
}

constexpr bool operator>=(Instant lhs, Instant rhs) noexcept {
	return !(lhs < rhs);
}

constexpr bool operator<=(Instant lhs, Instant rhs) noexcept {
	return !(rhs < lhs);
}

constexpr bool operator!=(Instant lhs, Instant rhs) noexcept {
	return !(lhs == rhs);
}

constexpr Instant operator+(Instant lhs, Duration rhs) noexcept {
	return lhs += rhs;
}

constexpr Instant operator+(Duration lhs, Instant rhs) noexcept {
	return rhs += lhs;
}

constexpr Instant operator-(Instant lhs, Duration rhs) noexcept {
	return lhs -= rhs;
}

#pragma endregion synthesized operators

} // namespace rb::time
