#include "MonoTime.hpp"

#include <rb/core/os.hpp>

#ifdef RB_OS_WIN
#include <Windows.h>
#else
	#include <ctime>
#endif

using namespace rb::time;

#ifdef RB_OS_WIN

namespace {
    u64 getFrequency() noexcept {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return frequency.QuadPart;
    }

    u64 const kFrequency = getFrequency();

    constexpr Duration toDuration(u64 count) noexcept {
        auto const secs = static_cast<i64>(count / kFrequency);
        auto const frac = count % kFrequency;
        auto const ticks = static_cast<i64>(frac * (Duration::kTicksPerSecond / kFrequency));
        return impl::duration(secs, ticks);
    }
} // namespace

#endif

MonoTime MonoTime::now() noexcept {
#ifdef RB_OS_WIN
    LARGE_INTEGER count;
    // On systems that run Windows XP or later, the function will always succeed and will thus never return zero.
    QueryPerformanceCounter(&count);
    return MonoTime{toDuration(count.QuadPart)};
#else
	std::timespec ts = {};
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return MonoTime{Duration::from(ts)};
#endif
}

Duration MonoTime::elapsed() const noexcept {
    return now() - *this;
}

std::ostream &rb::time::operator<<(std::ostream &os, MonoTime clock) {
    return os << "MonoTime{" << clock.rep_ << "}";
}
