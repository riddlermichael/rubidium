#include "SystemTime.hpp"

#include <rb/core/os.hpp>

#ifdef RB_OS_WIN
#include <Windows.h>
#else
	#include <ctime>
#endif

using namespace rb::time;

SystemTime SystemTime::now() noexcept {
#ifdef RB_OS_WIN
    constexpr u64 kTicksFrom1601ToUnixEpoch = 116444736000000000ULL;
    constexpr u64 kTicksPerSecond = 10000000;
    constexpr u64 kNanosecondsPerTick = 100;
    FILETIME ft;
    GetSystemTimePreciseAsFileTime(&ft);
    // QuadPart is the number of 100 ns ticks since 1601-01-01 00:00:00Z
    auto const ticksSince1601 = core::bitCast<u64>(ft);
    auto const ticks = ticksSince1601 - kTicksFrom1601ToUnixEpoch;
    auto const secs = static_cast<i64>(ticks / kTicksPerSecond);
    auto const nsecs = static_cast<i64>((ticks % kTicksPerSecond) * kNanosecondsPerTick);
    auto const dur = impl::duration(secs, nsecs * Duration::kTicksPerNanosecond);
    return SystemTime{dur};
#else
	std::timespec ts = {};
	clock_gettime(CLOCK_REALTIME, &ts);
	return from(ts);
#endif
}

SystemTime::Result SystemTime::elapsed() const noexcept {
    return now().since(*this);
}

std::ostream &rb::time::operator<<(std::ostream &os, SystemTime clock) {
    return os << "SystemTime{" << clock.rep_ << "}";
}
