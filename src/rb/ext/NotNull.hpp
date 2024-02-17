#pragma once

#include <initializer_list>

#include <rb/core/assert.hpp>
#include <rb/core/in_place.hpp>
#include <rb/core/traits/assignable.hpp>
#include <rb/core/traits/constructible.hpp>
#include <rb/core/traits/destructible.hpp>
#include <rb/core/traits/IsConvertible.hpp>
#include <rb/ext/Nullable.hpp>

namespace rb::ext {

template <class T>
class NotNull;

namespace impl {

	template <class T, class U>
	using ConvertsFromNotNull = core::Or<
	    core::IsConstructible<T, NotNull<U> const&>,
	    core::IsConstructible<T, NotNull<U>&>,
	    core::IsConstructible<T, NotNull<U> const&&>,
	    core::IsConstructible<T, NotNull<U>&&>,
	    core::IsConvertible<NotNull<U> const&, T>,
	    core::IsConvertible<NotNull<U>&, T>,
	    core::IsConvertible<NotNull<U> const&&, T>,
	    core::IsConvertible<NotNull<U>&&, T>>;

	template <class T, class U>
	using AssignsFromNotNull = core::Or<
	    core::IsAssignable<T&, NotNull<U> const&>,
	    core::IsAssignable<T&, NotNull<U>&>,
	    core::IsAssignable<T&, NotNull<U> const&&>,
	    core::IsAssignable<T&, NotNull<U>&&>>;

} // namespace impl

template <class T>
class NotNull final {
	static_assert(Nullable<T>::value, "NotNull makes sense only for nullable types");

	using N = Nullable<T>;

	template <class U>
	using NotSelf = core::Not<core::IsSame<NotNull, core::RemoveCvRef<U>>>;

public:
#define RB_ENSURE_NONNULL RB_ASSERT(!N::isNull(value_))

	template <bool _ = true, RB_REQUIRES(_&& core::isDefaultConstructible<T> && !N::kDefaultIsNull)>
	constexpr NotNull() noexcept(core::isNothrowDefaultConstructible<T>)
	    : value_() {
		RB_ENSURE_NONNULL;
	}

	// NOLINTBEGIN(bugprone-forwarding-reference-overload,google-explicit-constructor)
	template <class U = T,
	    RB_REQUIRES_T(core::And<NotSelf<U>, core::IsConstructible<T, U>, core::IsConvertible<T, U>>)>
	constexpr NotNull(U&& value) noexcept(core::isNothrowConstructible<T, U>)
	    : value_(RB_FWD(value)) {
		RB_ENSURE_NONNULL;
	}

	template <class U = T,
	    RB_REQUIRES_T(core::And<NotSelf<U>, core::IsConstructible<T, U>, core::Not<core::IsConvertible<T, U>>>)>
	constexpr explicit NotNull(U&& value) noexcept(core::isNothrowConstructible<T, U>)
	    : value_(RB_FWD(value)) {
		RB_ENSURE_NONNULL;
	}

	template <class U,
	    RB_REQUIRES_T(core::And<
	        core::Not<core::IsSame<T, U>>,
	        core::IsConstructible<T, U const&>,
	        core::IsConvertible<U const&, T>,
	        core::Not<impl::ConvertsFromNotNull<T, U>>>)>
	constexpr NotNull(NotNull<U> const& rhs) noexcept(core::isNothrowConstructible<T, U const&>)
	    : value_(rhs.value_) {
		RB_ENSURE_NONNULL;
	}

	template <class U,
	    RB_REQUIRES_T(core::And<
	        core::Not<core::IsSame<T, U>>,
	        core::IsConstructible<T, U const&>,
	        core::Not<core::IsConvertible<U const&, T>>,
	        core::Not<impl::ConvertsFromNotNull<T, U>>>)>
	constexpr explicit NotNull(NotNull<U> const& rhs) noexcept(core::isNothrowConstructible<T, U const&>)
	    : value_(rhs.value_) {
		RB_ENSURE_NONNULL;
	}

	template <class U,
	    RB_REQUIRES_T(core::And<
	        core::Not<core::IsSame<T, U>>,
	        core::IsConstructible<T, U>,
	        core::IsConvertible<U, T>,
	        core::Not<impl::ConvertsFromNotNull<T, U>>>)>
	constexpr NotNull(NotNull<U>&& rhs) noexcept(core::isNothrowConstructible<T, U>)
	    : value_(RB_MOVE(rhs.value_)) {
		RB_ENSURE_NONNULL;
	}

	// NOLINTEND(bugprone-forwarding-reference-overload,google-explicit-constructor)

