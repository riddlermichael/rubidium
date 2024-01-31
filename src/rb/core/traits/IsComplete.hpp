#pragma once

#include <rb/core/traits/Bool.hpp>
#include <rb/core/traits/declval.hpp>

namespace rb::core {

namespace impl {

	// ReSharper disable once CppFunctionIsNotImplemented
	template <class T, unsigned = sizeof(T)>
	True isCompleteImpl(T*);

	False isCompleteImpl(...);

} // namespace impl

inline namespace traits {

	template <class T>
	using IsComplete = decltype(impl::isCompleteImpl(RB_DECLVAL(T*)));

} // namespace traits

} // namespace rb::core
