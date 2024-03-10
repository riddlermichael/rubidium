#pragma once

#include <rb/core/byte.hpp>
#include <rb/core/slices/ContiguousView.hpp>
#include <rb/fmt/pprint.hpp>

namespace rb::core {

template <class T, usize n>
class Span;

template <class T, usize n>
struct ContainerTraits<Span<T, n>> {
	using Value = RemoveCv<T>;
	using Iterator = T*;
	using ConstIterator = T const*;
	using Difference = isize;
	using Size = usize;
};

template <class C>
inline constexpr bool isSpanNothrowConstructibleFrom = //
    noexcept(core::data(RB_DECLVAL(C&))) && noexcept(core::size(RB_DECLVAL(C&)));

template <class T, usize n = kDynExtent>
class RB_EXPORT Span final
    : public ContiguousView<T, Span<T const>, Span<T>, n> {
	using Super = ContiguousView<T, Span<T const>, Span<T>, n>;

	// We want to enable conversion from vector<T*> to Span<T const* const>
	// but disable conversion from vector<Derived> to Span<Base>.
	// Here we use the fact that U** is convertible to Q* const*
	// if and only if Q is the same type or a more cv-qualified version of U.
	// We also decay the result type of data() to avoid problems with classes
	// which have a member function data() which returns a reference.
	template <class C>
	using HasData = IsConvertible<Decay<decltype(core::data(RB_DECLVAL(C&)))>*, T* const*>;

	template <class C>
	using IsConvertibleFrom = And<HasData<C>, HasSize<C>>;

public:
	RB_USE_BASE_CONTAINER_TYPES(Super)

	using Element = T;
	using Pointer = T*;
	using ConstPointer = T const*;

	// ctor.1
	template <bool _ = true, RB_REQUIRES(_ && (n == kDynExtent || n == 0))>
	constexpr Span() noexcept
	    : Super() {
	}

	// ctor.2
	template <class It,
	    bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
	constexpr Span(It first, usize size) noexcept
	    : Super(toAddress(first), size) {
	}

	// ctor.2
	template <class It,
	    bool _ = true, RB_REQUIRES(_&& n != kDynExtent)>
	constexpr explicit Span(It first, usize size) noexcept
	    : Super(toAddress(first), size) {
	}

	// ctor.3
	template <class It, class End,
	    RB_REQUIRES(!isConvertible<End, usize> && (n == kDynExtent))>
	constexpr Span(It first, End last) RB_NOEXCEPT_LIKE(last - first)
	    : Super(toAddress(first), last - first) {
	}

	// ctor.3
	template <class It, class End,
	    RB_REQUIRES(!isConvertible<End, usize> && (n != kDynExtent))>
	constexpr explicit Span(It first, End last) RB_NOEXCEPT_LIKE(last - first)
	    : Super(toAddress(first), last - first) {
	}

	// NOLINTBEGIN(google-explicit-constructor)

	// ctor.4
	template <usize sz,
	    RB_REQUIRES(n == kDynExtent || n == sz)>
	constexpr Span(T (&a)[sz]) noexcept
	    : Super(a, sz) {
	}

	// ctor.5
	template <class U, usize sz,
	    RB_REQUIRES(isConvertible<U (*)[], T (*)[]> && (n == kDynExtent || n == sz))>
	constexpr Span(std::array<U, sz>& a) noexcept
	    : Super(a.data(), sz) {
	}

	// ctor.6
	template <class U, usize sz,
	    RB_REQUIRES(isConvertible<U (*)[], T (*)[]> && (n == kDynExtent || n == sz))>
	constexpr Span(std::array<U, sz> const& a) noexcept
	    : Super(a.data(), sz) {
	}

	// ctor.8
	template <class U, usize sz,
	    RB_REQUIRES(isConvertible<U (*)[], T (*)[]> && (n == kDynExtent || sz == kDynExtent || n == sz))>
	constexpr Span(Span<U, sz> const& rhs) noexcept
	    : Super(rhs.data(), rhs.size()) {
	}

#ifdef __cpp_conditional_explicit
	constexpr explicit(n != kDynExtent) Span(std::initializer_list<Value> il RB_LIFETIME_BOUND) noexcept
	    requires(isConst<T>)
	    : Super(il.begin(), il.size()) {
	}
#else
	template <bool _ = true, RB_REQUIRES(_&& isConst<T> && (n != kDynExtent))>
	constexpr explicit Span(std::initializer_list<Value> il RB_LIFETIME_BOUND) noexcept
	    : Super(il.begin(), il.size()) {
	}

	template <bool _ = true, RB_REQUIRES(_&& isConst<T> && (n == kDynExtent))>
	constexpr Span(std::initializer_list<Value> il RB_LIFETIME_BOUND) noexcept
	    : Super(il.begin(), il.size()) {
	}
#endif

	template <class C,
	    RB_REQUIRES_T(And<Not<IsConst<T>>, IsConvertibleFrom<C>, Not<IsView<C>>>)>
	constexpr explicit Span(C& c RB_LIFETIME_BOUND) noexcept(isSpanNothrowConstructibleFrom<C>)
	    : Span(core::data(c), core::size(c)) {
	}

	template <class C,
	    RB_REQUIRES_T(And<IsConst<T>, IsConvertibleFrom<C>, Not<IsView<C>>>)>
	constexpr Span(C const& c RB_LIFETIME_BOUND) noexcept(isSpanNothrowConstructibleFrom<C>)
	    : Span(core::data(c), core::size(c)) {
	}

	template <class C,
	    RB_REQUIRES_T(And<Not<IsConst<T>>, IsConvertibleFrom<C>, IsView<C>>)>
	constexpr explicit Span(C& c) noexcept(isSpanNothrowConstructibleFrom<C>)
	    : Span(core::data(c), core::size(c)) {
	}

	template <class C,
	    RB_REQUIRES_T(And<IsConst<T>, IsConvertibleFrom<C>, IsView<C>>)>
	constexpr Span(C const& c) noexcept(isSpanNothrowConstructibleFrom<C>)
	    : Span(core::data(c), core::size(c)) {
	}

	// NOLINTEND(google-explicit-constructor)

	constexpr Span(Span const&) noexcept = default;
	constexpr Span(Span&&) noexcept = default;

	constexpr Span& operator=(Span const&) noexcept = default;
	constexpr Span& operator=(Span&&) noexcept = default;

	using Super::begin;
	using Super::data;
	using Super::end;
	using Super::size;

	constexpr usize sizeBytes() const noexcept {
		return size() * sizeof(T);
	}

	// Subviews `first`/`subSpan`/`last`.
	// Variants: `left`/`mid`/`right` (Qt), `take`/none/`tail` (D).

	template <usize count,
	    bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
	constexpr Span<T, count> first() const noexcept {
		RB_ASSERT(count <= size());
		return Span<T, count>{data(), count};
	}

	template <usize count,
	    bool _ = true, RB_REQUIRES(_&& n != kDynExtent && count <= n)>
	constexpr Span<T, count> first() const noexcept {
		return Span<T, count>{data(), count};
	}

	constexpr Span<T> first(usize count) const noexcept {
		RB_ASSERT(count <= size());
		return {data(), count};
	}

	template <usize count,
	    bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
	constexpr Span<T, count> last() const noexcept {
		RB_ASSERT(count <= size());
		return Span<T, count>{data() + size() - count, count};
	}

	template <usize count,
	    bool _ = true, RB_REQUIRES(_&& n != kDynExtent && count <= n)>
	constexpr Span<T, count> last() const noexcept {
		return Span<T, count>{data() + size() - count, count};
	}

	constexpr Span<T> last(usize count) const noexcept {
		RB_ASSERT(count <= size());
		return {data() + size() - count, count};
	}

	template <usize offset,
	    bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
	constexpr Span<T> subSpan() const noexcept {
		RB_ASSERT(offset <= size());
		return {data() + offset, size() - offset};
	}

	template <usize offset,
	    bool _ = true, RB_REQUIRES(_&& n != kDynExtent && offset <= n)>
	constexpr Span<T, n - offset> subSpan() const noexcept {
		return Span<T, n - offset>{data() + offset, size() - offset};
	}

	template <usize offset, usize count,
	    bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
	constexpr Span<T, count> subSpan() const noexcept {
		RB_ASSERT(offset + count <= size());
		return Span<T, count>{data() + offset, count};
	}

	template <usize offset, usize count,
	    bool _ = true, RB_REQUIRES(_&& n != kDynExtent && offset + count <= n)>
	constexpr Span<T, count> subSpan() const noexcept {
		return Span<T, count>{data() + offset, count};
	}

	constexpr Span<T> subSpan(usize offset) const noexcept {
		RB_ASSERT(offset <= size());
		return {data() + offset, size() - offset};
	}

	constexpr Span<T> subSpan(usize offset, usize count) const noexcept {
		RB_ASSERT(offset + count <= size());
		return {data() + offset, count};
	}

	// conversions a-la slice

	template <class C>
	constexpr C to() const RB_NOEXCEPT_LIKE(C(begin(), end())) {
		return C(begin(), end());
	}

	template <template <class...> class C,
	    RB_REQUIRES_T(IsDetected<C, Value>)>
	constexpr C<Value> to() const RB_NOEXCEPT_LIKE(C<Value>(begin(), end())) {
		return C<Value>(begin(), end());
	}
};

template <class It, class EndOrSize>
Span(It, EndOrSize) -> Span<RemoveReference<decltype(*RB_DECLVAL(It&))>>;

template <class T, usize n>
Span(T (&)[n]) -> Span<T, n>;

template <class T, usize n>
Span(std::array<T, n>&) -> Span<T, n>;

template <class T, usize n>
Span(std::array<T, n> const&) -> Span<T const, n>;

template <class C>
Span(C&) -> Span<RemovePointer<decltype(core::data(RB_DECLVAL(C&)))>>;

template <class T, usize n>
RB_EXPORT constexpr auto asBytes(Span<T, n> s) noexcept {
	using S = Span<byte const, n == kDynExtent ? kDynExtent : sizeof(T) * n>;
	return S{reinterpret_cast<byte const*>(s.data()), s.sizeBytes()}; // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
}

template <class T, usize n,
    RB_REQUIRES(!isConst<T>)>
RB_EXPORT constexpr auto asWriteableBytes(Span<T, n> s) noexcept {
	using S = Span<byte, n == kDynExtent ? kDynExtent : sizeof(T) * n>;
	return S{reinterpret_cast<byte*>(s.data()), s.sizeBytes()}; // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
}

template <class T, usize n,
    RB_REQUIRES_T(IsWritableTo<T, std::ostream>)>
std::ostream& operator<<(std::ostream& os, Span<T, n> span) {
	return fmt::pprint(os, span, "[", "]", ", ");
}

template <class T, usize n>
constexpr Span<T, n> save(Span<T, n> const& s) noexcept {
	return s;
}

template <class C>
constexpr auto span(C& c) RB_NOEXCEPT_LIKE(Span{c}) {
	return Span{c};
}

} // namespace rb::core