	template <class U,
	    RB_REQUIRES_T(core::And<
	        core::Not<core::IsSame<T, U>>,
	        core::IsConstructible<T, U>,
	        core::Not<core::IsConvertible<U, T>>,
	        core::Not<impl::ConvertsFromNotNull<T, U>>>)>
	constexpr explicit NotNull(NotNull<U>&& rhs) noexcept(core::isNothrowConstructible<T, U>)
	    : value_(RB_MOVE(rhs.value_)) {
		RB_ENSURE_NONNULL;
	}

	template <class... Args,
	    RB_REQUIRES(core::isConstructible<T, Args...>)>
	constexpr explicit NotNull(core::InPlace /*kInPlace*/, Args&&... args)
	    noexcept(core::isNothrowConstructible<T, Args...>)
	    : value_(RB_FWD(args)...) {
		RB_ENSURE_NONNULL;
	}

	// clang-format off
	template <class U, class... Args,
	    RB_REQUIRES(core::isConstructible<T, std::initializer_list<U>&, Args...>)>
	constexpr explicit NotNull(core::InPlace /*kInPlace*/, std::initializer_list<U> il, Args&&... args)
		noexcept(core::isNothrowConstructible<T, std::initializer_list<U>&, Args...>)
		: value_(il, RB_FWD(args)...) {
		RB_ENSURE_NONNULL;
	}

	// clang-format on

	template <bool _ = true, RB_REQUIRES(_&& core::isPointer<T>)>
	NotNull(std::nullptr_t) = delete;

	constexpr NotNull(NotNull const&) noexcept(core::isNothrowCopyConstructible<T>) = default;
	constexpr NotNull(NotNull&&) noexcept(core::isNothrowMoveConstructible<T>) = default;
	~NotNull() noexcept(core::isNothrowDestructible<T>) = default;

	constexpr NotNull& operator=(NotNull const&) noexcept(core::isNothrowCopyAssignable<T>) = default;
	constexpr NotNull& operator=(NotNull&&) noexcept(core::isNothrowMoveAssignable<T>) = default;

	template <
	    class U = T,
	    RB_REQUIRES_T(core::And<NotSelf<U>, core::IsConstructible<T, U>, core::IsAssignable<T&, U>>)>
	NotNull& operator=(U&& rhs) noexcept(core::isNothrowConstructible<T, U> && core::isNothrowAssignable<T&, U>) {
		value_ = RB_FWD(rhs);
		RB_ENSURE_NONNULL;
		return *this;
	}

	template <
	    class U,
	    RB_REQUIRES_T(core::And<
	        core::Not<core::IsSame<T, U>>,
	        core::IsConstructible<T, U const&>,
	        core::IsAssignable<T&, U const&>,
	        core::Not<impl::ConvertsFromNotNull<T, U>>,
	        core::Not<impl::AssignsFromNotNull<T, U>>>)>
	NotNull& operator=(NotNull<U> const& rhs) //
	    noexcept(core::isNothrowConstructible<T, U const&> && core::isNothrowAssignable<T&, U const&>) {
		value_ = rhs.value_;
		RB_ENSURE_NONNULL;
		return *this;
	}

	template <class U,
	    RB_REQUIRES_T(core::And<
	        core::Not<core::IsSame<T, U>>,
	        core::IsConstructible<T, U>,
	        core::IsAssignable<T&, U>,
	        core::Not<impl::ConvertsFromNotNull<T, U>>,
	        core::Not<impl::AssignsFromNotNull<T, U>>>)>
	NotNull& operator=(NotNull<U>&& rhs)
	    noexcept(core::isNothrowConstructible<T, U> && core::isNothrowAssignable<T&, U>) {
		value_ = RB_FWD(rhs.value_);
		RB_ENSURE_NONNULL;
		return *this;
	}

	template <bool _ = true, RB_REQUIRES(_&& core::isPointer<T>)>
	void operator=(std::nullptr_t) = delete;

	constexpr T& get() & noexcept {
		RB_ENSURE_NONNULL;
		return value_;
	}

	constexpr T const& get() const& noexcept {
		RB_ENSURE_NONNULL;
		return value_;
	}

	constexpr T&& get() && noexcept {
		RB_ENSURE_NONNULL;
		return value_;
	}

	constexpr T const&& get() const&& noexcept {
		RB_ENSURE_NONNULL;
		return value_;
	}

#undef RB_ENSURE_NONNULL

	// NOLINTBEGIN(google-explicit-constructor)

	constexpr operator T&() & noexcept {
		return get();
	}

	constexpr operator T const&() const& noexcept {
		return get();
	}

	constexpr operator T&&() && noexcept {
		return get();
	}

	constexpr operator T const&&() const&& noexcept {
		return get();
	}

	// NOLINTEND(google-explicit-constructor)

private:
	T value_;
};

} // namespace rb::ext
