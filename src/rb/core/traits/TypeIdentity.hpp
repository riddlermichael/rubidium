#pragma once

namespace rb::core {

namespace impl {

	/// Provides the member typedef `Type` that names `T` (i.e., the identity transformation).
	template <class T>
	struct TypeIdentityImpl { using Type = T; };

} // namespace impl

inline namespace traits {

	/// TypeIdentity can be used to establish
	/// <a href="https://en.cppreference.com/w/cpp/language/template_argument_deduction#Non-deduced_contexts">
	/// non-deduced contexts
	/// </a>
	/// in template argument deduction.
	template <class T>
	using TypeIdentity = typename impl::TypeIdentityImpl<T>::Type;

} // namespace traits
} // namespace rb::core
