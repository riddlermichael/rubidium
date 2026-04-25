#pragma once

#include <rb/core/has.hpp>

namespace rb::core {

namespace impl {

#if RB_HAS_BUILTIN(__remove_cv)
	template <class T>
	struct RemoveCvImpl {
		using Type = __remove_cv(T);
	};
#else
	template <class T>
	struct RemoveCvImpl {
		using Type = T;
	};

	template <class T>
	struct RemoveCvImpl<T const> {
		using Type = T;
	};

	template <class T>
	struct RemoveCvImpl<T volatile> {
		using Type = T;
	};

	template <class T>
	struct RemoveCvImpl<T const volatile> {
		using Type = T;
	};
#endif

#if RB_HAS_BUILTIN(__remove_const)
	template <class T>
	struct RemoveConstImpl {
		using Type = __remove_const(T);
	};
#else
	template <class T>
	struct RemoveConstImpl {
		using Type = T;
	};

	template <class T>
	struct RemoveConstImpl<T const> {
		using Type = T;
	};
#endif

#if RB_HAS_BUILTIN(__remove_volatile)
	template <class T>
	struct RemoveVolatileImpl {
		using Type = __remove_volatile(T);
	};
#else
	template <class T>
	struct RemoveVolatileImpl {
		using Type = T;
	};

	template <class T>
	struct RemoveVolatileImpl<T volatile> {
		using Type = T;
	};
#endif

#if RB_HAS_BUILTIN(__remove_reference)
	template <class T>
	struct RemoveRefImpl {
		using Type = __remove_reference(T);
	};
#else
	template <class T>
	struct RemoveRefImpl {
		using Type = T;
	};

	template <class T>
	struct RemoveRefImpl<T&> {
		using Type = T;
	};

	template <class T>
	struct RemoveRefImpl<T&&> {
		using Type = T;
	};
#endif

#if RB_HAS_BUILTIN(__remove_pointer)
	template <class T>
	struct RemovePointerImpl {
		using Type = __remove_pointer(T);
	};
#else
	template <class T>
	struct RemovePointerImpl {
		using Type = T;
	};

	template <class T>
	struct RemovePointerImpl<T*> {
		using Type = T;
	};
#endif

} // namespace impl

inline namespace traits {

	template <class T>
	using RemoveConst = typename impl::RemoveConstImpl<T>::Type;

	template <class T>
	using RemoveVolatile = typename impl::RemoveVolatileImpl<T>::Type;

	template <class T>
	using RemoveCv = typename impl::RemoveCvImpl<T>::Type;

	template <class T>
	using RemoveConstVolatile = typename impl::RemoveCvImpl<T>::Type;

	template <class T>
	using RemoveRef = typename impl::RemoveRefImpl<T>::Type;

	template <class T>
	using RemoveCvRef = RemoveCv<RemoveRef<T>>;

	template <class T>
	using RemovePointer = typename impl::RemovePointerImpl<RemoveCv<T>>::Type;

} // namespace traits
} // namespace rb::core
