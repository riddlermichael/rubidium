#include "Clock.hpp"

#include <rb/core/os.hpp>

using namespace rb::time;

#ifdef RB_OS_WIN

	#include <Windows.h>

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

Duration Clock<ClockId::kMonotonicCoarse>::now() noexcept {
	return milliseconds(GetTickCount64());
}

Duration Clock<ClockId::kRealtime>::now() noexcept {
	FILETIME fileTime;
	GetSystemTimePreciseAsFileTime(&fileTime);
	return toDuration(fileTime);
}

Duration Clock<ClockId::kRealtimeCoarse>::now() noexcept {
	FILETIME fileTime;
	GetSystemTimeAsFileTime(&fileTime);
	return toDuration(fileTime);
}

Duration Clock<ClockId::kBootTime>::now() noexcept {
	return milliseconds(GetTickCount64());
}

#else
#endif
