#pragma once

#include <rb/core/requires.hpp>
#include <rb/core/traits/IsArithmetic.hpp>
#include <rb/core/traits/IsPointer.hpp>

namespace rb::ext {

namespace impl {

	template <class T, class = void>
	struct Nullable : core::False {
		static constexpr bool kDefaultIsNull = false;

		static constexpr bool isNull(T const& /*value*/) noexcept {
			return false;
		}
	};

	template <class T>
	struct Nullable<T, core::EnableIf<core::isArithmetic<T>>> : core::True {
		static constexpr bool kDefaultIsNull = true;

		static constexpr bool isNull(T const& value) noexcept {
			return value == 0;
		}

		static constexpr T null() noexcept {
			return 0;
		}
	};

	template <class T>
	struct Nullable<T, core::EnableIf<core::isPointer<T>>> : core::True {
		static constexpr bool kDefaultIsNull = true;

		static constexpr bool isNull(T const& value) noexcept {
			return value == nullptr;
		}

		static constexpr T null() noexcept {
			return nullptr;
		}
	};

} // namespace impl

template <class T>
using Nullable = impl::Nullable<T>; // prevent accidental specialization

} // namespace rb::ext
