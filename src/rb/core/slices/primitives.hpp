#pragma once

#include <rb/core/helpers.hpp>
#include <rb/core/move.hpp>
#include <rb/core/slices/Slice.hpp>
#include <rb/core/slices/StepSlice.hpp>
#include <rb/core/traits/CommonType.hpp>

namespace rb::core {
inline namespace slices {

	template <class T = isize>
	RB_EXPORT constexpr auto slice() noexcept {
		return Slice<T>{};
	}

	template <class T>
	RB_EXPORT constexpr auto slice(T end) noexcept {
		return Slice<T>{end};
	}

	template <class T, class U>
	RB_EXPORT constexpr auto slice(T begin, U end) noexcept {
		return Slice<CommonType<T, U>>(begin, end);
	}

	template <class T, class U, class V>
	RB_EXPORT constexpr auto slice(T begin, U end, V step) noexcept {
		return StepSlice<CommonType<T, U, V>>(begin, end, step);
	}

	template <class T>
	RB_EXPORT constexpr decltype(auto) all(T&& value) RB_NOEXCEPT_LIKE(RB_FWD(value)[slice(RB_FWD(value).size())]) {
		return RB_FWD(value)[slice(RB_FWD(value).size())];
	}

} // namespace slices
} // namespace rb::core
