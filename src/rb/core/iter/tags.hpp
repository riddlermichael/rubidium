#pragma once

namespace rb::core {
inline namespace iter {

	struct InputIteratorTag {};

	struct ForwardIteratorTag : public InputIteratorTag {};

	struct BidirectionalIteratorTag : public ForwardIteratorTag {};

	struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};

	struct ContiguousIteratorTag : public RandomAccessIteratorTag {};

} // namespace iter
} // namespace rb::core
