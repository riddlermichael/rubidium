#pragma once

#include <ostream>

#include <rb/core/requires.hpp>
#include <rb/core/traits/IsBaseOf.hpp>
#include <rb/core/traits/ops.hpp>
#include <rb/core/types.hpp>

namespace rb::core {

inline namespace meta {
	template <class... Ts>
	struct TypeSeq;
} // namespace meta

namespace impl::ts {
	//
	template <class TS, usize idx>
	struct At;

	template <class T, class... Ts>
	struct At<TypeSeq<T, Ts...>, 0> {
		using Type = T;
	};

	template <class T, class... Ts, usize idx>
	struct At<TypeSeq<T, Ts...>, idx> {
		using Type = typename At<TypeSeq<Ts...>, idx - 1>::Type;
	};

	//
	template <class TS, usize n>
	struct PopFront;

	template <>
	struct PopFront<TypeSeq<>, 0> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts>
	struct PopFront<TypeSeq<T, Ts...>, 0> {
		using Type = TypeSeq<T, Ts...>;
	};

	template <class T, class... Ts, usize n>
	struct PopFront<TypeSeq<T, Ts...>, n> {
		using Type = typename PopFront<TypeSeq<Ts...>, n - 1>::Type;
	};

	//
	template <class... Ts>
	struct Join;

	template <>
	struct Join<> {
		using Type = TypeSeq<>;
	};

	template <class... Ts>
	struct Join<TypeSeq<Ts...>> {
		using Type = TypeSeq<Ts...>;
	};

	template <class... Ts, class... Us, class... Vs>
	struct Join<TypeSeq<Ts...>, TypeSeq<Us...>, Vs...> {
		using Type = typename Join<TypeSeq<Ts..., Us...>, Vs...>::Type;
	};

	//
	template <class T>
	struct Flatten {
		using Type = TypeSeq<T>;
	};

	template <class... Ts>
	struct Flatten<TypeSeq<Ts...>> {
		using Type = typename Join<typename Flatten<Ts>::Type...>::Type;
	};

	//
	template <class This, class TS>
	struct Append;

	template <class... Ts, class... Us>
	struct Append<TypeSeq<Ts...>, TypeSeq<Us...>> {
		using Type = TypeSeq<Ts..., Us...>;
	};

	//
	template <class TS, class T>
	struct IndexOf;

	template <class T>
	struct IndexOf<TypeSeq<>, T> {
		enum : isize {
			kValue = -1
		};
	};

	template <class T, class... Ts>
	struct IndexOf<TypeSeq<T, Ts...>, T> {
		enum : isize {
			kValue = 0
		};
	};

	template <class T, class... Ts, class U>
	struct IndexOf<TypeSeq<T, Ts...>, U> {
	private:
		enum : isize {
			// clang-format off
			kTemp = IndexOf<TypeSeq<Ts...>, U>::kValue
			// clang-format on
		};

	public:
		enum : isize {
			kValue = kTemp == -1 ? -1 : 1 + kTemp
		};
	};

	//
	template <class TS, class T>
	struct Erase;

	template <class T>
	struct Erase<TypeSeq<>, T> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts>
	struct Erase<TypeSeq<T, Ts...>, T> {
		using Type = TypeSeq<Ts...>;
	};

	template <class T, class... Ts, class U>
	struct Erase<TypeSeq<T, Ts...>, U> {
		using Tail = typename Erase<TypeSeq<Ts...>, U>::Type;
		using Type = typename Tail::template PushFront<T>;
	};

	//
	template <class TS, usize pos>
	struct EraseAt;

	template <class... Ts, usize pos>
	struct EraseAt<TypeSeq<Ts...>, pos> {
		using TS = TypeSeq<Ts...>;
		using Head = typename TS::template Take<pos>;
		using Tail = typename TS::template Slice<pos + 1, sizeof...(Ts)>;
		using Type = typename Join<Head, Tail>::Type;
	};

	//
	template <class TS, usize from, usize to>
	struct EraseRange;

	template <class... Ts, usize from, usize to>
	struct EraseRange<TypeSeq<Ts...>, from, to> {
		using TS = TypeSeq<Ts...>;
		using Head = typename TS::template Take<from>;
		using Tail = typename TS::template Slice<to, sizeof...(Ts)>;
		using Type = typename Join<Head, Tail>::Type;
	};

	//
	template <class TS, class T>
	struct EraseAll;

	template <class T>
	struct EraseAll<TypeSeq<>, T> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts>
	struct EraseAll<TypeSeq<T, Ts...>, T> {
		using Type = typename EraseAll<TypeSeq<Ts...>, T>::Type;
	};

