#pragma once

namespace rb::core {

namespace impl {

	template <bool value, class T = void>
	struct EnableIfImpl {};

	template <class T>
	struct EnableIfImpl<true, T> {
		using Type = T;
	};

} // namespace impl

inline namespace traits {

	template <bool value, class T = void>
	using EnableIf = typename impl::EnableIfImpl<value, T>::Type;

} // namespace traits

} // namespace rb::core
