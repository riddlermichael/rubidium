#pragma once

#include <rb/core/iter/ContainerTraits.hpp>
#include <rb/core/slices/StepIterator.hpp>
#include <rb/core/traits/Conditional.hpp>

namespace rb::core {
inline namespace slices {

	/// @tparam T type of object being viewed
	/// @tparam isMutable is this view mutable
	template <class T, bool isMutable>
	class RB_EXPORT ViewImpl final {
		using Traits = ContainerTraits<T>;

	public:
		using Value = typename Traits::Value;
		using ConstIt = typename Traits::ConstIterator;
		using Iterator = Conditional<isMutable, typename Traits::Iterator, ConstIt>;
		using Difference = typename Traits::Difference;
		using Size = typename Traits::Size;

		using Index = Difference;
		using ConstStepIt = StepIterator<ConstIt>;
		using StepIt = StepIterator<Iterator>;

		constexpr ViewImpl(Iterator const& begin, Iterator const& end, Index step = 1) RB_NOEXCEPT_LIKE(Iterator(begin))
		    : begin_(begin)
		    , end_(end)
		    , step_(step) {
			RB_ASSERT_MSG("Step must be non-zero", step != 0);
			auto const sz = size();
			end_ = begin_ + sz * step_; // 'range-based for' uses `begin != end`, not `begin < end`
		}

		constexpr auto const& operator[](Index index) const RB_NOEXCEPT_LIKE(begin_[index]) {
			return begin_[index * step_];
		}

		constexpr auto& operator[](Index index) RB_NOEXCEPT_LIKE(begin_[index]) {
			return begin_[index * step_];
		}

		constexpr ConstStepIt begin() const RB_NOEXCEPT_LIKE(ConstStepIt(begin_, step_)) {
			return ConstStepIt{begin_, step_};
		}

		constexpr StepIt begin() RB_NOEXCEPT_LIKE(StepIt(begin_, step_)) {
			return StepIt{begin_, step_};
		}

		constexpr ConstStepIt end() const RB_NOEXCEPT_LIKE(ConstStepIt(end_, step_)) {
			return ConstStepIt{end_, step_};
		}

		constexpr StepIt end() RB_NOEXCEPT_LIKE(StepIt(end_, step_)) {
			return StepIt{end_, step_};
		}

		constexpr Size size() const noexcept {
			auto const n = end_ - begin_;
			if (n == 0) {
				return 0;
			}

			if (n > 0) {
				return step_ > 0 ? (1 + (n - 1) / step_) : 0;
			}

			return step_ > 0 ? 0 : (1 + (-n - 1) / (-step_));
		}

		[[nodiscard]] constexpr bool empty() const noexcept {
			return size() == 0;
		}

		template <class C>
		constexpr C to() const RB_NOEXCEPT_LIKE(C(begin(), end())) {
			return C(begin(), end());
		}

		template <template <class> class C>
		constexpr C<Value> to() const RB_NOEXCEPT_LIKE(C<Value>(begin(), end())) {
			return C<Value>(begin(), end());
		}

	private:
		Iterator begin_;
		Iterator end_;
		Index step_;
	};

} // namespace slices
} // namespace rb::core
