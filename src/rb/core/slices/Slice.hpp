#pragma once

#include <ostream>

#include <rb/core/export.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/IsConvertible.hpp>
#include <rb/core/traits/Unsigned.hpp>

namespace rb::core {
inline namespace slices {

	template <class T>
	struct RB_EXPORT Slice {
		T const begin;
		T const end;

		constexpr explicit Slice(T end = 0) noexcept
		    : Slice(0, end) {
		}

		constexpr Slice(T begin, T end) noexcept
		    : begin(begin)
		    , end(end) {
		}

		template <class U,
		    RB_REQUIRES_T(IsConvertible<U, T>)>
		constexpr Slice(Slice<U> const& rhs) noexcept // NOLINT(google-explicit-constructor)
		    : begin(static_cast<T>(rhs.begin))
		    , end(static_cast<T>(rhs.end)) {
		}

		constexpr Unsigned<T> size() const noexcept {
			return begin < end ? end - begin : 0;
		}

		[[nodiscard]] constexpr bool empty() const noexcept {
			return size() == 0;
		}
	};

	template <class Char, class Traits, class T>
	std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, Slice<T> const& slice) {
		return os << os.widen('[')
		          << slice.begin
		          << os.widen(',') << os.widen(' ')
		          << slice.end
		          << os.widen(')');
	}

} // namespace slices
} // namespace rb::core
