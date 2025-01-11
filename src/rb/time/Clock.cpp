#include "Clock.hpp"

using namespace rb::time;

#ifdef RB_OS_WIN

	#include <Windows.h>

	#include <rb/core/os.hpp>

namespace {

u64 getQpcFrequency() noexcept {
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return frequency.QuadPart;
}

Duration toDuration(FILETIME fileTime) noexcept {
	constexpr u64 kTicksFrom1601ToUnixEpoch = 116444736000000000ULL;
	constexpr u64 kTicksPerSecond = 10000000;
	constexpr u64 kNanosecondsPerTick = 100;
	auto const ticksSince1601 = rb::core::bitCast<u64>(fileTime);
	auto const ticks = ticksSince1601 - kTicksFrom1601ToUnixEpoch;
	auto const secs = static_cast<i64>(ticks / kTicksPerSecond);
	auto const nsecs = static_cast<i64>((ticks % kTicksPerSecond) * kNanosecondsPerTick);
	return impl::duration(secs, nsecs * Duration::kTicksPerNanosecond);
}

} // namespace

Duration Clock<ClockId::kMonotonic>::now() noexcept {
	static u64 const kQpcFrequency = getQpcFrequency();

	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	auto const secs = static_cast<i64>(count.QuadPart / kQpcFrequency);
	auto const frac = count.QuadPart % kQpcFrequency;
	auto const ticks = static_cast<i64>(frac * (Duration::kTicksPerSecond / kQpcFrequency));
	return impl::duration(secs, ticks);
}

Duration Clock<ClockId::kMonotonicFast>::now() noexcept {
	return milliseconds(GetTickCount64());
}

Duration Clock<ClockId::kRealtime>::now() noexcept {
	FILETIME fileTime;
	GetSystemTimePreciseAsFileTime(&fileTime);
	return toDuration(fileTime);
}

Duration Clock<ClockId::kRealtimeFast>::now() noexcept {
	FILETIME fileTime;
	GetSystemTimeAsFileTime(&fileTime);
	return toDuration(fileTime);
}

Duration Clock<ClockId::kUptime>::now() noexcept {
	return milliseconds(GetTickCount64());
}

#else

namespace {

// TODO add fallback variants
Duration getTime(clockid_t clockId) noexcept {
	std::timespec ts = {};
	clock_gettime(clockId, &ts);
	return Duration::from(ts);
}

} // namespace

Duration Clock<ClockId::kMonotonic>::now() noexcept {
	return getTime(CLOCK_MONOTONIC);
}

Duration Clock<ClockId::kMonotonicFast>::now() noexcept {
	return getTime(CLOCK_MONOTONIC_COARSE);
}

Duration Clock<ClockId::kRealtime>::now() noexcept {
	return getTime(CLOCK_REALTIME);
}

Duration Clock<ClockId::kRealtimeFast>::now() noexcept {
	return getTime(CLOCK_REALTIME_COARSE);
}

Duration Clock<ClockId::kUptime>::now() noexcept {
	return getTime(CLOCK_BOOTTIME);
}

Duration Clock<ClockId::kTai>::now() noexcept {
	return getTime(CLOCK_TAI); // since Linux 3.10; Linux-specific
}

Duration Clock<ClockId::kProcessCpuTime>::now() noexcept {
	return getTime(CLOCK_PROCESS_CPUTIME_ID);
}

Duration Clock<ClockId::kThreadCpuTime>::now() noexcept {
	return getTime(CLOCK_THREAD_CPUTIME_ID);
}

#endif
