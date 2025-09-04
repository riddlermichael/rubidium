#pragma once

namespace rb::core {

namespace impl {

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

	template <class T>
	struct RemoveConstImpl {
		using Type = T;
	};

	template <class T>
	struct RemoveConstImpl<T const> {
		using Type = T;
	};

	template <class T>
	struct RemoveVolatile {
		using Type = T;
	};

	template <class T>
	struct RemoveVolatile<T volatile> {
		using Type = T;
	};

	template <class T>
	struct RemoveReference {
		using Type = T;
	};

	template <class T>
	struct RemoveReference<T&> {
		using Type = T;
	};

	template <class T>
	struct RemoveReference<T&&> {
		using Type = T;
	};

	template <class T>
	struct RemovePointerImpl {
		using Type = T;
	};

	template <class T>
	struct RemovePointerImpl<T*> {
		using Type = T;
	};

} // namespace impl

inline namespace traits {

	template <class T>
	using RemoveConst = typename impl::RemoveConstImpl<T>::Type;

	template <class T>
	using RemoveVolatile = typename impl::RemoveVolatile<T>::Type;

	template <class T>
	using RemoveCv = typename impl::RemoveCvImpl<T>::Type;

	template <class T>
	using RemoveConstVolatile = typename impl::RemoveCvImpl<T>::Type;

	template <class T>
	using RemoveReference = typename impl::RemoveReference<T>::Type;

	template <class T>
	using RemoveRef = RemoveReference<T>;

	template <class T>
	using RemoveCvRef = RemoveCv<RemoveRef<T>>;

	template <class T>
	using RemovePointer = typename impl::RemovePointerImpl<RemoveCv<T>>::Type;

} // namespace traits
} // namespace rb::core