	template <class T, class... Ts, class U>
	struct EraseAll<TypeSeq<T, Ts...>, U> {
		using Tail = typename EraseAll<TypeSeq<Ts...>, U>::Type;
		using Type = typename Tail::template PushFront<T>;
	};

	// EraseIf
	template <class TS, template <class> class UnaryPredicate>
	struct EraseIf;

	template <template <class> class UnaryPredicate>
	struct EraseIf<TypeSeq<>, UnaryPredicate> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts, template <class> class UnaryPredicate>
	struct EraseIf<TypeSeq<T, Ts...>, UnaryPredicate> {
		using Tail = typename EraseIf<TypeSeq<Ts...>, UnaryPredicate>::Type;
		using Type = Conditional<UnaryPredicate<T>{}, Tail, typename Tail::template PushFront<T>>;
	};

	//
	template <class TS, template <class> class F>
	struct Filter;

	template <template <class> class F>
	struct Filter<TypeSeq<>, F> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts, template <class> class F>
	struct Filter<TypeSeq<T, Ts...>, F> {
	private:
		using Tail = typename Filter<TypeSeq<Ts...>, F>::Type;

	public:
		using Type = Conditional<F<T>{}, typename Join<TypeSeq<T>, Tail>::Type, Tail>;
	};

	//
	template <class TS, class T>
	struct PushBack;

	template <class... Ts, class T>
	struct PushBack<TypeSeq<Ts...>, T> {
		using Type = TypeSeq<Ts..., T>;
	};

	//
	template <class T, usize count>
	struct Repeat;

	template <class T>
	struct Repeat<T, 0> {
		using Type = TypeSeq<>;
	};

	template <class T, usize count>
	struct Repeat {
		using Type = typename PushBack<typename Repeat<T, count - 1>::Type, T>::Type;
	};

	//
	template <class TS>
	struct Back;

	template <class T>
	struct Back<TypeSeq<T>> {
		using Type = T;
	};

	template <class T, class... Ts>
	struct Back<TypeSeq<T, Ts...>> {
		using Type = typename Back<TypeSeq<Ts...>>::Type;
	};

	//
	template <class TS>
	struct PopBack;

	template <>
	struct PopBack<TypeSeq<>> {
		using Type = TypeSeq<>;
	};

	template <class T>
	struct PopBack<TypeSeq<T>> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts>
	struct PopBack<TypeSeq<T, Ts...>> {
		using Tail = typename PopBack<TypeSeq<Ts...>>::Type;
		using Type = typename Tail::template PushFront<T>;
	};

	//
	template <class TS>
	struct Reverse;

	template <>
	struct Reverse<TypeSeq<>> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts>
	struct Reverse<TypeSeq<T, Ts...>> {
		using Head = typename Reverse<TypeSeq<Ts...>>::Type;
		using Type = typename Head::template PushBack<T>;
	};

	//
	template <class TS>
	struct Unique;

	template <>
	struct Unique<TypeSeq<>> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts>
	struct Unique<TypeSeq<T, Ts...>> {
		using Tail = typename Unique<TypeSeq<Ts...>>::Type;
		using UniqueTail = typename Tail::template Erase<T>;
		using Type = typename UniqueTail::template PushFront<T>;
	};

	//
	template <class TS, template <class, class> class BinaryPredicate>
	struct UniqueIf;

	template <template <class, class> class BinaryPredicate>
	struct UniqueIf<TypeSeq<>, BinaryPredicate> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts, template <class, class> class BinaryPredicate>
	struct UniqueIf<TypeSeq<T, Ts...>, BinaryPredicate> {
		template <class _1>
		using UnaryPredicate = BinaryPredicate<T, _1>;

		using Tail = typename UniqueIf<TypeSeq<Ts...>, BinaryPredicate>::Type;
		using UniqueTail = typename Tail::template EraseIf<UnaryPredicate>;
		using Type = typename UniqueTail::template PushFront<T>;
	};

	//
	template <class TS, class T, class U>
	struct Replace;

	template <class T, class U>
	struct Replace<TypeSeq<>, T, U> {
		using Type = TypeSeq<>;
	};

	template <class... Ts, class T, class U>
	struct Replace<TypeSeq<T, Ts...>, T, U> {
		using Type = TypeSeq<U, Ts...>;
	};

