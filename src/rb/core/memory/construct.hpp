#pragma once

#include <rb/core/helpers.hpp>
#include <rb/core/move.hpp>
#include <rb/core/traits/declval.hpp>

namespace rb::core {
inline namespace memory {

	template <class T, class... Args>
	constexpr auto construct(T* ptr, Args&&... args)
	    RB_NOEXCEPT_LIKE(::new(RB_DECLVAL(void*)) T(RB_DECLVAL(Args)...))
	        -> decltype(::new(RB_DECLVAL(void*)) T(RB_DECLVAL(Args)...)) {
		return ::new (static_cast<void*>(ptr)) T(RB_FWD(args)...);
	}

} // namespace memory
} // namespace rb::core
