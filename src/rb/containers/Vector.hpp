#pragma once

#include <rb/core/error/RangeError.hpp>
#include <rb/core/exchange.hpp>
#include <rb/core/memory/Allocator.hpp>
#include <rb/core/memory/AllocatorTraits.hpp>
#include <rb/core/memory/uninitialized.hpp>
#include <rb/core/Span.hpp>
#include <rb/core/swap.hpp>

namespace rb::containers {
template <class T>
class Vector;
} // namespace rb::containers

template <class T>
struct rb::core::ContainerTraits<rb::containers::Vector<T>> {
	using Value = T;
	using Iterator = T*;
	using ConstIterator = T const*;
	using Difference = isize;
	using Size = usize;
};

namespace rb::containers {

template <class T>
class Vector final : public core::Sliceable<Vector<T>, core::Span<T const>, core::Span<T>> {
	using Super = core::Sliceable<Vector, core::Span<T const>, core::Span<T>>;
	using Alloc = core::Allocator<T>;
	using AllocTraits = core::AllocatorTraits<Alloc>;

public:
	RB_USE_BASE_CONTAINER_TYPES(Super)

	using ConstRange = core::Span<T const>;
	using Range = core::Span<T>;

	// NOLINTBEGIN(*-identifier-naming)
	using value_type = T;
	using size_type = usize;
	using difference_type = isize;
	using reference = T&;
	using const_reference = T const&;
	using const_iterator = T const*;
	using iterator = T*;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	// NOLINTEND(*-identifier-naming)

	static constexpr usize kMaxSize = core::max<usize> / sizeof(T);

#pragma region constructors

	// ctor.1
	constexpr Vector() noexcept = default;

	// ctor.3
	Vector(usize count, T const& value)
	    : size_{count}
	    , capacity_{count}
	    , data_{Alloc{}.allocate(capacity_)} {
		core::uninitializedFillN(data_, count, value);
	}

	// ctor.4
	explicit Vector(usize count)
	    : size_{count}
	    , capacity_{count}
	    , data_{Alloc{}.allocate(capacity_)} {
		core::uninitializedValueConstructN(data_, count);
	}

	// ctor.5
	template <class InputIt,
	    RB_REQUIRES_T(core::IsInputIterator<InputIt>)>
	Vector(InputIt first, InputIt last)
	    : size_{static_cast<usize>(std::distance(first, last))}
	    , capacity_{size_}
	    , data_{Alloc{}.allocate(capacity_)} {
		core::uninitializedCopy(first, last, data_);
	}

	// ctor.6
	Vector(Vector const& rhs)
	    : Vector(rhs.begin(), rhs.end()) {
	}

	// ctor.8
	Vector(Vector&& rhs) noexcept
	    : size_(core::exchange(rhs.size_, 0))
	    , capacity_(core::exchange(rhs.capacity_, 0))
	    , data_(core::exchange(rhs.data_, nullptr)) {
	}

	// ctor.10
	/// Constructs the container with the contents of the initializer list @p il.
	Vector(std::initializer_list<T> il)
	    : Vector(il.begin(), il.end()) {
	}

	// ctor.11
	template <class R,
	    RB_REQUIRES_T(core::And<ranges::IsInputRangeNonStrict<R>, core::Not<ranges::IsInfinite<R>>>)>
	Vector(ranges::FromRange /*fromRange*/, R&& range)
	    : size_{ranges::size(RB_FWD(range))}
	    , capacity_{size_}
	    , data_{Alloc{}.allocate(capacity_)} {
		for (auto&& r = RB_FWD(range); !ranges::empty(r); ranges::popFront(r)) {
			// FIXME
		}
	}

	~Vector() noexcept(core::isNothrowDestructible<T>) {
		if (data_) {
			clear();
			Alloc alloc;
			AllocTraits::deallocate(alloc, data_, capacity_);
		}
	}

#pragma endregion constructors

#pragma region operators

	/// Copy assignment operator. Replaces the contents with a copy of @p rhs.
	Vector& operator=(Vector const& rhs) {
		if (this != &rhs) {
			this->~Vector();
			new (this) Vector(rhs);
		}
		return *this;
	}

