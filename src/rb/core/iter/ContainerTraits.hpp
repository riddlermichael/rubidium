#pragma once

namespace rb::core {

namespace impl {

	template <class C, class = void, class = void>
	struct ContainerTraits {
	};

	template <class C>
	struct ContainerTraits<C,
	    Void<
	        typename C::ConstIterator,
	        typename C::Difference,
	        typename C::Iterator,
	        typename C::Size,
	        typename C::Value>,
	    void> //
	{
		using ConstIterator = typename C::ConstIterator;
		using Difference = typename C::Difference;
		using Iterator = typename C::Iterator;
		using Size = typename C::Size;
		using Value = typename C::Value;
	};

	template <class C, class _>
	struct ContainerTraits<C,
	    _,
	    Void<
	        typename C::const_iterator,
	        typename C::difference_type,
	        typename C::iterator,
	        typename C::size_type,
	        typename C::value_type>> //
	{
		using ConstIterator = typename C::const_iterator;
		using Difference = typename C::difference_type;
		using Iterator = typename C::iterator;
		using Size = typename C::size_type;
		using Value = typename C::value_type;
	};

} // namespace impl

inline namespace iter {

	template <class C>
	struct ContainerTraits : impl::ContainerTraits<C> {};

} // namespace iter

} // namespace rb::core
