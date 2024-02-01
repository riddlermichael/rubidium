#pragma once

#include <ostream>

#include <rb/core/assert.hpp>
#include <rb/core/invoke.hpp>
#include <rb/core/memory/helpers.hpp>
#include <rb/core/traits/arrays.hpp>
#include <rb/core/traits/requirements.hpp>

namespace rb::core {
inline namespace memory {

	template <class T, class D>
	class UniquePtr;

	template <class T>
	class OwnerPtr final {
		static_assert(!isArray<T>, "OwnerPtr<T[]> is forbidden");

		// there should be `is_nothrow_invocable`, but we relax requirements for external deleters
		template <class D>
		using IsDeleter = And<IsMoveConstructible<D>, IsInvocable<D, T*>>;

		template <class U>
		using SafeConvertible = IsConvertible<U*, T*>;

		struct IDeleter {
			virtual ~IDeleter() = 0;

			virtual void operator()(T* ptr) noexcept = 0;
		};

		template <class D>
		struct Deleter : IDeleter {
			D d;

			explicit Deleter(D d)
			    : d(RB_MOVE(d)) {
			}

			void operator()(T* ptr) noexcept override {
				d(ptr);
			}
		};

	public:
		// NOLINTBEGIN(readability-identifier-naming)

		using pointer = T*;

		/// @p T, the type of the object managed by this OwnerPtr.
		using element_type = T;

		// NOLINTEND(readability-identifier-naming)

		static auto borrow(pointer& ptr) noexcept
		    -> RB_REQUIRES_RETURN_T(OwnerPtr, impl::SafeDelete<T>) {
			return OwnerPtr(ptr);
		}

		template <class D,
		    RB_REQUIRES_T(IsDeleter<D>)>
		static OwnerPtr borrow(pointer& ptr, D d) noexcept {
			return OwnerPtr(ptr, new Deleter<D>{RB_MOVE(d)});
		}

		template <class... Args,
		    RB_REQUIRES_T(IsConstructible<T, Args...>)>
		static OwnerPtr from(Args&&... args) RB_NOEXCEPT_LIKE(new T(RB_FWD(args)...)) {
			return OwnerPtr(new T(RB_FWD(args)...));
		}

		template <class D, class... Args,
		    RB_REQUIRES_T(And<IsDeleter<D>, IsConstructible<T, Args...>>)>
		static OwnerPtr with(D d, Args&&... args) RB_NOEXCEPT_LIKE(new T(RB_FWD(args)...)) {
			return OwnerPtr(new T(RB_FWD(args)...), new Deleter<D>{RB_MOVE(d)});
		}

		constexpr OwnerPtr() noexcept = default;

		// NOLINTBEGIN(google-explicit-constructor)

		constexpr OwnerPtr(std::nullptr_t) noexcept {
		}

		template <class U, class D,
		    RB_REQUIRES_T(SafeConvertible<U>)>
		constexpr OwnerPtr(UniquePtr<U, D>&& ptr) noexcept
		    : OwnerPtr(ptr.release(), new Deleter<D>{RB_MOVE(ptr.deleter())}) {
		}

		OwnerPtr(OwnerPtr const&) = delete;

		constexpr OwnerPtr(OwnerPtr&& rhs) noexcept
		    : ptr_(rhs.ptr_)
		    , d_(rhs.d_) {
			rhs.ptr_ = nullptr;
			rhs.d_ = nullptr;
		}

		template <class U,
		    RB_REQUIRES_T(SafeConvertible<U>)>
		constexpr OwnerPtr(OwnerPtr<U>&& rhs) noexcept
		    : ptr_(rhs.ptr_)
		    , d_(rhs.d_) {
			rhs.ptr_ = nullptr;
			rhs.d_ = nullptr;
		}

		// NOLINTEND(google-explicit-constructor)

		~OwnerPtr() {
			reset();
			delete d_;
		}

		OwnerPtr& operator=(OwnerPtr const&) = delete;

		OwnerPtr& operator=(OwnerPtr&& rhs) noexcept {
			if (this != &rhs) {
				this->~OwnerPtr();
				new (this) OwnerPtr(RB_MOVE(rhs));
			}
			return *this;
		}

		template <class U,
		    RB_REQUIRES_T(SafeConvertible<U>)>
		OwnerPtr& operator=(OwnerPtr<U>&& rhs) noexcept {
			this->~OwnerPtr();
			new (this) OwnerPtr(RB_MOVE(rhs));
			return *this;
		}

		template <class U, class D,
		    RB_REQUIRES_T(SafeConvertible<U>)>
		constexpr OwnerPtr& operator=(UniquePtr<U, D>&& ptr) noexcept {
			this->~OwnerPtr();
			new (this) OwnerPtr(RB_MOVE(ptr));
			return *this;
		}

		OwnerPtr& operator=(std::nullptr_t) noexcept {
			reset();
			return *this;
		}

		constexpr explicit operator bool() const noexcept {
			return ptr_;
		}

#define RB_ENSURE_NONNULL RB_ASSERT_MSG(*this, "dereference null")

		AddLValueRef<T const> operator*() const noexcept {
			RB_ENSURE_NONNULL;
			return *ptr_;
		}

		AddLValueRef<T> operator*() noexcept {
			RB_ENSURE_NONNULL;
			return *ptr_;
		}

		pointer operator->() const noexcept {
			RB_ENSURE_NONNULL;
			return ptr_;
		}

#undef RB_ENSURE_NONNULL

		void reset() noexcept {
			reset(nullptr);
		}

	private:
		constexpr explicit OwnerPtr(T*& ptr, IDeleter* d = nullptr) noexcept
		    : ptr_(ptr)
		    , d_(d) {
			ptr = nullptr;
		}

		constexpr explicit OwnerPtr(T*&& ptr, IDeleter* d = nullptr) noexcept
		    : ptr_(ptr)
		    , d_(d) {
			ptr = nullptr;
		}

		void destroy(T* ptr) noexcept {
			RB_CHECK_COMPLETENESS(T);
			if (!ptr) {
				return;
			}

			if (d_) {
				(*d_)(ptr);
			} else {
				delete ptr_;
			}
		}

		void reset(T* p) noexcept {
			T* const ptr = ptr_;
			ptr_ = p;
			destroy(ptr);
		}

		T* ptr_ = nullptr;
		IDeleter* d_ = nullptr;
	};

	template <class T>
	OwnerPtr<T>::IDeleter::~IDeleter() = default;

	template <class T, class... Args,
	    RB_REQUIRES_T(IsConstructible<T, Args...>)>
	OwnerPtr<T> makeOwner(Args&&... args) RB_NOEXCEPT_LIKE(OwnerPtr<T>::from(RB_FWD(args)...)) {
		return OwnerPtr<T>::from(RB_FWD(args)...);
	}

	template <class T, class D, class... Args,
	    RB_REQUIRES_T(IsConstructible<T, Args...>)>
	OwnerPtr<T> makeOwnerWithDeleter(D&& d, Args&&... args)
	    RB_NOEXCEPT_LIKE(OwnerPtr<T>::with(RB_FWD(d), RB_FWD(args)...)) {
		return OwnerPtr<T>::with(RB_FWD(d), RB_FWD(args)...);
	}

	template <class Char, class Traits, class T,
	    RB_REQUIRES_T(IsWritableTo<traits::AddLValueRef<T const>, std::basic_ostream<Char, Traits>>)>
	std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, OwnerPtr<T> const& ptr) {
		return ptr
		         ? os << os.widen('&') << *ptr
		         : os << os.widen('n') << os.widen('u') << os.widen('l') << os.widen('l');
	}

} // namespace memory
} // namespace rb::core