	/// Move assignment operator.
	/// Replaces the contents with those of @p rhs using move semantics
	/// (i.e., the data in @p rhs is moved from @p rhs into this container).
	/// @p rhs is in a valid but unspecified state afterward.
	Vector& operator=(Vector&& rhs) noexcept {
		if (this != &rhs) {
			this->~Vector();
			new (this) Vector(RB_MOVE(rhs));
		}
		return *this;
	}

	/// Replaces the contents with those identified by initializer list @p il.
	Vector& operator=(std::initializer_list<T> il) {
		this->~Vector();
		new (this) Vector(il);
		return *this;
	}

	using Super::operator[];

	constexpr T const& operator[](usize pos) const {
		RB_CHECK_RANGE(pos, 0, size_);
		return data_[pos];
	}

	constexpr T& operator[](usize pos) {
		RB_CHECK_RANGE(pos, 0, size_);
		return data_[pos];
	}

#pragma endregion operators

#pragma region iteration

	// begin/end/range

	constexpr ConstRange range() const noexcept {
		return {data_, size_};
	}

	constexpr Range range() noexcept {
		return {data_, size_};
	}

	constexpr const_iterator begin() const noexcept {
		return data();
	}

	constexpr const_iterator cbegin() const noexcept {
		return begin();
	}

	constexpr iterator begin() noexcept {
		return data();
	}

	constexpr const_iterator end() const noexcept {
		return data() + size();
	}

	constexpr const_iterator cend() const noexcept {
		return end();
	}

	constexpr iterator end() noexcept {
		return data() + size();
	}

	constexpr const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator{end()};
	}

	constexpr reverse_iterator rbegin() noexcept {
		return reverse_iterator{end()};
	}

	constexpr const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}

	constexpr const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator{begin()};
	}

	constexpr reverse_iterator rend() noexcept {
		return reverse_iterator{begin()};
	}

	constexpr const_reverse_iterator crend() const noexcept {
		return rend();
	}

#pragma endregion iteration

#pragma region container

	// back/capacity/data/empty/front/size

	[[nodiscard]] constexpr bool empty() const noexcept {
		return size_ == 0;
	}

	constexpr usize size() const noexcept {
		return size_;
	}

	constexpr usize capacity() const noexcept {
		return capacity_;
	}

	constexpr T const* data() const noexcept {
		return data_;
	}

	constexpr T* data() noexcept {
		return data_;
	}

	constexpr T const& front() const {
		RB_ASSERT(!empty());
		return data_[0];
	}

	constexpr T& front() {
		RB_ASSERT(!empty());
		return data_[0];
	}

	constexpr T const& back() const {
		RB_ASSERT(!empty());
		return data_[size_ - 1];
	}

	constexpr T& back() {
		RB_ASSERT(!empty());
		return data_[size_ - 1];
	}

#pragma endregion container

	/// Erases all elements from the container.
	/// After this call, size() returns zero. Leaves the capacity() of the vector unchanged.
	void clear() noexcept(core::isNothrowDestructible<T>) {
		core::destroy(rbegin(), rend());
		size_ = 0;
	}

	/// Increase the capacity of the vector to a value that's greater or equal to @p newCapacity.
	/// If @p newCapacity is greater than the current capacity(), new storage is allocated;
	/// otherwise, the function does nothing.
	/// Reserve() does not change the size of the vector.
	void reserve(usize newCapacity) {
		RB_ASSERT_MSG("Too big capacity", newCapacity <= kMaxSize);
		if (newCapacity <= capacity_) {
			return;
		}

		Alloc alloc;
		auto const newData = AllocTraits::allocate(alloc, newCapacity);
		try {
			if constexpr (core::isNothrowMoveConstructible<T> || !core::isCopyConstructible<T>) {
				core::uninitializedMove(data_, data_ + size_, newData);
			} else {
				core::uninitializedCopy(data_, data_ + size_, newData);
			}
		} catch (...) {
			AllocTraits::deallocate(alloc, newData, newCapacity);
			throw;
		}

		if (data_) {
			clear();
			AllocTraits::deallocate(alloc, data_, capacity_);
		}
		data_ = newData;
		capacity_ = newCapacity;
	}

	void shrinkToFit() {
	}

	constexpr void swap(Vector& rhs) noexcept {
		core::swap(size_, rhs.size_);
		core::swap(capacity_, rhs.capacity_);
		core::swap(data_, rhs.data_);
	}

private:
	usize size_ = 0;
	usize capacity_ = 0;
	T* data_ = nullptr;
};

} // namespace rb::containers
