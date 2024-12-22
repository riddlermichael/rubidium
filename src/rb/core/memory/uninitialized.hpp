#pragma once

#include <rb/core/iter/IteratorTraits.hpp>
#include <rb/core/memory/addressOf.hpp>
#include <rb/core/memory/destroy.hpp>

namespace rb::core {
inline namespace memory {

	template <class InputIt, class ForwardIt,
	    RB_REQUIRES_T(And<IsInputIterator<InputIt>, IsForwardIterator<ForwardIt>>)>
	ForwardIt uninitializedCopy(InputIt first, InputIt last, ForwardIt it) {
		using Value = typename IteratorTraits<ForwardIt>::Value;
		ForwardIt current = it;
		try {
			for (; first != last; ++first) {
				::new (static_cast<void*>(addressOf(*current))) Value(*first);
				++current;
			}
			return current;
		} catch (...) {
			destroy(it, current);
			throw;
		}
	}

	template <class InputIt, class ForwardIt,
	    RB_REQUIRES_T(And<IsInputIterator<InputIt>, IsForwardIterator<ForwardIt>>)>
	ForwardIt uninitializedMove(InputIt first, InputIt last, ForwardIt it) {
		using Value = typename IteratorTraits<ForwardIt>::Value;
		ForwardIt current = it;
		try {
			for (; first != last; ++first) {
				::new (static_cast<void*>(addressOf(*current))) Value(RB_MOVE(*first));
				++current;
			}
			return current;
		} catch (...) {
			destroy(it, current);
			throw;
		}
	}

	template <class ForwardIt, class Size, class T,
	    RB_REQUIRES_T(IsForwardIterator<ForwardIt>)>
	ForwardIt uninitializedFillN(ForwardIt first, Size count, T const& value) {
		using Value = typename IteratorTraits<ForwardIt>::Value;
		ForwardIt current = first;
		try {
			for (; count > 0; ++current) {
				::new (static_cast<void*>(addressOf(*current))) Value(value);
				--count;
			}
			return current;
		} catch (...) {
			destroy(first, current);
			throw;
		}
	}

	template <class ForwardIt, class Size,
	    RB_REQUIRES_T(IsForwardIterator<ForwardIt>)>
	ForwardIt uninitializedValueConstructN(ForwardIt first, Size count) {
		using Value = typename IteratorTraits<ForwardIt>::Value;
		ForwardIt current = first;
		try {
			for (; count > 0; ++current) {
				::new (static_cast<void*>(addressOf(*current))) Value();
				--count;
			}
			return current;
		} catch (...) {
			destroy(first, current);
			throw;
		}
	}

} // namespace memory
} // namespace rb::core
