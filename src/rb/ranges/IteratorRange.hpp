#pragma once

#include <rb/ranges/range.hpp>

namespace rb::ranges {

namespace impl {

	template <class It, class End, class IteratorCategory>
	class IteratorRange;

	template <class It, class End>
	class IteratorRange<It, End, std::input_iterator_tag> {
	public:
		constexpr IteratorRange() //
		    noexcept(core::isNothrowDefaultConstructible<It>&& core::isNothrowDefaultConstructible<End>) = default;

		constexpr IteratorRange(It begin, End end) //
		    noexcept(core::isNothrowMoveConstructible<It>&& core::isNothrowMoveConstructible<End>)
		    : begin_(RB_MOVE(begin))
		    , end_(RB_MOVE(end)) {
		}

		constexpr decltype(auto) empty() const RB_NOEXCEPT_LIKE(begin_ == end_) {
			return begin_ == end_;
		}

		constexpr void popFront() RB_NOEXCEPT_LIKE(++begin_) {
			++begin_;
		}

		constexpr decltype(auto) front() const RB_NOEXCEPT_LIKE(*begin_) {
			return *begin_;
		}

		constexpr It const& begin() const noexcept {
			return begin_;
		}

		constexpr It& begin() noexcept {
			return begin_;
		}

		constexpr End const& end() const noexcept {
			return end_;
		}

		constexpr End& end() noexcept {
			return end_;
		}

	private:
		It begin_;
		End end_;
	};

	template <class It, class End>
	class IteratorRange<It, End, std::forward_iterator_tag>
	    : public IteratorRange<It, End, std::input_iterator_tag> {
	public:
		using Super = IteratorRange<It, End, std::input_iterator_tag>;
		using Self = IteratorRange;

		using Super::Super;

		// save() defined as non-class function below
	};

	template <class It, class End>
	class IteratorRange<It, End, std::bidirectional_iterator_tag>
	    : public IteratorRange<It, End, std::forward_iterator_tag> {
	public:
		using Super = IteratorRange<It, End, std::forward_iterator_tag>;
		using Self = IteratorRange;

		using Super::Super;

		constexpr void popBack() RB_NOEXCEPT_LIKE(--Super::end()) {
			--Super::end();
		}

		constexpr decltype(auto) back() const RB_NOEXCEPT_LIKE(*Super::end()) {
			auto it = Super::end();
			return *--it;
		}
	};

	template <class It, class End>
	class IteratorRange<It, End, std::random_access_iterator_tag>
	    : public IteratorRange<It, End, std::bidirectional_iterator_tag> {
	public:
		using Super = IteratorRange<It, End, std::bidirectional_iterator_tag>;
		using Self = IteratorRange;
		using Index = typename std::iterator_traits<It>::difference_type;

		using Super::Super;

		constexpr decltype(auto) operator[](Index pos) const RB_NOEXCEPT_LIKE(Super::begin()[pos]) {
			return Super::begin()[pos];
		}

		constexpr auto size() const RB_NOEXCEPT_LIKE(Super::end() - Super::begin()) {
			return Super::end() - Super::begin();
		}
	};

#if __cplusplus > 201703L
	template <class It, class End>
	class IteratorRange<It, End, std::contiguous_iterator_tag>
	    : public IteratorRange<It, End, std::random_access_iterator_tag> {
	public:
		using Super = IteratorRange<It, End, std::random_access_iterator_tag>;
		using Self = IteratorRange;

		using Super::Super;
	};
#endif

	template <class It, class End>
	class IteratorRange<It, End, std::output_iterator_tag> {
	public:
		constexpr IteratorRange() //
		    noexcept(core::isNothrowDefaultConstructible<It>&& core::isNothrowDefaultConstructible<End>) = default;

		constexpr IteratorRange(It begin, End end) //
		    noexcept(core::isNothrowMoveConstructible<It>&& core::isNothrowMoveConstructible<End>)
		    : begin_(RB_MOVE(begin))
		    , end_(RB_MOVE(end)) {
		}

		template <class E,
		    RB_REQUIRES_T(core::IsAssignable<decltype(*RB_DECLVAL(It&)), E>)>
		constexpr void put(E&& e) RB_NOEXCEPT_LIKE(*(this->begin_)++ = RB_FWD(e)) {
			*begin_++ = RB_FWD(e);
		}

		// begin/end

		constexpr It const& begin() const noexcept {
			return begin_;
		}

		constexpr It& begin() noexcept {
			return begin_;
		}

		constexpr End const& end() const noexcept {
			return end_;
		}

		constexpr End& end() noexcept {
			return end_;
		}

	private:
		It begin_;
		End end_;
	};

} // namespace impl

template <class It, class End = It>
struct IteratorRange final
    : public impl::IteratorRange<It, End, typename std::iterator_traits<It>::iterator_category> {
public:
	using Super = impl::IteratorRange<It, End, typename std::iterator_traits<It>::iterator_category>;
	using Self = IteratorRange;

	using Super::Super;
};

template <class It, class End>
IteratorRange(It, End) -> IteratorRange<It, End>;

// return type of `save` must be equal to the type of range itself;
// impl inside IteratorRange would require CRTP, which is superfluous
template <class It, class End,
    RB_REQUIRES(!core::isSame<typename std::iterator_traits<It>::iterator_category, std::input_iterator_tag>)>
constexpr auto save(IteratorRange<It, End> const& r) noexcept(core::isNothrowCopyConstructible<IteratorRange<It, End>>) {
	return r;
}

template <class C,
    RB_REQUIRES_T(core::And<core::Not<IsInputRange<C>>, core::Not<HasRangeMethod<C>>,
        core::HasBegin<C>, core::HasEnd<C>>)>
constexpr auto range(C& c) RB_NOEXCEPT_LIKE(IteratorRange{core::begin(c), core::end(c)}) {
	return IteratorRange{core::begin(c), core::end(c)};
}

template <class It, class End = It,
    RB_REQUIRES_T(core::IsIteratorType<It>)>
constexpr auto range(It&& it, End&& end) RB_NOEXCEPT_LIKE(IteratorRange{RB_FWD(it), RB_FWD(end)}) {
	return IteratorRange{RB_FWD(it), RB_FWD(end)};
}

} // namespace rb::ranges
