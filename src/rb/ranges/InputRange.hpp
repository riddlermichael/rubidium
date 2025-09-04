#pragma once

#include <rb/core/Option.hpp>
#include <rb/ranges/traits.hpp>

namespace rb::core {

template <class Char, class Traits>
constexpr bool empty(std::basic_istream<Char, Traits>& is) noexcept {
	return is.fail();
}

} // namespace rb::core

namespace rb::ranges {

template <class S, class E,
    RB_REQUIRES(core::isReadableFrom<E, S>&& core::isDefaultConstructible<E>)>
class InputRange final {
public:
	constexpr explicit InputRange(S& s) noexcept
	    : s_(s) {
	}

	[[nodiscard]] constexpr decltype(auto) empty() const RB_NOEXCEPT_LIKE(core::empty(s_)) {
		return core::empty(s_);
	}

	// ReSharper disable once CppMemberFunctionMayBeConst
	constexpr void popFront() RB_NOEXCEPT_LIKE(popFrontImpl()) {
		popFrontImpl();
	}

	constexpr E front() const RB_NOEXCEPT_LIKE(popFrontImpl()) {
		if (!e_) {
			popFrontImpl();
		}
		return *e_;
	}

private:
	constexpr void popFrontImpl() const {
		E e;
		s_ >> e;
		e_ = e;
	}

	S& s_;
	core::Option<E> mutable e_;
};

template <class E, class S,
    RB_REQUIRES(!isInputRange<S>
        && core::isReadableFrom<E, S>
        && core::isDefaultConstructible<E>)>
constexpr auto range(S& s) noexcept {
	return InputRange<S, E>{s};
}

} // namespace rb::ranges
