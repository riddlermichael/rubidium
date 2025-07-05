#pragma once

#include <array>

#include <rb/core/iter/primitives.hpp>
#include <rb/core/memory/toAddress.hpp>
#include <rb/core/slices/Sliceable.hpp>

namespace rb::core {

enum : usize {
	kDynExtent = static_cast<usize>(-1)
};

namespace impl {

	template <class T, usize n>
	class RB_EXPORT ContiguousViewStorage {
	protected:
		constexpr ContiguousViewStorage(T* data, usize size) noexcept
		    : data_(data) {
			RB_ASSERT_MSG("ContiguousView size mismatch", size == n);
		}

		constexpr T* data() const noexcept {
			return data_;
		}

		constexpr usize size() const noexcept {
			return n;
		}

	private:
		T* data_;
	};

	template <class T>
	class RB_EXPORT ContiguousViewStorage<T, kDynExtent> {
	protected:
		constexpr ContiguousViewStorage(T* data, usize size) noexcept
		    : data_(data)
		    , size_(size) {
		}

		constexpr T* data() const noexcept {
			return data_;
		}

		constexpr usize size() const noexcept {
			return size_;
		}

		constexpr void popFrontImpl() noexcept {
			++data_;
			--size_;
		}

		constexpr void popBackImpl() noexcept {
			--size_;
		}

