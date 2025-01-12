#include "Clock.hpp"

#include <rb/time/windows.hpp>

using namespace rb::time;

#ifdef RB_OS_WIN

	#include <Windows.h>

Duration Clock<ClockId::kMonotonic>::now() noexcept {
	static u64 const kQpcFrequency = [] {
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		return frequency.QuadPart;
	}();

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
	return toDurationUnix(fileTime);
}

Duration Clock<ClockId::kRealtimeFast>::now() noexcept {
	FILETIME fileTime;
	GetSystemTimeAsFileTime(&fileTime);
	return toDurationUnix(fileTime);
}

Duration Clock<ClockId::kUptime>::now() noexcept {
	return milliseconds(GetTickCount64());
}

Duration Clock<ClockId::kProcessCpuTime>::now() noexcept {
	FILETIME creationTime;
	FILETIME exitTime;
	FILETIME kernelTime;
	FILETIME userTime;
	GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
	return toDuration(kernelTime) + toDuration(userTime);
}

Duration Clock<ClockId::kThreadCpuTime>::now() noexcept {
	FILETIME creationTime;
	FILETIME exitTime;
	FILETIME kernelTime;
	FILETIME userTime;
	GetThreadTimes(GetCurrentThread(), &creationTime, &exitTime, &kernelTime, &userTime);
	return toDuration(kernelTime) + toDuration(userTime);
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

Duration Clock<ClockId::kProcessCpuTime>::now() noexcept {
	return getTime(CLOCK_PROCESS_CPUTIME_ID);
}

Duration Clock<ClockId::kThreadCpuTime>::now() noexcept {
	return getTime(CLOCK_THREAD_CPUTIME_ID);
}

Duration Clock<ClockId::kTai>::now() noexcept {
	return getTime(CLOCK_TAI); // since Linux 3.10; Linux-specific
}

#endif
