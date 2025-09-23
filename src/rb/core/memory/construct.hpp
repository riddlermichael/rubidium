#pragma once

#include <rb/core/traits/constructible.hpp>
#include <rb/core/traits/declval.hpp>

namespace rb::core {
inline namespace memory {

	template <class T, class... Args>
	constexpr auto construct(T* ptr, Args&&... args) noexcept(isNothrowConstructible<T, Args...>)
	    -> decltype(::new(RB_DECLVAL(void*)) T(RB_DECLVAL(Args)...)) {
		return ::new (static_cast<void*>(ptr)) T(RB_FWD(args)...);
	}

} // namespace memory
} // namespace rb::core
