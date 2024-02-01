#pragma once

#include <rb/core/helpers.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/alwaysFalse.hpp>
#include <rb/core/traits/IsFunction.hpp>
#include <rb/core/traits/IsPointer.hpp>

namespace rb::core {
inline namespace memory {

	struct DefaultDeleter {
		template <class P,
		    RB_REQUIRES(isPointer<P> && !isFunction<RemovePointer<P>>)>
		constexpr void operator()(P ptr) const RB_NOEXCEPT_LIKE(delete ptr) {
			using T = RemovePointer<P>;
			static_assert(!isVoid<T>, "can't delete pointer to void");
			RB_CHECK_COMPLETENESS(T);
			delete ptr;
		}

		template <class T, usize n>
		constexpr void operator()(T (&a)[n]) const noexcept {
			RB_UNUSED(a);
			static_assert(alwaysFalse<T>, "delete of an array of known bounds");
		}

		template <class T, usize n>
		constexpr void operator()(T (&&a)[n]) const noexcept {
			RB_UNUSED(a);
			static_assert(alwaysFalse<T>, "delete of an array of known bounds");
		}

		template <class T>
		constexpr void operator()(T (&a)[]) const noexcept {
			RB_UNUSED(a);
			static_assert(alwaysFalse<T>, "delete of an array of unknown bounds");
		}

		template <class T>
		constexpr void operator()(T (&&a)[]) const noexcept {
			RB_UNUSED(a);
			static_assert(alwaysFalse<T>, "delete of an array of unknown bounds");
		}
	};

} // namespace memory
} // namespace rb::core