	template <class V, class... Ts, class T, class U>
	struct Replace<TypeSeq<V, Ts...>, T, U> {
		using Tail = typename Replace<TypeSeq<Ts...>, T, U>::Type;
		using Type = typename Tail::template PushFront<V>;
	};

	//
	template <class TS, class T, class U>
	struct ReplaceAll;

	template <class T, class U>
	struct ReplaceAll<TypeSeq<>, T, U> {
		using Type = TypeSeq<>;
	};

	template <class... Ts, class T, class U>
	struct ReplaceAll<TypeSeq<T, Ts...>, T, U> {
		using Tail = typename ReplaceAll<TypeSeq<Ts...>, T, U>::Type;
		using Type = typename Tail::template PushFront<U>;
	};

	template <class V, class... Ts, class T, class U>
	struct ReplaceAll<TypeSeq<V, Ts...>, T, U> {
		using Tail = typename ReplaceAll<TypeSeq<Ts...>, T, U>::Type;
		using Type = typename Tail::template PushFront<V>;
	};

	//
	template <class TS, usize count>
	struct Take;

	template <class... Ts>
	struct Take<TypeSeq<Ts...>, 0> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts>
	struct Take<TypeSeq<T, Ts...>, 0> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts, usize count>
	struct Take<TypeSeq<T, Ts...>, count> {
		using Tail = typename Take<TypeSeq<Ts...>, count - 1>::Type;
		using Type = typename Tail::template PushFront<T>;
	};

	//
	template <class TS, usize from, usize count, bool isValid>
	struct Slice;

	template <class... Ts, usize from, usize count>
	struct Slice<TypeSeq<Ts...>, from, count, true> {
		using Tail = typename TypeSeq<Ts...>::template PopFront<from>;
		using Type = typename Take<Tail, count>::Type;
	};

	//
	template <class TS, class T, usize count, bool shrink>
	struct Resize;

	template <class... Ts, class T, usize count>
	struct Resize<TypeSeq<Ts...>, T, count, true> {
		using Type = typename Take<TypeSeq<Ts...>, count>::Type;
	};

	template <class... Ts, class T, usize count>
	struct Resize<TypeSeq<Ts...>, T, count, false> {
		using Tail = typename Repeat<T, count - sizeof...(Ts)>::Type;
		using Type = typename TypeSeq<Ts...>::template Append<Tail>;
	};

	//
	template <class TS, class T>
	struct MostDerived;

	template <class T>
	struct MostDerived<TypeSeq<>, T> {
		using Type = T;
	};

	template <class T, class... Ts, class U>
	struct MostDerived<TypeSeq<T, Ts...>, U> {
		using Tail = typename MostDerived<TypeSeq<Ts...>, U>::Type;
		using Type = Conditional<isBaseOf<Tail, T>, T, Tail>;
	};

	//
	template <class TS>
	struct DerivedToFront;

	template <>
	struct DerivedToFront<TypeSeq<>> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts>
	struct DerivedToFront<TypeSeq<T, Ts...>> {
		using TheMostDerived = typename TypeSeq<Ts...>::template MostDerived<T>;
		using TheMostDerivedTail = typename TypeSeq<Ts...>::template Replace<TheMostDerived, T>;
		using Tail = typename DerivedToFront<TheMostDerivedTail>::Type;
		using Type = typename Tail::template PushFront<TheMostDerived>;
	};

	//
	template <class TS, template <class> class F>
	struct Map;

	template <template <class> class F>
	struct Map<TypeSeq<>, F> {
		using Type = TypeSeq<>;
	};

	template <class T, class... Ts, template <class> class F>
	struct Map<TypeSeq<T, Ts...>, F> {
		using Tail = typename Map<TypeSeq<Ts...>, F>::Type;
		using Type = typename Tail::template PushFront<F<T>>;
	};

	//
	template <class TS>
	struct All;

	template <class... Ts>
	struct All<TypeSeq<Ts...>> {
		using Type = And<Ts...>;
	};

	//
	template <class TS>
	struct Any;

	template <class... Ts>
	struct Any<TypeSeq<Ts...>> {
		using Type = Or<Ts...>;
	};

	//
	template <class This, usize pos, class TS>
	struct Insert;

	template <class... Ts, usize pos, class TS>
	struct Insert<TypeSeq<Ts...>, pos, TS> {
		using Head = typename TypeSeq<Ts...>::template Take<pos>;
		using Tail = typename TypeSeq<Ts...>::template Slice<pos, sizeof...(Ts)>;
		using Type = typename Join<Head, TS, Tail>::Type;
	};

