#pragma once

#include <rb/core/assert.hpp>
#include <rb/core/helpers.hpp>
#include <rb/core/iter/IteratorTraits.hpp>
#include <rb/core/traits/constructible.hpp>
#include <rb/core/traits/declval.hpp>
#include <rb/core/traits/IsBaseOf.hpp>

namespace rb::core {
inline namespace slices {

	template <class It>
	class RB_EXPORT StepIterator final {
		using Traits = IteratorTraits<It>;
		using Self = StepIterator<It>;

		static_assert(isBaseOf<RandomAccessIteratorTag, typename Traits::Category>,
		    "Iterator must be random-access at least");

	public:
		using Category = RandomAccessIteratorTag; // even if the underlying category is ContiguousIteratorTag
		using Difference = typename Traits::Difference;
		using Index = Difference;
		using Pointer = typename Traits::Pointer;
		using Reference = typename Traits::Reference;
		using Value = typename Traits::Value;

		constexpr StepIterator(It it, Index step) noexcept(isNothrowCopyConstructible<It>)
		    : it_(it)
		    , step_(step) {
			RB_ASSERT_MSG("Step must be non-zero", step != 0);
		}

		constexpr Index step() const noexcept {
			return step_;
		}

		// iterator methods

		constexpr decltype(auto) operator*() const RB_NOEXCEPT_LIKE(*it_) {
			return *it_;
		}

		constexpr decltype(auto) operator*() RB_NOEXCEPT_LIKE(*it_) {
			return *it_;
		}

		constexpr Self& operator++() RB_NOEXCEPT_LIKE(it_ += step_) {
			it_ += step_;
			return *this;
		}

		// input iterator methods

		constexpr bool operator==(Self const& rhs) const RB_NOEXCEPT_LIKE(it_ == rhs.it_) {
			return it_ == rhs.it_;
		}

		constexpr bool operator!=(Self const& rhs) const RB_NOEXCEPT_LIKE(it_ == rhs.it_) {
			return !operator==(rhs);
		}

		constexpr bool operator==(It const& rhs) const RB_NOEXCEPT_LIKE(it_ == rhs) {
			return it_ == rhs;
		}

		constexpr bool operator!=(It const& rhs) const RB_NOEXCEPT_LIKE(it_ == rhs) {
			return !operator==(rhs);
		}

		// FIXME primitive types
		constexpr decltype(auto) operator->() const RB_NOEXCEPT_LIKE(it_.operator->()) {
			return it_.operator->();
		}

		constexpr decltype(auto) operator->() RB_NOEXCEPT_LIKE(it_.operator->()) {
			return it_.operator->();
		}

		// forward iterator methods

		constexpr Self operator++(int) RB_NOEXCEPT_LIKE(++RB_DECLVAL(Self&)) {
			Self result = *this;
			++*this;
			return result;
		}

		// bidirectional iterator methods

		constexpr Self& operator--() RB_NOEXCEPT_LIKE(it_ -= step_) {
			it_ -= step_;
			return *this;
		}

		constexpr Self operator--(int) RB_NOEXCEPT_LIKE(--RB_DECLVAL(Self&)) {
			Self result = *this;
			--*this;
			return result;
		}

		// random-access iterator methods

		constexpr Self& operator+=(Index n) RB_NOEXCEPT_LIKE(it_ += n) {
			it_ += n * step_;
			return *this;
		}

		constexpr Self& operator-=(Index n) RB_NOEXCEPT_LIKE(it_ -= n) {
			it_ -= n * step_;
			return *this;
		}

		constexpr Self operator+(Index n) const RB_NOEXCEPT_LIKE(it_ += n) {
			Self result = *this;
			return result += n;
		}

		constexpr Self operator-(Index n) const RB_NOEXCEPT_LIKE(it_ -= n) {
			Self result = *this;
			return result -= n;
		}

		constexpr Index operator-(Self const& rhs) const RB_NOEXCEPT_LIKE(it_ - rhs.it_) {
			Index const n = it_ - rhs.it_;
			RB_ASSERT_MSG("Iterator is unreachable", n % rhs.step_ == 0);
			return n / rhs.step_;
		}

		constexpr Index operator-(It const& it) const RB_NOEXCEPT_LIKE(it_ - it) {
			Index const n = it_ - it;
			RB_ASSERT_MSG("Iterator is unreachable", n % step_ == 0);
			return n / step_;
		}

		constexpr decltype(auto) operator[](Index n) const RB_NOEXCEPT_LIKE(*(it_ + n)) {
			return *(it_ + n * step_);
		}

		constexpr bool operator<(Self const& rhs) const RB_NOEXCEPT_LIKE(it_ < rhs.it_) {
			return it_ < rhs.it_;
		}

		constexpr bool operator<=(Self const& rhs) const RB_NOEXCEPT_LIKE(it_ <= rhs.it_) {
			return it_ <= rhs.it_;
		}

		constexpr bool operator>(Self const& rhs) const RB_NOEXCEPT_LIKE(it_ > rhs.it_) {
			return it_ > rhs.it_;
		}

		constexpr bool operator>=(Self const& rhs) const RB_NOEXCEPT_LIKE(it_ >= rhs.it_) {
			return it_ >= rhs.it_;
		}

		constexpr bool operator<(It const& it) const RB_NOEXCEPT_LIKE(it_ < it) {
			return it_ < it;
		}

		constexpr bool operator<=(It const& it) const RB_NOEXCEPT_LIKE(it_ < it) {
			return it_ <= it;
		}

		constexpr bool operator>(It const& it) const RB_NOEXCEPT_LIKE(it_ < it) {
			return it_ > it;
		}

		constexpr bool operator>=(It const& it) const RB_NOEXCEPT_LIKE(it_ < it) {
			return it_ >= it;
		}

	private:
		It it_;
		Index step_;
	};

	template <class It>
	StepIterator<It> operator+(typename StepIterator<It>::Index n, StepIterator<It> const& stepIterator)
	    RB_NOEXCEPT_LIKE(stepIterator + n) {
		return stepIterator + n;
	}

	template <class It>
	constexpr typename StepIterator<It>::Index operator-(It const& lhs, StepIterator<It> const& rhs)
	    RB_NOEXCEPT_LIKE(rhs - rhs) {
		return StepIterator<It>(lhs, rhs.step()) - rhs; // for clarity, a step of lhs ignored
	}

} // namespace slices
} // namespace rb::core