	private:
		T* data_;
		usize size_;
	};

} // namespace impl

template <class T, class ConstSliceView, class SliceView, usize n>
class ContiguousView;

template <class T, class ConstSliceView, class SliceView, usize n>
struct ContainerTraits<ContiguousView<T, ConstSliceView, SliceView, n>> {
	using Value = RemoveCv<T>;
	using Iterator = T*;
	using ConstIterator = T const*;
	using Difference = isize;
	using Size = usize;
};

template <class T,
    class ConstSliceView = ViewImpl<T, false>,
    class SliceView = ViewImpl<T, true>,
    usize n = kDynExtent>
class RB_EXPORT ContiguousView
    : impl::ContiguousViewStorage<T, n>
    , public Sliceable<ContiguousView<T, ConstSliceView, SliceView, n>, ConstSliceView, SliceView> {
	using Super = impl::ContiguousViewStorage<T, n>;
	using Base = Sliceable<ContiguousView<T, ConstSliceView, SliceView, n>, ConstSliceView, SliceView>;

public:
	RB_USE_BASE_CONTAINER_TYPES(Base)

	enum : usize {
		kExtent = n
	};

	// ctor.1
	template <bool _ = true, RB_REQUIRES(_ && (n == kDynExtent || n == 0))>
	constexpr ContiguousView() noexcept
	    : Super(nullptr, 0) {
	}

	// ctor.2
	template <class It,
	    bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
	constexpr ContiguousView(It first, usize size) noexcept
	    : Super(toAddress(first), size) {
	}

	// ctor.2
	template <class It,
	    bool _ = true, RB_REQUIRES(_&& n != kDynExtent)>
	constexpr explicit ContiguousView(It first, usize size) noexcept
	    : Super(toAddress(first), size) {
	}

	// ctor.3
	template <class It, class End,
	    RB_REQUIRES(!isConvertible<End, usize> && (n == kDynExtent))>
	constexpr ContiguousView(It first, End last) RB_NOEXCEPT_LIKE(last - first)
	    : Super(toAddress(first), last - first) {
	}

	// ctor.3
	template <class It, class End,
	    RB_REQUIRES(!isConvertible<End, usize> && (n != kDynExtent))>
	constexpr explicit ContiguousView(It first, End last) RB_NOEXCEPT_LIKE(last - first)
	    : Super(toAddress(first), last - first) {
	}

	// NOLINTBEGIN(google-explicit-constructor)

	// ctor.4
	template <usize sz,
	    RB_REQUIRES(n == kDynExtent || n == sz)>
	constexpr ContiguousView(T (&a)[sz]) noexcept
	    : Super(a, sz) {
	}

	// ctor.5
	template <class U, usize sz,
	    RB_REQUIRES(isConvertible<U (*)[], T (*)[]> && (n == kDynExtent || n == sz))>
	constexpr ContiguousView(std::array<U, sz>& a) noexcept
	    : Super(a.data(), sz) {
	}

	// ctor.6
	template <class U, usize sz,
	    RB_REQUIRES(isConvertible<U (*)[], T (*)[]> && (n == kDynExtent || n == sz))>
	constexpr ContiguousView(std::array<U, sz> const& a) noexcept
	    : Super(a.data(), sz) {
	}

	// ctor.8
	template <class U, usize sz, class _1, class _2,
	    RB_REQUIRES(isConvertible<U (*)[], T (*)[]> && (n == kDynExtent || sz == kDynExtent || n == sz))>
	constexpr ContiguousView(ContiguousView<U, _1, _2, sz> const& rhs) noexcept
	    : Super(rhs.data(), rhs.size()) {
	}

	// NOLINTEND(google-explicit-constructor)

	constexpr ContiguousView(ContiguousView const&) noexcept = default;
	constexpr ContiguousView(ContiguousView&&) noexcept = default;

	constexpr ContiguousView& operator=(ContiguousView const&) noexcept = default;
	constexpr ContiguousView& operator=(ContiguousView&&) noexcept = default;

	template <bool _ = true, RB_REQUIRES(_&& n != 0)>
	constexpr T& front() const noexcept {
		RB_ASSERT_MSG("Call front() on empty ContiguousView", !empty());
		return data()[0];
	}

	template <bool _ = true, RB_REQUIRES(_&& n != 0)>
	constexpr T& back() const noexcept {
		RB_ASSERT_MSG("Call back() on empty ContiguousView", !empty());
		return data()[size() - 1];
	}

	using Super::data;
	using Super::size;

	constexpr Iterator begin() const noexcept {
		return data();
	}

	constexpr Iterator end() const noexcept {
		return data() + size();
	}

	using Base::operator[];

	template <bool _ = true, RB_REQUIRES(_&& n != 0)>
	constexpr T& operator[](usize idx) const noexcept {
		return at(idx);
	}

	template <usize idx,
	    bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
	constexpr T& at() const noexcept {
		RB_CHECK_RANGE(idx, 0_UZ, size());
		return data()[idx];
	}

	template <usize idx,
	    bool _ = true, RB_REQUIRES(_&& n != kDynExtent && idx < n)>
	constexpr T& at() const noexcept {
		return data()[idx];
	}

	constexpr T& at(usize idx) const noexcept {
		RB_CHECK_RANGE(idx, 0_UZ, size());
		return data()[idx];
	}

	[[nodiscard]] constexpr bool empty() const noexcept {
		return size() == 0;
	}

	template <bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
	constexpr void popFront() noexcept {
		RB_ASSERT_MSG("Call popFront() on empty ContiguousView", !empty());
		Super::popFrontImpl();
	}

	template <bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
	constexpr void popBack() noexcept {
		RB_ASSERT_MSG("Call popBack() on empty ContiguousView", !empty());
		Super::popBackImpl();
	}
};

namespace impl {
	template <class T, class = void, class = void>
	struct IsViewImpl {
		using Type = False;
	};

	template <class T>
	struct IsViewImpl<T,
	    Void<decltype(core::data(RB_DECLVAL(T const&)))>,
	    Void<decltype(core::data(RB_DECLVAL(T&)))>> {
	private:
		using C = RemoveConst<T>;
		using ConstData = decltype(core::data(RB_DECLVAL(C const&)));
		using MutableData = decltype(core::data(RB_DECLVAL(C&)));

	public:
		using Type = IsSame<ConstData, MutableData>;
	};
} // namespace impl

// IsView is true for types where the return type of .data() is the same for mutable and const instances.
template <class T>
using IsView = typename impl::IsViewImpl<T>::Type;

} // namespace rb::core
