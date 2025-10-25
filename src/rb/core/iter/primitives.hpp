#pragma once

#include <initializer_list>

#include <rb/core/helpers.hpp>
#include <rb/core/requires.hpp>
#include <rb/core/traits/CommonType.hpp>
#include <rb/core/traits/detection.hpp>
#include <rb/core/traits/Signed.hpp>

namespace rb::core {

namespace impl {
	RB_METHOD_DETECTOR_NAME(begin, BeginMember)
	RB_METHOD_DETECTOR_NAME(end, EndMember)
	RB_METHOD_DETECTOR_NAME(empty, EmptyMember)
	RB_METHOD_DETECTOR_NAME(data, DataMember)
	RB_METHOD_DETECTOR_NAME(size, SizeMember)
} // namespace impl

inline namespace iter {

	// begin

	template <class C,
	    RB_REQUIRES_T(impl::HasBeginMember<C>)>
	[[nodiscard]] constexpr decltype(auto) begin(C& c) RB_NOEXCEPT_LIKE(c.begin()) {
		return c.begin();
	}

	template <class C,
	    RB_REQUIRES_T(impl::HasBeginMember<C const>)>
	[[nodiscard]] constexpr decltype(auto) begin(C const& c) RB_NOEXCEPT_LIKE(c.begin()) {
		return c.begin();
	}

	template <class T, usize n>
	[[nodiscard]] constexpr T* begin(T (&a)[n]) noexcept {
		return a;
	}

	template <class C>
	[[nodiscard]] constexpr decltype(auto) cbegin(C const& c) RB_NOEXCEPT_LIKE(core::begin(c)) {
		return core::begin(c);
	}

	RB_FUNCTION_DETECTOR(core::begin, Begin)

	// end

	template <class C,
	    RB_REQUIRES_T(impl::HasEndMember<C>)>
	[[nodiscard]] constexpr decltype(auto) end(C& c) RB_NOEXCEPT_LIKE(c.end()) {
		return c.end();
	}

	template <class C,
	    RB_REQUIRES_T(impl::HasEndMember<C const>)>
	[[nodiscard]] constexpr decltype(auto) end(C const& c) RB_NOEXCEPT_LIKE(c.end()) {
		return c.end();
	}

	template <class T, usize n>
	[[nodiscard]] constexpr T* end(T (&a)[n]) noexcept {
		return a + n;
	}

	template <class C>
	[[nodiscard]] constexpr decltype(auto) cend(C const& c) RB_NOEXCEPT_LIKE(core::end(c)) {
		return core::end(c);
	}

	RB_FUNCTION_DETECTOR(core::end, End)

	// empty

	template <class C,
	    RB_REQUIRES_T(impl::HasEmptyMember<C const>)>
	[[nodiscard]] constexpr decltype(auto) empty(C const& c) RB_NOEXCEPT_LIKE(c.empty()) {
		return c.empty();
	}

	template <class T, usize n>
	[[nodiscard]] constexpr bool empty(T const (&a)[n]) noexcept {
		RB_UNUSED(a);
		return false;
	}

	template <class E>
	[[nodiscard]] constexpr bool empty(std::initializer_list<E> il) noexcept {
		return il.size() == 0;
	}

	RB_FUNCTION_DETECTOR(core::empty, Empty)

	// data

	template <class C,
	    RB_REQUIRES_T(impl::HasDataMember<C>)>
	[[nodiscard]] constexpr decltype(auto) data(C& c) RB_NOEXCEPT_LIKE(c.data()) {
		return c.data();
	}

	template <class C,
	    RB_REQUIRES_T(impl::HasDataMember<C const>)>
	[[nodiscard]] constexpr decltype(auto) data(C const& c) RB_NOEXCEPT_LIKE(c.data()) {
		return c.data();
	}

	template <class T, usize n>
	[[nodiscard]] constexpr T* data(T (&a)[n]) noexcept {
		return a;
	}

	template <class T>
	[[nodiscard]] constexpr T const* data(std::initializer_list<T> il) noexcept {
		return il.begin();
	}

	RB_FUNCTION_DETECTOR(core::data, Data)

	// size

	template <class C,
	    RB_REQUIRES_T(impl::HasSizeMember<C const>)>
	[[nodiscard]] constexpr decltype(auto) size(C const& c) RB_NOEXCEPT_LIKE(c.size()) {
		return c.size();
	}

	template <class T, usize n>
	[[nodiscard]] constexpr usize size(T const (&a)[n]) noexcept {
		RB_UNUSED(a);
		return n;
	}

	RB_FUNCTION_DETECTOR(core::size, Size)

	// ssize

	template <class C>
	[[nodiscard]] constexpr decltype(auto) ssize(C const& c) RB_NOEXCEPT_LIKE(c.size()) {
		using R = CommonType<isize, SignedType<decltype(c.size())>>;
		return static_cast<R>(c.size());
	}

	template <class T, isize n>
	[[nodiscard]] constexpr isize ssize(T const (&a)[n]) noexcept {
		RB_UNUSED(a);
		return n;
	}

} // namespace iter

} // namespace rb::core
