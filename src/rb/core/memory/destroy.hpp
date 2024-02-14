#pragma once

#include <rb/core/helpers.hpp>
#include <rb/core/memory/addressOf.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/destructible.hpp>

namespace rb::core {
inline namespace memory {

	template <class T,
	    RB_REQUIRES(!isArray<T>)>
	void destroy(T* ptr) noexcept(isNothrowDestructible<T>) {
		ptr->~T();
	}

	template <class T,
	    RB_REQUIRES(isArray<T>)>
	void destroy(T* ptr) RB_NOEXCEPT_LIKE(destroy(addressOf((*ptr)[0]))) {
		for (auto& x : *ptr) {
			destroy(addressOf(x));
		}
	}

	template <class ForwardIt>
	void destroy(ForwardIt first, ForwardIt last) {
		for (; first != last; ++first) {
			destroy(addressOf(*first));
		}
	}

} // namespace memory
} // namespace rb::core
