#pragma once

#include <new>

#include <rb/core/attributes.hpp>
#include <rb/core/helpers.hpp>
#include <rb/core/memory/AllocationResult.hpp>
#include <rb/core/traits/detection.hpp>

namespace rb::core {

namespace impl {

	template <class T, class... Args>
	using OperatorNewDetector = decltype(T::operator new(RB_DECLVAL(Args)...));

	template <class T, class... Args>
	using OperatorNewArrayDetector = decltype(T::operator new[](RB_DECLVAL(Args)...));

	template <class T, class... Args>
	using OperatorDeleteDetector = decltype(T::operator delete(RB_DECLVAL(Args)...));

	template <class T, class... Args>
	using OperatorDeleteArrayDetector = decltype(T::operator delete[](RB_DECLVAL(Args)...));

} // namespace impl

template <class T, class... Args>
using HasOperatorNew = IsDetectedExact<void*, impl::OperatorNewDetector, T, usize, Args...>;

template <class T, class... Args>
inline constexpr bool hasOperatorNew = HasOperatorNew<T, Args...>::value;

template <class T, class... Args>
using HasOperatorNewArray = IsDetectedExact<void*, impl::OperatorNewArrayDetector, T, usize, Args...>;

template <class T, class... Args>
inline constexpr bool hasOperatorNewArray = HasOperatorNewArray<T, Args...>::value;

template <class T, class... Args>
using HasOperatorDelete = IsDetectedExact<void, impl::OperatorDeleteDetector, T, void*, Args...>;

template <class T, class... Args>
inline constexpr bool hasOperatorDelete = HasOperatorDelete<T, Args...>::value;

template <class T, class... Args>
using HasOperatorDeleteArray = IsDetectedExact<void, impl::OperatorDeleteArrayDetector, T, void*, Args...>;

template <class T, class... Args>
inline constexpr bool hasOperatorDeleteArray = HasOperatorDeleteArray<T, Args...>::value;

template <class T,
    bool useClassSpecificNewDelete = hasOperatorNewArray<T> && hasOperatorDeleteArray<T>>
struct ArrayAllocator final {
	using Value = T;
	using Size = usize;
	using Difference = isize;

	constexpr ArrayAllocator() noexcept = default;
	constexpr ArrayAllocator(ArrayAllocator const&) noexcept = default;

	// ReSharper disable once CppNonExplicitConvertingConstructor
	template <class U, bool _>
	constexpr ArrayAllocator(ArrayAllocator<U, _> const& /*rhs*/) noexcept { // NOLINT(google-explicit-constructor)
	}

	/// Allocates `n * sizeof(T)` bytes of uninitialized storage.
	[[nodiscard]] RB_ALLOCATOR RB_RETURNS_NONNULL static T* allocate(usize n) RB_ALLOC_SIZE(1) {
		RB_CHECK_COMPLETENESS(T);
		if (n > static_cast<usize>(-1) / sizeof(T)) {
			throw std::bad_array_new_length();
		}

		auto const nbBytes = n * sizeof(T);
		void* ptr = nullptr;
#ifdef __cpp_aligned_new
		auto const align = std::align_val_t{alignof(T)};
		if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
			if constexpr (useClassSpecificNewDelete) {
				if constexpr (hasOperatorNewArray<T, std::align_val_t>) {
					ptr = T::operator new[](nbBytes, align);
				} else {
					ptr = T::operator new[](nbBytes);
				}
			} else {
				ptr = ::operator new[](nbBytes, align);
			}
		} else
#endif
		{
			if constexpr (useClassSpecificNewDelete) {
				ptr = T::operator new[](nbBytes);
			} else {
				ptr = ::operator new[](nbBytes);
			}
		}
		return static_cast<T*>(ptr);
	}

	/// Allocates `count * sizeof(T)` bytes of uninitialized storage,
	/// where `count` is an unspecified integer value not less than `n`,
	/// by calling `::operator new` (an additional `std::align_val_t` argument might be provided).
	[[nodiscard]] static AllocationResult<T*> allocateAtLeast(usize n) {
		return {allocate(n), n};
	}

	/// Deallocates the storage referenced by the pointer @p ptr,
	/// which must be a pointer obtained by an earlier call to allocate().
	static void deallocate(T* ptr, [[maybe_unused]] usize n) noexcept {
#ifdef __cpp_sized_deallocation
		auto const nbBytes = n * sizeof(T);
#endif
#ifdef __cpp_aligned_new
		auto const align = std::align_val_t{alignof(T)};
		if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
			if constexpr (useClassSpecificNewDelete) {
	#ifdef __cpp_sized_deallocation
				if constexpr (hasOperatorDeleteArray<T, usize, std::align_val_t>
				              && !hasOperatorDeleteArray<T, std::align_val_t>) {
					T::operator delete[](ptr, nbBytes, align);
				} else
	#endif
				{
					if constexpr (hasOperatorDeleteArray<T, std::align_val_t>) {
						T::operator delete[](ptr, align);
					} else {
						T::operator delete[](ptr);
					}
				}
			} else {
				::operator delete[](ptr,
	#ifdef __cpp_sized_deallocation
				    nbBytes,
	#endif
				    align);
			}
		} else
#endif // __cpp_aligned_new
		{
			if constexpr (useClassSpecificNewDelete) {
#ifdef __cpp_sized_deallocation
				if constexpr (hasOperatorDeleteArray<T, usize>) {
					T::operator delete[](ptr, nbBytes);
				} else
#endif
				{
					T::operator delete[](ptr);
				}
			} else { // useClassSpecificNewDelete
				::operator delete[](ptr
#ifdef __cpp_sized_deallocation
				    ,
				    nbBytes
#endif
				);
			}
		}
	}
}; // namespace rb::core

template <class T, bool _1, class U, bool _2> // NOLINT(readability-identifier-naming)
constexpr bool operator==(ArrayAllocator<T, _1> const& /*lhs*/, ArrayAllocator<U, _2> const& /*rhs*/) noexcept {
	return true;
}

template <class T, bool _1, class U, bool _2> // NOLINT(readability-identifier-naming)
constexpr bool operator!=(ArrayAllocator<T, _1> const& /*lhs*/, ArrayAllocator<U, _2> const& /*rhs*/) noexcept {
	return false;
}

} // namespace rb::core
