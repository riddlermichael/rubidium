#pragma once

#include <rb/core/os.hpp>

#ifdef RB_OS_WIN

	#include <Windows.h>

	#include <rb/time/Duration.hpp>

namespace rb::time {

constexpr Duration toDurationUnix(FILETIME fileTime) noexcept {
	constexpr u64 kTicksFrom1601ToUnixEpoch = 116444736000000000ULL;
	constexpr u64 kTicksPerSecond = 10'000'000;
	constexpr u64 kNanosecondsPerTick = 100;
	auto const ticksSince1601 = core::bitCast<u64>(fileTime);
	auto const ticks = ticksSince1601 - kTicksFrom1601ToUnixEpoch;
	auto const secs = static_cast<i64>(ticks / kTicksPerSecond);
	auto const nsecs = static_cast<i64>((ticks % kTicksPerSecond) * kNanosecondsPerTick);
	return impl::duration(secs, nsecs * Duration::kTicksPerNanosecond);
}

constexpr Duration toDuration(FILETIME fileTime) noexcept {
	auto const hnsecs = core::bitCast<u64>(fileTime);
	return nanoseconds(hnsecs) * 100;
}

} // namespace rb::time

#endif
