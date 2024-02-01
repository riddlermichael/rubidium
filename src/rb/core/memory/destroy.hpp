#pragma once

#include <memory>

#include <rb/core/helpers.hpp>
#include <rb/core/memory/addressOf.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/destructible.hpp>

namespace rb::core {
inline namespace memory {

	template <class T,
	    RB_REQUIRES(!isArray<T>)>
	void destroyAt(T* ptr) noexcept(isNothrowDestructible<T>) {
		ptr->~T();
	}

	template <class T,
	    RB_REQUIRES(isArray<T>)>
	void destroyAt(T* ptr) RB_NOEXCEPT_LIKE(destroyAt(addressOf((*ptr)[0]))) {
		for (auto& x : *ptr) {
			destroyAt(addressOf(x));
		}
	}

	template <class ForwardIt>
	void destroy(ForwardIt first, ForwardIt last) {
		for (; first != last; ++first) {
			destroyAt(addressOf(*first));
		}
	}

	template <class ForwardIt, class Alloc>
	void destroy(ForwardIt first, ForwardIt last, Alloc& alloc) {
		for (; first != last; ++first) {
			std::allocator_traits<Alloc>::destroy(alloc, addressOf(*first));
		}
	}

} // namespace memory
} // namespace rb::core
