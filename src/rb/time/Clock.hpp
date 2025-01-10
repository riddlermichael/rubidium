#pragma once

#include <rb/time/Duration.hpp>

namespace rb::time {

enum class ClockId {
	kMonotonic,
	kMonotonicCoarse,
	kRealtime,
	kRealtimeCoarse,
	kBootTime,
	// CLOCK_TAI
	// CLOCK_PROCESS_CPUTIME_ID
	// CLOCK_THREAD_CPUTIME_ID
};

template <ClockId clockId>
class Clock;

template <>
class Clock<ClockId::kMonotonic> {
public:
	static constexpr bool kIsMonotonic = true;

	static Duration now() noexcept;
};

template <>
class Clock<ClockId::kMonotonicCoarse> {
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
class Clock<ClockId::kRealtimeCoarse> {
public:
	static constexpr bool kIsMonotonic = false;

	static Duration now() noexcept;
};

template <>
class Clock<ClockId::kBootTime> {
public:
	static constexpr bool kIsMonotonic = true;

	static Duration now() noexcept;
};

} // namespace rb::time
