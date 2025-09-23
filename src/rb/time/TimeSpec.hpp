#pragma once

#include <ostream>
#include <tuple>

// ReSharper disable CppUnusedIncludeDirective
#include <rb/core/compiler.hpp>
#include <rb/core/os.hpp>
#if defined(RB_COMPILER_MINGW) || defined(RB_OS_CYGWIN)
// NOLINTBEGIN
	#include <time.h>

namespace std {
using ::timespec;
} // namespace std

// NOLINTEND
#else
	#include <ctime>
#endif

#include <rb/core/error/InvalidArgumentError.hpp>
#include <rb/core/Option.hpp>
#include <rb/time/I64.hpp>

namespace rb::time {

class TimeSpec final {
public:
	static constexpr u32 kNanosecondsPerSecond = 1'000'000'000;

	static constexpr TimeSpec from(std::timespec ts) {
		// ReSharper disable once CppRedundantCastExpression
		return TimeSpec{static_cast<i64>(ts.tv_sec), static_cast<u32>(ts.tv_nsec)};
	}

	constexpr TimeSpec() noexcept = default;

	constexpr explicit TimeSpec(i64 seconds) noexcept
	    : secs_{seconds} {
	}

	constexpr explicit TimeSpec(i64 seconds, u32 nanoseconds)
	    : secs_{seconds}
	    , nsecs_{nanoseconds} {
		// ReSharper disable once CppDFAConstantConditions
		if (nsecs_ >= kNanosecondsPerSecond) {
			throw core::InvalidArgumentError{};
		}
	}

	constexpr i64 seconds() const noexcept {
		return secs_;
	}

	constexpr u32 nanoseconds() const noexcept {
		return nsecs_;
	}

	constexpr core::Option<std::timespec> toTimespec() const noexcept {
		std::timespec ts = {static_cast<std::time_t>(secs_), static_cast<long>(nsecs_)};
		if (ts.tv_sec == secs_) { // no time_t narrowing
			return ts;
		}
		return core::kNone;
	}

private:
	impl::I64 secs_;
	u32 nsecs_ = 0;
};

inline std::ostream& operator<<(std::ostream& os, TimeSpec ts) {
	return os << "TimeSpec{secs: " << ts.seconds() << ", nsecs: " << ts.nanoseconds() << "}";
}

namespace impl {

	template <usize n>
	struct TimeSpecGetter;

	template <>
	struct TimeSpecGetter<0> {
		static constexpr i64 get(TimeSpec ts) noexcept {
			return ts.seconds();
		}
	};

	template <>
	struct TimeSpecGetter<1> {
		static constexpr u32 get(TimeSpec ts) noexcept {
			return ts.nanoseconds();
		}
	};

} // namespace impl

template <usize n>
constexpr auto get(TimeSpec ts) noexcept {
	return impl::TimeSpecGetter<n>::get(ts);
}

} // namespace rb::time

template <>
struct std::tuple_size<rb::time::TimeSpec>
    : std::integral_constant<usize, 2> {
};

template <>
struct std::tuple_element<0, rb::time::TimeSpec> {
	using type = i64; // NOLINT(*-identifier-naming)
};

template <>
struct std::tuple_element<1, rb::time::TimeSpec> {
	using type = u32; // NOLINT(*-identifier-naming)
};
