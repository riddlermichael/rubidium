#pragma once

#include <rb/core/error/RangeError.hpp>
#include <rb/core/memory/allocators.hpp>
#include <rb/core/memory/AllocatorTraits.hpp>
#include <rb/core/Span.hpp>

namespace rb::core {

template <class T>
class Array;

template <class T>
struct ContainerTraits<Array<T>> {
	using Value = T;
	using Iterator = T*;
	using ConstIterator = T const*;
	using Difference = isize;
	using Size = usize;
};

/// Owning counterpart of rb::core::Span, or std::unique_ptr<T[]> with `size()`, or std::vector<T> without resizing.
template <class T>
class RB_EXPORT Array final : public Sliceable<Array<T>, Span<T const>, Span<T>> {
	using Super = Sliceable<Array, Span<T const>, Span<T>>;
	using Alloc = ArrayAllocator<T>;
	using AllocTraits = AllocatorTraits<Alloc>;

	enum class Op {
		kDefault,
		kCopy,
		kMove,
	};

public:
	RB_USE_BASE_CONTAINER_TYPES(Super)

	using Pointer = T*;
	using ConstPointer = T const*;

	template <bool _ = true, RB_REQUIRES(_&& isDefaultConstructible<T>)>
	explicit Array(usize size) noexcept(isNothrowDefaultConstructible<T>) {
		init<Op::kDefault>(nullptr, size);
	}

	// elements of an initializer list are always passed via const reference,
	// so we can't use an initializer list with move-only types (but can declare it, meh);
	// use a plain array in such a case
	template <bool _ = true, RB_REQUIRES(_&& isCopyConstructible<T>)>
	Array(std::initializer_list<T> il) noexcept(isNothrowCopyConstructible<T>) {
		init(il.begin(), il.size());
	}

	template <usize n,
	    bool _ = true, RB_REQUIRES(_&& isCopyConstructible<T>)>
	explicit Array(T const (&a)[n]) noexcept(isNothrowCopyConstructible<T>) {
		init(a, n);
	}

	template <usize n,
	    bool _ = true, RB_REQUIRES(_&& isMoveConstructible<T>)>
	explicit Array(T (&&a)[n]) noexcept(isNothrowMoveConstructible<T>) {
		init<Op::kMove>(a, n);
	}

	template <usize n,
	    bool _ = true, RB_REQUIRES(_&& isCopyConstructible<T>)>
	explicit Array(std::array<T, n> const& a) noexcept(isNothrowCopyConstructible<T>) {
		init(a.data(), n);
	}

	template <usize n,
	    bool _ = true, RB_REQUIRES(_&& isMoveConstructible<T>)>
	explicit Array(std::array<T, n>&& a) noexcept(isNothrowMoveConstructible<T>) {
		init<Op::kMove>(a.data(), n);
	}

	template <class InputIt>
	Array(InputIt first, InputIt last) noexcept(isNothrowCopyConstructible<T>) {
		init(first, last - first);
	}

	Array(Array const&) = delete;

	constexpr Array(Array&& rhs) noexcept
	    : size_{rhs.size_}
	    , data_{rhs.data_} {
		rhs.size_ = 0;
		rhs.data_ = nullptr;
	}

	~Array() {
		if (!data_ || !size_) {
			return;
		}

		Alloc alloc;
		for (Pointer ptr = data_ + size_ - 1; ptr >= data_; --ptr) {
			AllocTraits::destroy(alloc, ptr);
		}
		Alloc::deallocate(data_, size_);
	}

	Array& operator=(Array const&) = delete;

	constexpr Array& operator=(Array&& rhs) noexcept {
		if (this != &rhs) {
			this->~Array();
			new (this) Array{RB_MOVE(rhs)};
		}
		return *this;
	}

	using Super::operator[];

	constexpr T const& operator[](usize idx) const {
		RB_CHECK_RANGE(idx, 0, size_);
		return data_[idx];
	}

	constexpr T& operator[](usize idx) {
		RB_CHECK_RANGE(idx, 0, size_);
		return data_[idx];
	}

#pragma region STL

	constexpr Iterator begin() noexcept {
		return data_;
	}

	constexpr ConstIterator begin() const noexcept {
		return data_;
	}

	constexpr ConstIterator cbegin() const noexcept {
		return data_;
	}

	constexpr Iterator end() noexcept {
		return data_ + size_;
	}

	constexpr ConstIterator end() const noexcept {
		return data_ + size_;
	}

	constexpr ConstIterator cend() const noexcept {
		return data_ + size_;
	}

	constexpr ConstPointer data() const noexcept {
		return data_;
	}

	constexpr Pointer data() noexcept {
		return data_;
	}

	constexpr usize size() const noexcept {
		return size_;
	}

	[[nodiscard]] constexpr bool empty() const noexcept {
		return size_ == 0;
	}

#pragma endregion STL

private:
	// unfortunately, we cannot use syntax `Array<Op::kCopy>(...)` with constructors
	// because constructors are not functions, and usage of SFINAE tricks would be redundant
	template <Op op = Op::kCopy, class It>
	void init(It first, usize size) {
		size_ = size;
		data_ = Alloc::allocate(size_);
		Alloc alloc;
		usize idx = 0;
		try {
			for (; idx < size_; ++idx) {
				if constexpr (op == Op::kDefault) {
					AllocTraits::construct(alloc, data_ + idx);
				} else if constexpr (op == Op::kCopy) {
					AllocTraits::construct(alloc, data_ + idx, *first++);
				} else {
					AllocTraits::construct(alloc, data_ + idx, RB_MOVE(*first++));
				}
			}
		} catch (...) {
			destroy(data_, data_ + idx, alloc);
			Alloc::deallocate(data_, size_);
			throw;
		}
	}

	usize size_ = 0;
	Pointer data_ = nullptr;
};

template <class T, usize n,
    RB_REQUIRES_T(IsCopyConstructible<T>)>
Array(T (&)[n]) -> Array<T>;

template <class T, usize n,
    RB_REQUIRES_T(IsMoveConstructible<T>)>
Array(T (&&)[n]) -> Array<T>;

template <class T, usize n,
    RB_REQUIRES_T(IsCopyConstructible<T>)>
[[nodiscard]] constexpr auto toArray(T (&a)[n]) {
	return Array<RemoveCv<T>>(a);
}

template <class T, usize n,
    RB_REQUIRES_T(IsMoveConstructible<T>)>
[[nodiscard]] constexpr auto toArray(T (&&a)[n]) {
	return Array<RemoveCv<T>>(RB_MOVE(a));
}

template <class T,
    RB_REQUIRES_T(IsWritableTo<T, std::ostream>)>
std::ostream& operator<<(std::ostream& os, Array<T> const& array) {
	return fmt::pprint(os, array, "[", "]", ", ");
}

} // namespace rb::core
