#pragma once

#include <ostream>

#include <rb/core/builtins.hpp>
#include <rb/core/meta/TypeSeq.hpp>

namespace rb::core {

inline namespace meta {
	template <auto...>
	struct ValueSeq;
} // namespace meta

namespace impl::vs {

	template <class VS, usize pos>
	struct At;

	template <auto v, auto... vs>
	struct At<ValueSeq<v, vs...>, 0> {
		static constexpr auto kValue = v;
	};

	template <auto v, auto... vs, usize pos>
	struct At<ValueSeq<v, vs...>, pos> {
		static constexpr auto kValue = At<ValueSeq<vs...>, pos - 1>::kValue;
	};

	template <class VS, usize n, bool inRange>
	struct PopFront;

	template <>
	struct PopFront<ValueSeq<>, 0, true> {
		using Type = ValueSeq<>;
	};

	template <auto v, auto... vs>
	struct PopFront<ValueSeq<v, vs...>, 0, true> {
		using Type = ValueSeq<v, vs...>;
	};

	template <auto v, auto... vs, usize n>
	struct PopFront<ValueSeq<v, vs...>, n, true> {
		using Type = typename PopFront<ValueSeq<vs...>, n - 1, true>::Type;
	};

	template <class... Ts>
	struct Join;

	template <>
	struct Join<> {
		using Type = ValueSeq<>;
	};

	template <auto... vs>
	struct Join<ValueSeq<vs...>> {
		using Type = ValueSeq<vs...>;
	};

	template <auto... vs, auto... us, class... Ts>
	struct Join<ValueSeq<vs...>, ValueSeq<us...>, Ts...> {
		using Type = typename Join<ValueSeq<vs..., us...>, Ts...>::Type;
	};

	template <class VS, class UL>
	struct Append;

	template <auto... vs, auto... us>
	struct Append<ValueSeq<vs...>, ValueSeq<us...>> {
		using Type = ValueSeq<vs..., us...>;
	};

	template <class VS, auto v>
	struct IndexOf;

	template <auto v>
	struct IndexOf<ValueSeq<>, v> {
		enum : isize {
			kValue = -1
		};
	};

	template <auto v, auto... vs>
	struct IndexOf<ValueSeq<v, vs...>, v> {
		enum : isize {
			kValue = 0
		};
	};

	template <auto v, auto... vs, auto u>
	struct IndexOf<ValueSeq<v, vs...>, u> {
	private:
		enum : isize {
			// clang-format off
			kTemp = IndexOf<ValueSeq<vs...>, u>::kValue
			// clang-format on
		};

	public:
		enum : isize {
			kValue = kTemp == -1 ? -1 : 1 + kTemp
		};
	};

	template <class VS, auto v>
	struct Erase;

	template <auto v>
	struct Erase<ValueSeq<>, v> {
		using Type = ValueSeq<>;
	};

	template <auto v, auto... vs>
	struct Erase<ValueSeq<v, vs...>, v> {
		using Type = ValueSeq<vs...>;
	};

	template <auto v, auto... vs, auto u>
	struct Erase<ValueSeq<v, vs...>, u> {
		using Type = typename Append<ValueSeq<v>, typename Erase<ValueSeq<vs...>, u>::Type>::Type;
	};

	template <class VS, auto v>
	struct EraseAll;

	template <auto v>
	struct EraseAll<ValueSeq<>, v> {
		using Type = ValueSeq<>;
	};

	template <auto v, auto... vs>
	struct EraseAll<ValueSeq<v, vs...>, v> {
		using Type = typename EraseAll<ValueSeq<vs...>, v>::Type;
	};

	template <auto v, auto... vs, auto u>
	struct EraseAll<ValueSeq<v, vs...>, u> {
		using Type = typename Append<ValueSeq<v>, typename EraseAll<ValueSeq<vs...>, u>::Type>::Type;
	};

	template <class VS, template <auto> class F>
	struct Filter;

	template <template <auto> class F>
	struct Filter<ValueSeq<>, F> {
		using Type = ValueSeq<>;
	};

	template <auto v, auto... vs, template <auto> class F>
	struct Filter<ValueSeq<v, vs...>, F> {
	private:
		using Tail = typename Filter<ValueSeq<vs...>, F>::Type;

	public:
		using Type = Conditional<F<v>{},
		    typename Join<ValueSeq<v>, Tail>::Type,
		    Tail>;
	};

} // namespace impl::vs

inline namespace meta {

	// NOLINTBEGIN(readability-identifier-naming)

	template <auto... vs>
	struct ValueSeq {
		using This = ValueSeq;

		enum : usize {
			kSize = sizeof...(vs)
		};

		template <usize pos>
		static constexpr auto at = impl::vs::At<This, pos>::kValue;

		using Types = TypeSeq<decltype(vs)...>;

		template <usize pos>
		using TypeAt = typename Types::template At<pos>;

		template <usize n = 1>
		using PopFront = typename impl::vs::PopFront<This, n, (n <= kSize)>::Type;

		template <auto... us>
		using PushFront = ValueSeq<us..., vs...>;

		template <auto... us>
		using PushBack = ValueSeq<vs..., us...>;

		template <class VS>
		using Append = typename impl::vs::Append<This, VS>::Type;

		template <auto v>
		static constexpr isize indexOf = impl::vs::IndexOf<This, v>::kValue;

		template <auto v>
		using Erase = typename impl::vs::Erase<This, v>::Type;

		template <auto v>
		using EraseAll = typename impl::vs::EraseAll<This, v>::Type;

		template <template <auto> class F>
		using Filter = typename impl::vs::Filter<This, F>::Type;

		template <auto f>
		using Map = ValueSeq<f(vs)...>;
	};

	// NOLINTEND(readability-identifier-naming)

	template <class T, T... values>
	struct IntegerSeq {
		using Values = ValueSeq<values...>;
	};

#if defined(RB_COMPILER_GCC) || defined(RB_COMPILER_MINGW)
	template <usize n>
	using IndexSeq = typename IntegerSeq<usize, __integer_pack(n)...>::Values;
#elif defined(RB_COMPILER_MSVC) || defined(RB_COMPILER_CLANG)
	template <usize n>
	using IndexSeq = typename __make_integer_seq<IntegerSeq, usize, n>::Values;
#endif

	inline std::ostream& operator<<(std::ostream& os, ValueSeq<> /*unused*/) {
		return os << "[]";
	}

	template <auto v, auto... vs>
	std::ostream& operator<<(std::ostream& os, ValueSeq<v, vs...> /*unused*/) {
		return os << "[" << v, ((os << ", " << vs), ...), os << "]";
	}

} // namespace meta
} // namespace rb::core
