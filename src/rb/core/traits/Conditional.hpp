#pragma once

namespace rb::core {

namespace impl {

	template <bool value, class T, class F>
	struct ConditionalImpl {
		using Type = T;
	};

	template <class T, class F>
	struct ConditionalImpl<false, T, F> {
		using Type = F;
	};

} // namespace impl

inline namespace traits {

	template <bool value, class T, class F>
	using Conditional = typename impl::ConditionalImpl<value, T, F>::Type;

} // namespace traits

} // namespace rb::core
