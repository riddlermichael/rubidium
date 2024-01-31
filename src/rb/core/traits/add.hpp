#pragma once

#include <rb/core/traits/Bool.hpp>
#include <rb/core/traits/Void.hpp>

namespace rb::core {

namespace impl {

	template <class T>
	struct AddConst {
		using Type = T const;
	};

	template <class T, class = void>
	struct AddPointerImpl {
		using Type = T;
	};

	template <class T>
	struct AddPointerImpl<T, Void<RemoveRef<T>*>> {
		using Type = RemoveRef<T>*;
	};

	template <class T, class = void>
	struct IsReferenceableImpl : False {};

	template <class T>
	struct IsReferenceableImpl<T, Void<T&>> : True {};

	template <class T, bool = IsReferenceableImpl<T>::value>
	struct AddRValueRefImpl {
		using Type = T;
	};

	template <class T>
	struct AddRValueRefImpl<T, true> {
		using Type = T&&;
	};

	template <class T, bool = IsReferenceableImpl<T>::value>
	struct AddLValueRefImpl {
		using Type = T;
	};

	template <class T>
	struct AddLValueRefImpl<T, true> {
		using Type = T&;
	};

} // namespace impl

inline namespace traits {

	/// AddConst can be used to establish
	/// <a href="https://en.cppreference.com/w/cpp/language/template_argument_deduction#Non-deduced_contexts">
	/// non-deduced contexts
	/// </a>
	/// in template argument deduction.
	template <class T>
	using AddConst = typename impl::AddConst<T>::Type;

	template <class T>
	using AddPointer = typename impl::AddPointerImpl<T>::Type;

	template <class T>
	using IsReferenceable = impl::IsReferenceableImpl<T>;

	template <class T>
	inline constexpr bool isReferenceable = IsReferenceable<T>::value;

	template <class T>
	using AddRValueRef = typename impl::AddRValueRefImpl<T>::Type;

	template <class T>
	using AddLValueRef = typename impl::AddLValueRefImpl<T>::Type;

} // namespace traits

} // namespace rb::core
