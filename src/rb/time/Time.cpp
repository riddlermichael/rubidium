#include "Time.hpp"

#include <rb/core/bits.hpp>
#include <rb/core/os.hpp>

#ifdef RB_OS_WIN
	#include <Windows.h>
#endif

namespace rb::time {

Time Time::deadlineFromTimeout(Duration timeout) noexcept {
	return now() + timeout;
}

Time Time::now() noexcept {
#ifdef RB_OS_WIN
	constexpr u64 ticksFrom1601ToUnixEpoch = 116'444'736'000'000'000ULL;
	constexpr u64 ticksPerSecond = 10'000'000;
	constexpr u64 nanosecondsPerTick = 100;

	FILETIME ft;
	GetSystemTimePreciseAsFileTime(&ft);
	// number of 100ns ticks since 1601-01-01T00:00:00Z
	u64 const ticksSince1601 = core::bitCast<u64>(ft);
	u64 const ticks = ticksSince1601 - ticksFrom1601ToUnixEpoch;
	u64 const secs = ticks / ticksPerSecond;
	u64 const nsecs = (ticks - secs * ticksPerSecond) * nanosecondsPerTick;
	return Time{seconds(secs) + nanoseconds(nsecs)};
#else
	return {}; // FIXME
#endif
}

std::ostream& operator<<(std::ostream& os, Time t) {
	return os << impl::toUnixDuration(t);
}

} // namespace rb::time
