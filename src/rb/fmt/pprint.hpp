#pragma once

#include <rb/core/iter/primitives.hpp>
#include <rb/core/meta/ValueSeq.hpp>
#include <rb/core/move.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/declval.hpp>
#include <rb/core/traits/IsTupleLike.hpp>
#include <rb/core/traits/requirements.hpp>

namespace rb::fmt {

template <class Stream, class... Args>
using AreWritableTo = core::And<core::traits::IsWritableTo<Args, Stream>...>;

template <class Stream, class... Args>
inline constexpr bool areWritableTo = AreWritableTo<Stream, Args...>::value;

template <class T, class Stream, class It, class Left, class Right, class Delim,
    RB_REQUIRES_T(AreWritableTo<Stream, Left, Right, Delim, T>)>
Stream& pprintAs(Stream& s, It first, It last, Left left, Right right, Delim delim) {
	if constexpr (core::IsBooleanTestable<Left>::value) {
		if (left) {
			s << left;
		}
	} else {
		s << left;
	}
	if (first != last) {
		s << static_cast<T>(*first++);
		while (first != last) {
			s << delim;
			s << static_cast<T>(*first++);
		}
	}
	if constexpr (core::IsBooleanTestable<Right>::value) {
		if (right) {
			s << right;
		}
	} else {
		s << right;
	}
	return s;
}

template <class T, class Stream, class Container, class Left, class Right, class Delim,
    RB_REQUIRES_T(AreWritableTo<Stream, Left, Right, Delim, T>)>
Stream& pprintAs(Stream& s, Container&& c, Left left, Right right, Delim delim) {
	using core::begin;
	auto first = begin(RB_FWD(c));
	using core::end;
	auto last = end(RB_FWD(c));
	return pprintAs<T>(s, first, last, left, right, delim);
}

template <class Stream, class It, class Left, class Right, class Delim,
    RB_REQUIRES_T(AreWritableTo<Stream, Left, Right, Delim>)>
Stream& pprint(Stream& s, It first, It last, Left left, Right right, Delim delim) {
	return pprintAs<decltype(*RB_DECLVAL(It&))>(s, first, last, left, right, delim);
}

template <class Stream, class Container, class Left, class Right, class Delim,
    RB_REQUIRES(areWritableTo<Stream, Left, Right, Delim>), RB_REQUIRES(!core::isTupleLike<Container>)>
Stream& pprint(Stream& s, Container&& c, Left left, Right right, Delim delim) {
	auto&& first = core::begin(RB_FWD(c));
	auto&& last = core::end(RB_FWD(c));
	return pprint(s, first, last, left, right, delim);
}

namespace impl {

	template <class Stream, class Tuple, usize... indices, class Delim>
	void printTupleWithDelim(Stream& s, Tuple&& t, core::ValueSeq<indices...> is, Delim delim) {
		RB_UNUSED(is);
		(..., (s << delim << std::get<indices>(RB_FWD(t))));
	}

	constexpr usize plusOne(usize x) noexcept {
		return x + 1;
	}

} // namespace impl

template <class Stream, class Container, class Left, class Right, class Delim,
    RB_REQUIRES(areWritableTo<Stream, Left, Right, Delim>), RB_REQUIRES(core::isTupleLike<Container>)>
Stream& pprint(Stream& s, Container&& c, Left left, Right right, Delim delim) {
	using C = core::RemoveCvRef<Container>;
	s << left;
	if constexpr (std::tuple_size_v<C> != 0) {
		s << std::get<0>(RB_FWD(c));
		if constexpr (std::tuple_size_v<C> >= 2) {
			using Indices = typename core::IndexSeq<std::tuple_size_v<C> - 1>::template Map<impl::plusOne>;
			impl::printTupleWithDelim(s, RB_FWD(c), Indices{}, delim);
		}
	}
	return s << right;
}

/**
 * @param f The signature of the functor should be equivalent to the following:
 * `auto f(Stream&, T const&);`
 * The signature does not need to have `const &`.
 * The type `T` must be such that an object of type `It` can be dereferenced and then implicitly converted to `T`.
 */
template <class Stream, class It, class Left, class Right, class Delim, class BinaryFunctor,
    RB_REQUIRES_T(AreWritableTo<Stream, Left, Right, Delim>)>
Stream& pprint(Stream& s, It first, It last, Left left, Right right, Delim delim, BinaryFunctor&& f) {
	s << left;
	if (first != last) {
		(RB_FWD(f))(s, *first++);
		while (first != last) {
			s << delim;
			(RB_FWD(f))(s, *first++);
		}
	}
	s << right;
	return s;
}

template <class Stream, class Container, class Left, class Right, class Delim, class BinaryFunctor,
    RB_REQUIRES_T(AreWritableTo<Stream, Left, Right, Delim>)>
Stream& pprint(Stream& s, Container&& c, Left left, Right right, Delim delim, BinaryFunctor&& f) {
	using core::begin;
	auto first = begin(RB_FWD(c));
	using core::end;
	auto last = end(RB_FWD(c));
	return pprint(s, first, last, left, right, delim, f);
}

} // namespace rb::fmt
