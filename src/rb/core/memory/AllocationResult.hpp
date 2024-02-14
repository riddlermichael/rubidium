#pragma once

#include <ostream>

#include <rb/core/traits/requirements.hpp>
#include <rb/core/types.hpp>

namespace rb::core {
inline namespace memory {

	/// AllocationResult specializations are return types of the `allocateAtLeast` member function
	/// of appropriate [Allocator](https://en.cppreference.com/w/cpp/named_req/Allocator) types.
	/// Twin of `std::allocation_result` (from C++23).
	/// Every specialization of AllocationResult has no base classes or declared members other than @a ptr and @a count,
	/// thus it is suitable for aggregate initialization and structured binding.
	template <class Pointer, class Size = usize>
	struct AllocationResult {
		Pointer ptr;
		Size count;
	};

	template <class Char, class Traits, class Pointer, class Size,
	    RB_REQUIRES_T(And<
	        IsWritableTo<Pointer, std::basic_ostream<Char, Traits>>,
	        IsWritableTo<Size, std::basic_ostream<Char, Traits>>>)>
	std::basic_ostream<Char, Traits>& operator<<(
	    std::basic_ostream<Char, Traits>& os,
	    AllocationResult<Pointer, Size> result) {
		return os << os.widen('{') << result.ptr << os.widen(',') << os.widen(' ') << result.count << os.widen('}');
	}

} // namespace memory
} // namespace rb::core
