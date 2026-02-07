#pragma once

#include <rb/time/Duration.hpp>

namespace rb::time {

namespace impl {
	RB_STATIC_METHOD_DETECTOR(now)
} // namespace impl

enum class RB_OPEN_ENUM ClockId {
	/// A system-wide clock that represents monotonic time from some unspecified point in the past.
	/// On Linux, that point corresponds to the number of seconds that the system has been running since it was booted.
	/// The kMonotonic clock is not affected by discontinuous jumps in the system time
	/// (e.g., if the system administrator manually changes the clock),
	/// but is affected by frequency adjustments.
	/// This clock does not count time that the system is suspended.
	/// All kMonotonic variants guarantee that the time returned by consecutive calls will not go backwards,
	/// but successive calls may return identical (not-increased) time values.
	kMonotonic = 0,

	/// A faster but less precise version of kMonotonic.
	/// Use when you need very fast, but not fine-grained timestamps.
	kMonotonicFast = 1,
	kMonotonicCoarse = kMonotonicFast,

	/// A system-wide clock that measures real (i.e., wall-clock) time.
	/// This clock is affected by discontinuous jumps in the system time
	/// (e.g., if the system administrator manually changes the clock),
	/// and by frequency adjustments performed by NTP and similar applications
	/// via adjtime(3), adjtimex(2), clock_adjtime(2), and ntp_adjtime(3).
	/// This clock normally counts the number of seconds since 1970-01-01 00:00:00 Coordinated Universal Time (UTC)
	/// except that it ignores leap seconds;
	/// near a leap second it is typically adjusted by NTP to stay roughly in sync with UTC.
	kRealtime = 2,

	/// A faster but less precise version of kRealtime.
	/// Use when you need very fast, but not fine-grained timestamps.
	kRealtimeFast = 3,
	kRealtimeCoarse = kRealtimeFast,

	/// A system-wide clock that is identical to kMonotonic,
	/// except that it also includes any time that the system is suspended.
	/// This allows applications to get a suspend-aware monotonic clock
	/// without having to deal with the complications of kRealtime,
	/// which may have discontinuities if the time is changed using settimeofday(2) or similar.
	kUptime = 4,
	kBootTime = kUptime,

	/// This is a clock that measures CPU time consumed by this process
	/// (i.e., CPU time consumed by all threads in the process).
	kProcessCpuTime = 5,

	/// This is a clock that measures CPU time consumed by this thread.
	kThreadCpuTime = 6,

	/// A system-wide clock derived from wall-clock time but counting leap seconds.
	/// This clock does not experience discontinuities or frequency adjustments
	/// caused by inserting leap seconds as kRealtime does.
	/// The acronym TAI refers to International Atomic Time.
	kTai = 7,
};

template <ClockId clockId>
class Clock;

template <class T>
using IsClock = core::IsDetectedExact<Duration, impl::nowStaticDetector, T>;

template <>
class Clock<ClockId::kMonotonic> {
public:
	static constexpr bool kIsMonotonic = true;

	static Duration now() noexcept;
};

template <>
class Clock<ClockId::kRealtime> {
public:
	static constexpr bool kIsMonotonic = false;

	static Duration now() noexcept;
};

template <>
class Clock<ClockId::kProcessCpuTime> {
public:
	static constexpr bool kIsMonotonic = true;

	static Duration now() noexcept;
};

template <>
class Clock<ClockId::kThreadCpuTime> {
public:
	static constexpr bool kIsMonotonic = true;

	static Duration now() noexcept;
};

#ifndef RB_OS_DARWIN
template <>
class Clock<ClockId::kMonotonicFast> {
public:
	static constexpr bool kIsMonotonic = true;

	static Duration now() noexcept;
};

template <>
class Clock<ClockId::kRealtimeFast> {
public:
	static constexpr bool kIsMonotonic = false;

	static Duration now() noexcept;
};

template <>
class Clock<ClockId::kUptime> {
public:
	static constexpr bool kIsMonotonic = true;

	static Duration now() noexcept;
};
#endif

#ifdef RB_OS_LINUX

template <>
class Clock<ClockId::kTai> {
public:
	static constexpr bool kIsMonotonic = false;

	static Duration now() noexcept;
};

#endif

} // namespace rb::time
