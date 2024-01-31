#pragma once

#include <rb/core/traits/IsComplete.hpp>

namespace rb::core {

namespace impl {

	template <class T>
	constexpr bool isFinalImpl() noexcept {
		if constexpr (IsComplete<T>::value) {
			return __is_final(T);
		} else {
			return false;
		}
	}

} // namespace impl

inline namespace traits {

	template <class T>
	using IsUnion = Bool<__is_union(T)>;

	template <class T>
	inline constexpr bool isUnion = IsUnion<T>::value;

	template <class T>
	using IsAggregate = Bool<__is_aggregate(T)>;

	template <class T>
	inline constexpr bool isAggregate = IsAggregate<T>::value;

	template <class T>
	using IsStandardLayout = Bool<__is_standard_layout(T)>;

	template <class T>
	inline constexpr bool isStandardLayout = IsStandardLayout<T>::value;

	template <class T>
	using IsAbstract = Bool<__is_abstract(T)>;

	template <class T>
	inline constexpr bool isAbstract = IsAbstract<T>::value;

	template <class T>
	using IsFinal = Bool<impl::isFinalImpl<T>()>;

	template <class T>
	inline constexpr bool isFinal = IsFinal<T>::value;

	template <class T>
	using IsEmpty = Bool<__is_empty(T)>;

	template <class T>
	inline constexpr bool isEmpty = IsEmpty<T>::value;

	template <class T>
	using IsTriviallyCopyable = Bool<__is_trivially_copyable(T)>;

	template <class T>
	inline constexpr bool isTriviallyCopyable = __is_trivially_copyable(T);

} // namespace traits

} // namespace rb::core
