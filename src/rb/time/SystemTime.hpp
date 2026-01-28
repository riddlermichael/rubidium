#pragma once

#include <rb/time/TimePoint.hpp>
#ifdef RB_OS_WIN
	#include <rb/time/windows.hpp>
#endif

namespace rb::time {

/// A measurement of the system's real-time clock.
///
/// A clock is *monotonic* if the value returned by a first call to now() is always less than or equal to the value
/// that is returned by a later call to now().
/// The SystemTime isn't monotonic because the system time may be adjusted between calls to now().
///
/// A clock is *steady* if the time between clock ticks is constant.
/// SystemTime is **not** steady.
class SystemTime final {
	using Impl = RealtimeTimePoint;

public:
	using Result = core::Expected<Duration, Duration>;

	static SystemTime const kUnixEpoch;

	static constexpr SystemTime from(std::time_t t) noexcept {
		return SystemTime{seconds(t)};
	}

	static constexpr SystemTime from(std::timespec ts) noexcept {
		return SystemTime{Duration::from(ts)};
	}

#ifdef RB_OS_WIN
	static constexpr SystemTime from(FILETIME fileTime) noexcept {
		return SystemTime{toDurationUnix(fileTime)};
	}
#endif

	static SystemTime now() noexcept {
		return SystemTime{Impl::now()};
	}

	constexpr SystemTime() noexcept = default;

	constexpr bool operator==(SystemTime rhs) const noexcept {
		return impl_ == rhs.impl_;
	}

	constexpr bool operator<(SystemTime rhs) const noexcept {
		return impl_ < rhs.impl_;
	}

	constexpr SystemTime& operator+=(Duration dur) noexcept {
		impl_ += dur;
		return *this;
	}

	constexpr SystemTime& operator-=(Duration dur) noexcept {
		impl_ -= dur;
		return *this;
	}

	constexpr std::time_t toTimeT() const noexcept {
		return toTimespec().tv_sec;
	}

	constexpr std::timespec toTimespec() const noexcept {
		return impl_.sinceEpoch().toTimespec();
	}

	constexpr bool isInf() const noexcept {
		return impl_.isInf();
	}

	constexpr Result since(SystemTime rhs) const noexcept {
		auto const duration = impl_.since(rhs.impl_);
		if (duration.isNegative()) {
			return core::err(duration);
		}
		return core::ok(duration);
	}

	Result elapsed() const noexcept {
		return now().since(*this);
	}

private:
	friend std::ostream& operator<<(std::ostream& os, SystemTime systemTime) {
		return os << "SystemTime{" << systemTime.impl_.sinceEpoch() << "}";
	}

	constexpr explicit SystemTime(Impl impl) noexcept
	    : impl_{impl} {
	}

	constexpr explicit SystemTime(Duration dur) noexcept
	    : impl_{Impl::from(dur)} {
	}

	Impl impl_;
};

inline SystemTime const SystemTime::kUnixEpoch;

#pragma region synthesized operators

constexpr bool operator>(SystemTime lhs, SystemTime rhs) noexcept {
	return rhs < lhs;
}

constexpr bool operator>=(SystemTime lhs, SystemTime rhs) noexcept {
	return !(lhs < rhs);
}

constexpr bool operator<=(SystemTime lhs, SystemTime rhs) noexcept {
	// ReSharper disable once CppRedundantComplexityInComparison
	return !(rhs < lhs);
}

constexpr bool operator!=(SystemTime lhs, SystemTime rhs) noexcept {
	return !(lhs == rhs);
}

constexpr SystemTime operator+(SystemTime lhs, Duration rhs) noexcept {
	return lhs += rhs;
}

constexpr SystemTime operator+(Duration lhs, SystemTime rhs) noexcept {
	return rhs += lhs;
}

constexpr SystemTime operator-(SystemTime lhs, Duration rhs) noexcept {
	return lhs -= rhs;
}

#pragma endregion synthesized operators
} // namespace rb::time
