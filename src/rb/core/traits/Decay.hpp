#pragma once

#include <rb/core/traits/add.hpp>
#include <rb/core/traits/arrays.hpp>
#include <rb/core/traits/Conditional.hpp>
#include <rb/core/traits/IsFunction.hpp>

namespace rb::core {

namespace impl {

	template <class T>
	using DecayImpl = Conditional<isArray<T>,
	    AddPointer<RemoveExtent<T>>,
	    Conditional<isFunction<T>,
	        AddPointer<T>,
	        RemoveCv<T>>>;

} // namespace impl

inline namespace traits {

	template <class T>
	using Decay = impl::DecayImpl<RemoveRef<T>>;

} // namespace traits

} // namespace rb::core
