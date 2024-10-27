#pragma once

#include <rb/core/iter/tags.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/detection.hpp>
#include <rb/core/traits/IsObject.hpp>
#include <rb/core/types.hpp>

namespace rb::core {

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
	template <class It, class = void, class = void>
	struct IteratorTraits {
	};

	template <class It>
	struct IteratorTraits<It,
	    Void<
	        typename It::Category,
	        typename It::Difference,
	        typename It::Pointer,
	        typename It::Reference,
	        typename It::Value>,
	    void> //
	{
		using Category = typename It::Category;
		using Difference = typename It::Difference;
		using Pointer = typename It::Pointer;
		using Reference = typename It::Reference;
		using Value = typename It::Value;
	};

	template <class It, class _>
	struct IteratorTraits<It,
	    _,
	    Void<
	        typename It::iterator_category,
	        typename It::difference_type,
	        typename It::pointer,
	        typename It::reference,
	        typename It::value_type>> //
	{
		using Category = FromStdTag<typename It::iterator_category>;
		using Difference = typename It::difference_type;
		using Pointer = typename It::pointer;
		using Reference = typename It::reference;
		using Value = typename It::value_type;
	};

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

namespace impl {

	template <class T>
	using ValueTypeDetector = typename IteratorTraits<T>::Value;

	template <class T>
	using CategoryDetector = typename IteratorTraits<T>::Category;

} // namespace impl

inline namespace iter {

	template <class T>
	using IsIteratorType = IsDetected<impl::ValueTypeDetector, T>;

	template <class T>
	using IsInputIterator = IsDetectedConvertible<InputIteratorTag, impl::CategoryDetector, T>;

	template <class T>
	using IsForwardIterator = IsDetectedConvertible<ForwardIteratorTag, impl::CategoryDetector, T>;

} // namespace iter

} // namespace rb::core
