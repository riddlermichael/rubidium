#pragma once

#include <rb/core/helpers.hpp>
#include <rb/core/memory/AllocationResult.hpp>

namespace rb::core {
inline namespace memory {

	template <class T>
	struct Allocator {
		using Value = T;
		using Size = usize;
		using Difference = isize;

		[[nodiscard]] constexpr T* allocate(Size n) {
			RB_CHECK_COMPLETENESS(T);
			return static_cast<T*>(::operator new(n * sizeof(Value)));
		}

		[[nodiscard]] constexpr AllocationResult<T*> allocateAtLeast(Size n) {
			return {allocate(n), n};
		}

		constexpr void deallocate(T* ptr, Size n) {
			RB_UNUSED(n);
			::operator delete(ptr);
		}
	};

} // namespace memory
} // namespace rb::core
