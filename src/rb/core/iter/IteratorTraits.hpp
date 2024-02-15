#pragma once

#include <rb/core/iter/tags.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/IsObject.hpp>
#include <rb/core/traits/Void.hpp>
#include <rb/core/types.hpp>

namespace rb::core {

namespace impl {

	template <class It, class = void>
	struct IteratorTraitsImpl {
	};

	template <class It>
	struct IteratorTraitsImpl<It,
	    Void<
	        typename It::Category,
	        typename It::Difference,
	        typename It::Pointer,
	        typename It::Reference,
	        typename It::Value>> {
		using Category = typename It::Category;
		using Difference = typename It::Difference;
		using Pointer = typename It::Pointer;
		using Reference = typename It::Reference;
		using Value = typename It::Value;
	};

} // namespace impl

inline namespace iter {

	/**
	 * IteratorTraits is the type trait class that provides uniform interface to the properties
	 * of [LegacyIterator](https://en.cppreference.com/w/cpp/named_req/Iterator) types.
	 * This makes it possible to implement algorithms only in terms of iterators.
	 *
	 * The template can be specialized for user-defined iterators so that the information about the iterator
	 * can be retrieved even if the type does not provide the usual <tt>typedef</tt>s.
	 *
	 * @tparam It   the iterator type to retrieve properties for
	 */
	template <class It, class = void>
	struct IteratorTraits : impl::IteratorTraitsImpl<It> {};

	template <class T>
	struct IteratorTraits<T*, EnableIf<isObject<T>>> {
		using Category = RandomAccessIteratorTag;
		using Concept = ContiguousIteratorTag;
		using Difference = isize;
		using Pointer = T*;
		using Reference = T&;
		using Value = RemoveCv<T>;
	};

} // namespace iter

} // namespace rb::core
