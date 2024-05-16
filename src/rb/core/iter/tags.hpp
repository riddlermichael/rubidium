#pragma once

namespace rb::core {

inline namespace iter {

	struct InputIteratorTag {};

	struct ForwardIteratorTag : public InputIteratorTag {};

	struct BidirectionalIteratorTag : public ForwardIteratorTag {};

	struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};

	struct ContiguousIteratorTag : public RandomAccessIteratorTag {};

} // namespace iter

namespace impl {

	template <class Tag>
	struct FromStdTagImpl {};

	template <>
	struct FromStdTagImpl<std::input_iterator_tag> {
		using Type = InputIteratorTag;
	};

	template <>
	struct FromStdTagImpl<std::forward_iterator_tag> {
		using Type = ForwardIteratorTag;
	};

	template <>
	struct FromStdTagImpl<std::bidirectional_iterator_tag> {
		using Type = BidirectionalIteratorTag;
	};

	template <>
	struct FromStdTagImpl<std::random_access_iterator_tag> {
		using Type = RandomAccessIteratorTag;
	};

#if __cplusplus > 201703L
	template <>
	struct FromStdTagImpl<std::contiguous_iterator_tag> {
		using Type = ContiguousIteratorTag;
	};
#endif

} // namespace impl

inline namespace iter {

	template <class Tag>
	using FromStdTag = typename impl::FromStdTagImpl<Tag>::Type;

} // namespace iter

} // namespace rb::core
