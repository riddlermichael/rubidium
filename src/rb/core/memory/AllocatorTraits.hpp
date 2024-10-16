#pragma once

#include <rb/core/limits.hpp>
#include <rb/core/memory/AllocationResult.hpp>
#include <rb/core/memory/construct.hpp>
#include <rb/core/memory/destroy.hpp>
#include <rb/core/memory/PointerTraits.hpp>
#include <rb/core/traits/builtins.hpp>
#include <rb/core/traits/Unsigned.hpp>

namespace rb::core {

namespace impl {

	RB_TYPE_DETECTOR(Pointer)
	RB_TYPE_DETECTOR(ConstPointer)
	RB_TYPE_DETECTOR(VoidPointer)
	RB_TYPE_DETECTOR(ConstVoidPointer)
	RB_TYPE_DETECTOR(Size)
	RB_TYPE_DETECTOR(IsAlwaysEqual)
	RB_METHOD_DETECTOR_NAME(allocate, AllocateMethod)
	RB_METHOD_DETECTOR_NAME(allocateAtLeast, AllocateAtLeastMethod)
	RB_METHOD_DETECTOR_NAME(construct, ConstructMethod)
	RB_METHOD_DETECTOR_NAME(destroy, DestroyMethod)
	RB_METHOD_DETECTOR_NAME(maxSize, MaxSizeMethod)

	template <class A, class T, class = void, class = void>
	struct RebindAllocImpl {};

	template <class A, class T>
	struct RebindAllocImpl<A, T, Void<typename A::template Rebind<T>>, void> {
		using Type = typename A::template Rebind<T>;
	};

	template <class A, class T, class _>
	struct RebindAllocImpl<A, T, _, Void<ReplaceFirstType<A, T>>> {
		using Type = ReplaceFirstType<A, T>;
	};

} // namespace impl

inline namespace memory {

	template <class Alloc>
	struct AllocatorTraits {
		using Allocator = Alloc;
		using Value = typename Alloc::Value;
		using Pointer = DetectedOrType<Value*, impl::PointerDetector, Alloc>;

	private:
		using PT = PointerTraits<Pointer>;

		template <template <class> class Op, class T, class = void>
		struct DetectOrRebindImpl {
			using Type = typename PT::template Rebind<T>;
		};

		template <template <class> class Op, class T>
		struct DetectOrRebindImpl<Op, T, Void<Op<Allocator>>> {
			using Type = Op<Allocator>;
		};

		template <template <class> class Op, class T>
		using DetectOrRebind = typename DetectOrRebindImpl<Op, T>::Type;

	public:
		using ConstPointer = DetectOrRebind<impl::ConstPointerDetector, Value const>;
		using VoidPointer = DetectOrRebind<impl::VoidPointerDetector, void>;
		using ConstVoidPointer = DetectOrRebind<impl::ConstVoidPointerDetector, void const>;
		using Difference = DetectedOrType<typename PT::Difference, impl::DifferenceDetector, Alloc>;
		using Size = DetectedOrType<Unsigned<Difference>, impl::SizeDetector, Alloc>;
		using IsAlwaysEqual = DetectedOrType<IsEmpty<Alloc>, impl::IsAlwaysEqualDetector, Alloc>;

		template <class T>
		using RebindAlloc = typename impl::RebindAllocImpl<Alloc, T>::Type;

		template <class T>
		using RebindTraits = AllocatorTraits<RebindAlloc<T>>;

		[[nodiscard]] static constexpr Pointer allocate(Alloc& a, Size n) {
			return a.allocate(n);
		}

		[[nodiscard]] static constexpr Pointer allocate(Alloc& a, Size n, ConstVoidPointer hint) {
			if constexpr (impl::HasAllocateMethod<Alloc, Size, ConstVoidPointer>::value) {
				return a.allocate(n, hint);
			} else {
				return a.allocate(n);
			}
		}

		[[nodiscard]] static constexpr AllocationResult<Pointer, Size> allocateAtLeast(Alloc& a, Size n) {
			using Result = AllocationResult<Pointer, Size>;
			if constexpr (isDetectedConvertible<Result, impl::AllocateAtLeastMethodDetector, Alloc, Size>) {
				return a.allocateAtLeast(n);
			} else {
				return {a.allocate(n), n};
			}
		}

		template <class T, class... Args>
		static constexpr void construct(Alloc& a, T* ptr, Args&&... args) {
			if constexpr (impl::HasConstructMethod<Alloc, T*, Args...>::value) {
				a.construct(ptr, RB_FWD(args)...);
			} else {
				core::construct(ptr, RB_FWD(args)...);
			}
		}

		static constexpr void deallocate(Alloc& a, Pointer p, Size n) {
			a.deallocate(p, n);
		}

		static constexpr void deallocate(Alloc& a, AllocationResult<Pointer, Size> result) {
			a.deallocate(result.ptr, result.count);
		}

		template <class T>
		static constexpr void destroy(Alloc& a, T* ptr) {
			if constexpr (impl::HasDestroyMethod<Alloc, T*>::value) {
				return a.destroy(ptr);
			} else {
				return core::destroy(ptr);
			}
		}

		static constexpr Size maxSize(Alloc const& a) noexcept {
			if constexpr (impl::HasMaxSizeMethod<Alloc>::value) {
				return a.maxSize();
			} else {
				return max<Size> / sizeof(Value);
			}
		}
	};

	template <class ForwardIt, class Alloc>
	void destroy(ForwardIt first, ForwardIt last, Alloc& alloc) {
		for (; first != last; ++first) {
			AllocatorTraits<Alloc>::destroy(alloc, addressOf(*first));
		}
	}

} // namespace memory
} // namespace rb::core
