#pragma once

#include <ostream>

#include <rb/core/assert.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/IsConvertible.hpp>
#include <rb/core/traits/Unsigned.hpp>

namespace rb::core {
inline namespace slices {

	template <class T>
	struct RB_EXPORT StepSlice {
		T const begin;
		T const end;
		T const step;

		constexpr StepSlice() noexcept
		    : StepSlice(0) {
		}

		constexpr explicit StepSlice(T end) noexcept
		    : StepSlice(0, end) {
		}

		constexpr StepSlice(T begin, T end, T step = 1) noexcept
		    : begin(begin)
		    , end(end)
		    , step(step) {
			RB_ASSERT_MSG(step != 0, "Step must be non-zero");
		}

		template <class U,
		    RB_REQUIRES_T(IsConvertible<U, T>)>
		constexpr StepSlice(StepSlice<U> const& rhs) noexcept // NOLINT(google-explicit-constructor)
		    : begin(rhs.begin)
		    , end(rhs.end)
		    , step(rhs.step) {
		}

		template <class U,
		    RB_REQUIRES_T(IsConvertible<U, T>)>
		constexpr explicit StepSlice(Slice<U> const& rhs) noexcept
		    : begin(rhs.begin)
		    , end(rhs.end)
		    , step(1) {
		}

		constexpr Unsigned<T> size() const noexcept {
			if (begin == end) {
				return 0;
			}

			if (begin < end) {
				return step > 0 ? (1 + (end - begin - 1) / step) : 0;
			}

			return step > 0 ? 0 : (1 + (begin - end - 1) / (-step));
		}

		[[nodiscard]] constexpr bool empty() const noexcept {
			return size() == 0;
		}
	};

	template <class Char, class Traits, class T>
	std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, StepSlice<T> const& slice) {
		return os << os.widen('[')
		          << slice.begin
		          << os.widen(',') << os.widen(' ')
		          << slice.end
		          << os.widen(',') << os.widen(' ')
		          << slice.step
		          << os.widen(')');
	}

} // namespace slices
} // namespace rb::core