	//
	template <class... Ts>
	struct TypeListCommon {
		template <usize n = 1>
		using PopBack = typename Slice<TypeSeq<Ts...>, 0, sizeof...(Ts) - n, (n <= sizeof...(Ts))>::Type;
	};

	//
	template <class... Ts>
	struct TypeListBase;

	template <>
	struct TypeListBase<> : TypeListCommon<> {
	};

	template <class T, class... Ts>
	struct TypeListBase<T, Ts...> : TypeListCommon<T, Ts...> {
	private:
		using TS = TypeSeq<T, Ts...>;

	public:
		template <class = void>
		using Front = T;

		template <class = void>
		using Back = typename Back<TS>::Type;
	};

} // namespace impl::ts

inline namespace meta {

	template <class... Ts>
	struct TypeSeq : impl::ts::TypeListBase<Ts...> {
		using This = TypeSeq;

		enum : usize {
			kSize = sizeof...(Ts)
		};

		enum : bool {
			kIsEmpty = kSize == 0
		};

		template <class T, usize count>
		using Repeat = typename impl::ts::Repeat<T, count>::Type;

		template <usize idx,
		    RB_REQUIRES(idx < kSize)>
		using At = typename impl::ts::At<This, idx>::Type;

		template <usize n = 1,
		    RB_REQUIRES(n <= kSize)>
		using PopFront = typename impl::ts::PopFront<This, n>::Type;

		template <class... Us>
		using PushFront = TypeSeq<Us..., Ts...>;

		template <class... Us>
		using PushBack = TypeSeq<Ts..., Us...>;

		template <class = void>
		using Flatten = typename impl::ts::Flatten<This>::Type;

		template <class TS>
		using Append = typename impl::ts::Append<This, TS>::Type;

		template <class T>
		inline static constexpr isize kIndexOf = impl::ts::IndexOf<This, T>::kValue;

		template <class T>
		using Erase = typename impl::ts::Erase<This, T>::Type;

		template <usize pos>
		using EraseAt = typename impl::ts::EraseAt<This, pos>::Type;

		/// Removes the elements in the range [@p from, @p to).
		template <usize from, usize to, RB_REQUIRES(from <= to)>
		using EraseRange = typename impl::ts::EraseRange<This, from, to>::Type;

		template <template <class> class UnaryPredicate>
		using EraseIf = typename impl::ts::EraseIf<This, UnaryPredicate>::Type;

		template <class T>
		using EraseAll = typename impl::ts::EraseAll<This, T>::Type;

		template <template <class> class F>
		using Filter = typename impl::ts::Filter<This, F>::Type;

		template <class = void>
		using Reverse = typename impl::ts::Reverse<This>::Type;

		template <class = void>
		using Unique = typename impl::ts::Unique<This>::Type;

		template <template <class, class> class BinaryPredicate>
		using UniqueIf = typename impl::ts::UniqueIf<This, BinaryPredicate>::Type;

		template <class T, class U>
		using Replace = typename impl::ts::Replace<This, T, U>::Type;

		template <class T, class U>
		using ReplaceAll = typename impl::ts::ReplaceAll<This, T, U>::Type;

		template <usize from, usize to>
		using Slice = typename impl::ts::Slice<This, from, to - from, (from <= to && to <= kSize)>::Type;

		template <usize count>
		using Take = Slice<0, count>;

		template <class T, usize count>
		using Resize = typename impl::ts::Resize<This, T, count, (count < kSize)>::Type;

		template <class T>
		using MostDerived = typename impl::ts::MostDerived<This, T>::Type;

		template <class = void>
		using DerivedToFront = typename impl::ts::DerivedToFront<This>::Type;

		template <template <class> class F>
		using Map = typename impl::ts::Map<This, F>::Type;

		template <template <class> class F>
		using All = Bool<impl::ts::All<Map<F>>::Type::value>;

		template <template <class> class F>
		using Any = Bool<impl::ts::Any<Map<F>>::Type::value>;

		template <usize pos, class TS>
		using Insert = typename impl::ts::Insert<This, pos, TS>::Type;

		template <usize pos, class T>
		using InsertAt = Insert<pos, TypeSeq<T>>;
	};

	inline std::ostream& operator<<(std::ostream& os, TypeSeq<> /*unused*/) {
		return os << "[]";
	}

	template <class T, class... Ts>
	std::ostream& operator<<(std::ostream& os, TypeSeq<T, Ts...> /*unused*/) {
		return os << "[" << typeid(T).name(), ((os << ", " << typeid(Ts).name()), ...), os << "]";
	}

} // namespace meta

} // namespace rb::core
