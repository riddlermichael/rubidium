#pragma once

#include <ostream>

#include <rb/core/move.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/constructible.hpp>
#include <rb/core/traits/requirements.hpp>

namespace rb::core {
inline namespace memory {

	template <class T, int _ = 0>
	class Wrapper {
		using Self = Wrapper;

		template <class U>
		using NotSelf = Not<IsSame<Self, RemoveCvRef<U>>>;

	public:
		constexpr Wrapper() noexcept(isNothrowDefaultConstructible<T>) = default;

		template <class... Args,
		    RB_REQUIRES(isConstructible<T, Args...>)>
		constexpr explicit Wrapper(Args&&... args) noexcept(isNothrowConstructible<T, Args...>)
		    : value_(RB_FWD(args)...) {
		}

		constexpr T& get() & noexcept {
			return value_;
		}

		constexpr T const& get() const& noexcept {
			return value_;
		}

		constexpr T&& get() && noexcept {
			return value_;
		}

		constexpr T const&& get() const&& noexcept {
			return value_;
		}

	private:
		T value_;
	};

	template <class Char, class Traits, class T, int _,
	    RB_REQUIRES_T(IsWritableTo<T, std::basic_ostream<Char, Traits>>)>
	std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, Wrapper<T, _> const& w) {
		return os << w.get();
	}

} // namespace memory
} // namespace rb::core
