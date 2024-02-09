#pragma once

#include <ostream>

#include <rb/core/assert.hpp>
#include <rb/core/memory/CompressedPair.hpp>
#include <rb/core/memory/DefaultDeleter.hpp>
#include <rb/core/memory/helpers.hpp>
#include <rb/core/traits/arrays.hpp>
#include <rb/core/traits/assignable.hpp>
#include <rb/core/traits/IsRef.hpp>

namespace rb::core {

namespace impl {

	template <class T, class D, class = void>
	struct DeleterPointerType {
		using Type = T*;
	};

	template <class T, class D>
	struct DeleterPointerType<T, D, Void<typename D::pointer>> {
		using Type = typename D::pointer;
	};

	template <class D>
	using DeleterEnableDefault = And<IsDefaultConstructible<D>, Not<IsPointer<D>>>;

} // namespace impl

inline namespace memory {

	template <class T>
	class OwnerPtr;

	/**
	 * UniquePtr is a smart pointer that owns and manages another object through a pointer
	 * and disposes of that object when the UniquePtr goes out of scope.
	 *
	 * @tparam T The type of the managed object, aliased as member type `element_type`.
	 * @tparam D Type of the callable object used as *deleter*, aliased as member type `deleter_type`.
	 * By default, this is DefaultDeleter, which is a stateless functor
	 * that simply uses the global `delete` for the operation,
	 * with class causing no additional overhead overall over a built-in pointer.<br>
	 * **NOTE:** @p D should not be `final` in order to use
	 * [empty base optimization](https://en.cppreference.com/w/cpp/language/ebo).
	 */
	template <class T, class D = DefaultDeleter>
	class UniquePtr final {
		static_assert(!isArray<T>, "UniquePtr<T[]> is forbidden");

	public:
		// NOLINTBEGIN(readability-identifier-naming)

		/// `RemoveRef<D>::pointer` if that type exists, otherwise `T*`.
		/// Must satisfy [NullablePointer](https://en.cppreference.com/w/cpp/named_req/NullablePointer).
		using pointer = typename impl::DeleterPointerType<T, RemoveRef<D>>::Type;

		/// @p T, the type of the object managed by this UniquePtr.
		using element_type = T;

		/// @p D, the function object or lvalue reference to function or to function object,
		/// to be called from the destructor.
		using deleter_type = D;

		// NOLINTEND(readability-identifier-naming)

	private:
		template <class U>
		using SafeConvertible = IsConvertible<typename UniquePtr<U, D>::pointer, pointer>;

	public:

		static auto borrow(pointer& p) noexcept
		    -> RB_REQUIRES_RETURN_T(UniquePtr, And<impl::DeleterEnableDefault<D>, impl::SafeDelete<T>>) {
			return UniquePtr(p);
		}

		static auto borrow(pointer& p, D const& d) noexcept
		    -> RB_REQUIRES_RETURN_T(UniquePtr, And<IsCopyConstructible<D>, impl::SafeDelete<T>>) {
			return UniquePtr(p, d);
		}

		static auto borrow(pointer& p, D&& d) noexcept
		    -> RB_REQUIRES_RETURN_T(UniquePtr, And<IsMoveConstructible<D>, Not<IsLValueRef<D>>, impl::SafeDelete<T>>) {
			return UniquePtr(p, RB_MOVE(d));
		}

		template <class... Args,
		    RB_REQUIRES(isConstructible<T, Args...>)>
		static UniquePtr from(Args&&... args) RB_NOEXCEPT_LIKE(UniquePtr(kInPlace, RB_FWD(args)...)) {
			return UniquePtr(kInPlace, RB_FWD(args)...);
		}

		template <class... Args>
		static auto with(D const& d, Args&&... args)
		    -> RB_REQUIRES_RETURN(UniquePtr, isConstructible<T, Args...>&& isCopyConstructible<D>) {
			pointer p = new T(RB_FWD(args)...);
			return UniquePtr(p, d);
		}

		template <class... Args>
		static auto with(D&& d, Args&&... args)
		    -> RB_REQUIRES_RETURN(UniquePtr, isConstructible<T, Args...>&& isMoveConstructible<D> && !isLValueRef<D>) {
			pointer p = new T(RB_FWD(args)...);
			return UniquePtr(p, RB_MOVE(d));
		}

		template <class DD = D, RB_REQUIRES_T(impl::DeleterEnableDefault<DD>)>
		constexpr UniquePtr() noexcept
		    : storage_(kInPlaceIndex<0>) {
		}

		// NOLINTBEGIN(google-explicit-constructor)

		template <class DD = D, RB_REQUIRES_T(impl::DeleterEnableDefault<DD>)>
		constexpr UniquePtr(std::nullptr_t /*nullptr*/) noexcept
		    : UniquePtr() {
		}

		template <class DD = D, RB_REQUIRES(isMoveConstructible<DD>)>
		UniquePtr(UniquePtr&& rhs) noexcept
		    : storage_(kInPlaceIndex<1>, RB_FWD(rhs.deleter()), rhs.release()) {
		}

		template <class U,
		    class DD = D, RB_REQUIRES_T(And<IsMoveConstructible<DD>, SafeConvertible<U>>)>
		UniquePtr(UniquePtr<U, D>&& rhs) noexcept
		    : storage_(kInPlaceIndex<1>, RB_FWD(rhs.deleter()), rhs.release()) {
		}

		// NOLINTEND(google-explicit-constructor)

		~UniquePtr() {
			reset();
		}

		RB_DISABLE_COPY(UniquePtr);

		// NOLINTBEGIN(*-c-copy-assignment-signature,*-unconventional-assign-operator)

		auto operator=(UniquePtr&& rhs) noexcept
		    -> RB_REQUIRES_RETURN(UniquePtr&, isMoveAssignable<D>) {
			if (this != &rhs) {
				reset(rhs.release());
				deleter() = RB_FWD(rhs.deleter());
			}
			return *this;
		}

		template <class U>
		auto operator=(UniquePtr<U, D>&& rhs) noexcept
		    -> RB_REQUIRES_RETURN_T(UniquePtr&, And<IsMoveAssignable<D>, SafeConvertible<U>>) {
			reset(rhs.release());
			deleter() = RB_FWD(rhs.deleter());
			return *this;
		}

		// NOLINTEND(*-c-copy-assignment-signature,*-unconventional-assign-operator)

		UniquePtr& operator=(std::nullptr_t) noexcept {
			reset();
			return *this;
		}

		constexpr explicit operator bool() const noexcept {
			return ptr();
		}

#define RB_ENSURE_NONNULL RB_ASSERT_MSG(*this, "dereference null")

		AddLValueRef<T const> operator*() const RB_NOEXCEPT_LIKE(*ptr()) {
			RB_ENSURE_NONNULL;
			return *ptr();
		}

		AddLValueRef<T> operator*() RB_NOEXCEPT_LIKE(*ptr()) {
			RB_ENSURE_NONNULL;
			return *ptr();
		}

		constexpr pointer operator->() const noexcept {
			RB_ENSURE_NONNULL;
			return ptr();
		}

#undef RB_ENSURE_NONNULL

		constexpr D const& deleter() const noexcept {
			return storage_.second();
		}

		constexpr D& deleter() noexcept {
			return storage_.second();
		}

		void reset() noexcept {
			reset(nullptr);
		}

		constexpr void swap(UniquePtr& rhs) noexcept {
			static_assert(core::isSwappable<D>, "deleter must be swappable");
			storage_.swap(rhs.storage_);
		}

	private:
		template <class U>
		friend class OwnerPtr;

		template <class U, class _>
		friend class UniquePtr;

		using Storage = CompressedPair<pointer, deleter_type>;

		constexpr explicit UniquePtr(pointer& p) noexcept
		    : storage_(kInPlaceIndex<0>, p) {
			p = nullptr;
		}

		template <class Del>
		constexpr explicit UniquePtr(pointer& p, Del&& del) noexcept
		    : storage_(kInPlaceIndex<1>, RB_FWD(del), p) {
			p = nullptr;
		}

		template <class... Args,
		    class DD = D, RB_REQUIRES_T(impl::DeleterEnableDefault<DD>)>
		constexpr explicit UniquePtr(InPlace /*kInPlace*/, Args&&... args) RB_NOEXCEPT_LIKE(new T(RB_FWD(args)...))
		    : storage_(kInPlaceIndex<0>, new T(RB_FWD(args)...)) {
		}

		constexpr pointer const& ptr() const noexcept {
			return storage_.first();
		}

		constexpr pointer& ptr() noexcept {
			return storage_.first();
		}

		pointer release() noexcept {
			pointer const p = ptr();
			ptr() = nullptr;
			return p;
		}

		void reset(pointer p) noexcept {
			pointer const old = ptr();
			ptr() = p;
			if (old) {
				deleter()(old);
			}
		}

		Storage storage_;
	};

	template <class T, class... Args,
	    RB_REQUIRES_T(IsConstructible<T, Args...>)>
	UniquePtr<T> makeUnique(Args&&... args) RB_NOEXCEPT_LIKE(UniquePtr<T>::from(RB_FWD(args)...)) {
		return UniquePtr<T>::from(RB_FWD(args)...);
	}

	template <class T, class D, class... Args,
	    RB_REQUIRES_T(IsConstructible<T, Args...>)>
	auto makeUniqueWithDeleter(D&& d, Args&&... args)
	    RB_NOEXCEPT_LIKE(UniquePtr<T, RemoveCvRef<D>>::with(RB_FWD(d), RB_FWD(args)...)) {
		return UniquePtr<T, RemoveCvRef<D>>::with(RB_FWD(d), RB_FWD(args)...);
	}

	template <class Char, class Traits, class T, class D,
	    RB_REQUIRES_T(IsWritableTo<AddLValueRef<T const>, std::basic_ostream<Char, Traits>>)>
	std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, UniquePtr<T, D> const& ptr) {
		return ptr
		         ? os << os.widen('&') << *ptr
		         : os << os.widen('n') << os.widen('u') << os.widen('l') << os.widen('l');
	}

} // namespace memory

template <class T, class D,
    RB_REQUIRES(isSwappable<D>)>
constexpr void swap(UniquePtr<T, D>& lhs, UniquePtr<T, D>& rhs) noexcept {
	lhs.swap(rhs);
}

} // namespace rb::core
