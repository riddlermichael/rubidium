#pragma once

#include <rb/core/slices/Slice.hpp>
#include <rb/core/slices/StepSlice.hpp>
#include <rb/core/slices/ViewImpl.hpp>

namespace rb::core {
inline namespace slices {

	template <
	    class T,
	    class ConstSliceView = ViewImpl<T, false>,
	    class SliceView = ViewImpl<T, true>>
	class RB_EXPORT Sliceable {
		using Traits = ContainerTraits<T>;

	public:
		using Value = typename Traits::Value;
		using Iterator = typename Traits::Iterator;
		using ConstIterator = typename Traits::ConstIterator;
		using Difference = typename Traits::Difference;
		using Size = typename Traits::Size;

		static_assert(isBaseOf<RandomAccessIteratorTag, typename IteratorTraits<Iterator>::Category>,
		    "Iterator must be random-access");

		using SliceT = Slice<Difference>;
		using StepSliceT = StepSlice<Difference>;
		using ConstView = ViewImpl<T, false>;
		using View = ViewImpl<T, true>;

		constexpr ConstSliceView operator[](SliceT slice) const noexcept {
			return subscript<ConstSliceView, false>(this, slice);
		}

		constexpr SliceView operator[](SliceT slice) noexcept {
			return subscript<SliceView, false>(this, slice);
		}

		constexpr ConstView operator[](StepSliceT slice) const noexcept {
			return subscript<ConstView, true>(this, slice);
		}

		constexpr View operator[](StepSliceT slice) noexcept {
			return subscript<View, true>(this, slice);
		}

	private:
		template <class ViewT, bool useStep, class This, class SliceT>
		static constexpr ViewT subscript(This* self, SliceT slice) {
			using X = Conditional<isConst<This>, T const, T>;
			auto const begin = static_cast<X*>(self)->begin();
			auto const end = static_cast<X*>(self)->end();
			auto const first = slice.begin + (slice.begin >= 0 ? begin : end);
			RB_CHECK_RANGE_EXCL(first - begin, 0, end - begin);
			auto const last = slice.end + (slice.end >= 0 ? begin : end);
			RB_CHECK_RANGE_EXCL(last - begin, 0, end - begin);
			if constexpr (useStep) {
				return {first, last, slice.step};
			} else {
				return {first, last};
			}
		}
	};

} // namespace slices
} // namespace rb::core

#define RB_USE_BASE_CONTAINER_TYPES(T) \
	using typename T::ConstIterator;   \
	using typename T::Difference;      \
	using typename T::Iterator;        \
	using typename T::Size;            \
	using typename T::Value;
