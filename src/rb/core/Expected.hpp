#pragma once

#include <variant>

#include <rb/core/Option.hpp>

namespace rb::core {

// Ok
template <class T>
struct Ok;

template <class T>
struct Ok<T&> {
	T& value;
};

template <class T>
struct Ok<T&&> {
	T&& value;
};

template <>
struct Ok<void> {
};

template <class T>
constexpr auto ok(T& value) noexcept {
	return Ok<T const&>{value};
}

template <class T>
constexpr auto ok(T&& value) noexcept {
	return Ok<T&&>{static_cast<T&&>(value)};
}

constexpr auto ok() noexcept {
	return Ok<void>{};
}

// Err
template <class T>
struct Err;

template <class T>
struct Err<T&> {
	T& value;
};

template <class T>
struct Err<T&&> {
	T&& value;
};

template <class E>
constexpr auto err(E& value) noexcept {
	return Err<E const&>{value};
}

template <class E>
constexpr auto err(E&& value) noexcept {
	return Err<E&&>{static_cast<E&&>(value)};
}

template <class T, class E>
class Expected;

} // namespace rb::core

namespace rb::core {

namespace impl {

	template <class>
	struct ExpectedValueTypeHelper;

	template <class T, class E>
	struct ExpectedValueTypeHelper<Expected<T, E>> {
		using Type = T;
	};

	template <class T>
	using ExpectedValueType = typename ExpectedValueTypeHelper<Decay<T>>::Type;

	template <class Exp>
	struct IsExpected : False {};

	template <class T, class E>
	struct IsExpected<Expected<T, E>> : True {};

	template <
	    class Exp,
	    class F,
	    RB_REQUIRES(!isVoid<ExpectedValueType<Exp>>),
	    class R = decltype(invoke(RB_DECLVAL(F), RB_DECLVAL(Exp).value()))>
	constexpr R andThen(Exp&& exp, F&& f) {
		if (exp.hasValue()) {
			return invoke(RB_FWD(f), RB_FWD(exp).value());
		}
		return err(RB_FWD(exp).error());
	}

	template <
	    class Exp,
	    class F,
	    RB_REQUIRES(isVoid<ExpectedValueType<Exp>>),
	    class R = decltype(invoke(RB_DECLVAL(F)))>
	constexpr R andThen(Exp&& exp, F&& f) {
		if (exp.hasValue()) {
			return invoke(RB_FWD(f));
		}
		return err(RB_FWD(exp).error());
	}

	template <class Template, class U>
	struct ReplaceFirstTypeHelper;

	template <template <class, class...> class Template, class T, class... Args, class U>
	struct ReplaceFirstTypeHelper<Template<T, Args...>, U> {
		using Type = Template<U, Args...>;
	};

	template <class Template, class U>
	using ReplaceFirstTypeUnqual = typename ReplaceFirstTypeHelper<RemoveCvRef<Template>, U>::Type;

	template <
	    class Exp,
	    class F,
	    RB_REQUIRES(!isVoid<ExpectedValueType<Exp>>),
	    class U = decltype(invoke(RB_DECLVAL(F), *RB_DECLVAL(Exp))),
	    RB_REQUIRES(!isVoid<U>)>
	constexpr auto map(Exp&& exp, F&& f) {
		using R = ReplaceFirstTypeUnqual<Exp, Decay<U>>;
		return exp.hasValue() ? R{invoke(RB_FWD(f), *RB_FWD(exp))} : err(RB_FWD(exp).error());
	}

	template <
	    class Exp,
	    class F,
	    RB_REQUIRES(!isVoid<ExpectedValueType<Exp>>),
	    class U = decltype(invoke(RB_DECLVAL(F), *RB_DECLVAL(Exp))),
	    RB_REQUIRES(isVoid<U>)>
	constexpr auto map(Exp&& exp, F&& f) {
		using R = ReplaceFirstTypeUnqual<Exp, Decay<U>>;
		if (exp.hasValue()) {
			invoke(RB_FWD(f), *RB_FWD(exp));
			return R{};
		}
		return err(RB_FWD(exp).error());
	}

	template <
	    class Exp,
	    class F,
	    RB_REQUIRES(isVoid<ExpectedValueType<Exp>>),
	    class U = decltype(invoke(RB_DECLVAL(F))),
	    RB_REQUIRES(!isVoid<U>)>
	constexpr auto map(Exp&& exp, F&& f) {
		using R = ReplaceFirstTypeUnqual<Exp, Decay<U>>;
		return exp.hasValue() ? R{invoke(RB_FWD(f))} : err(RB_FWD(exp).error());
	}

	template <
	    class Exp,
	    class F,
	    RB_REQUIRES(isVoid<ExpectedValueType<Exp>>),
	    class U = decltype(invoke(RB_DECLVAL(F))),
	    RB_REQUIRES(isVoid<U>)>
	constexpr auto map(Exp&& exp, F&& f) {
		using R = ReplaceFirstTypeUnqual<Exp, Decay<U>>;
		if (exp.hasValue()) {
			invoke(RB_FWD(f));
			return R{};
		}
		return err(RB_FWD(exp).error());
	}

	template <
	    class Exp,
	    class F,
	    class U = decltype(invoke(RB_DECLVAL(F), RB_DECLVAL(Exp).error())),
	    RB_REQUIRES(IsExpected<Decay<U>>::value)>
	constexpr auto orElse(Exp&& exp, F&& f) {
		if (exp.hasError()) {
			return invoke(RB_FWD(f), RB_FWD(exp).error());
		}
		return RB_FWD(exp);
	}

	template <
	    class Exp,
	    class F,
	    class U = decltype(invoke(RB_DECLVAL(F), RB_DECLVAL(Exp).error())),
	    RB_REQUIRES(isVoid<U>)>
	constexpr auto orElse(Exp&& exp, F&& f) {
		if (exp.hasError()) {
			invoke(RB_FWD(f), RB_FWD(exp).error());
		}
		return RB_FWD(exp);
	}

} // namespace impl

// Expected
template <class T, class E>
class RB_EXPORT [[nodiscard]] Expected final {
	static_assert(!isVoid<E>, "Error type must not be void");

	using Impl = std::variant<T, E>;

public:
	// NOLINTBEGIN(google-explicit-constructor)

	template <bool _ = true, RB_REQUIRES(isDefaultConstructible<T>)>
	constexpr Expected() {}

	constexpr Expected(T const& value) RB_NOEXCEPT_LIKE(Expected(std::in_place_index<0>, value))
	    : Expected(std::in_place_index<0>, value) {
	}

	constexpr Expected(T&& value) RB_NOEXCEPT_LIKE(Expected(std::in_place_index<0>, RB_MOVE(value)))
	    : Expected(std::in_place_index<0>, RB_MOVE(value)) {
	}

	constexpr Expected(Ok<T const&> ok) RB_NOEXCEPT_LIKE(Expected(std::in_place_index<0>, ok.value))
	    : Expected(std::in_place_index<0>, ok.value) {
	}

	constexpr Expected(Ok<T&&> ok) RB_NOEXCEPT_LIKE(Expected(std::in_place_index<0>, RB_MOVE(ok.value)))
	    : Expected(std::in_place_index<0>, RB_MOVE(ok.value)) {
	}

	constexpr Expected(Err<E const&> err) RB_NOEXCEPT_LIKE(Expected(std::in_place_index<1>, err.value))
	    : Expected(std::in_place_index<1>, err.value) {
	}

	constexpr Expected(Err<E&&> err) RB_NOEXCEPT_LIKE(Expected(std::in_place_index<1>, RB_MOVE(err.value)))
	    : Expected(std::in_place_index<1>, RB_MOVE(err.value)) {
	}

	// NOLINTEND(google-explicit-constructor)

	constexpr Expected(Expected const& rhs) noexcept(isNothrowCopyConstructible<Impl>) = default;
	constexpr Expected(Expected&& rhs) noexcept(isNothrowMoveConstructible<Impl>) = default;
	~Expected() = default;

	constexpr Expected& operator=(Expected const& rhs) noexcept(isNothrowCopyAssignable<Impl>) = default;
	constexpr Expected& operator=(Expected&& rhs) noexcept(isNothrowMoveAssignable<Impl>) = default;

	constexpr Expected& operator=(T const& value) noexcept(isNothrowAssignable<Impl, T const&>) {
		impl_ = value;
		return *this;
	}

	constexpr Expected& operator=(T&& value) RB_NOEXCEPT_LIKE(RB_DECLVAL(Impl&) = RB_DECLVAL(T&&)) {
		impl_ = RB_MOVE(value);
		return *this;
	}

	void swap(Expected& rhs) noexcept(isNothrowSwappable<Impl>) {
		impl_.swap(rhs.impl_);
	}

#define RB_ENSURE_VALUE RB_ASSERT_MSG(impl_.index() == 0, "Expected doesn't contain a value")
#define RB_ENSURE_ERROR RB_ASSERT_MSG(impl_.index() == 1, "Expected doesn't contain an error")

	constexpr T& value() & noexcept {
		RB_ENSURE_VALUE;
		return std::get<0>(impl_);
	}

	constexpr T&& value() && noexcept {
		RB_ENSURE_VALUE;
		return std::get<0>(RB_MOVE(impl_));
	}

	constexpr T const& value() const& noexcept {
		RB_ENSURE_VALUE;
		return std::get<0>(impl_);
	}

	constexpr T const&& value() const&& noexcept {
		RB_ENSURE_VALUE;
		return std::get<0>(RB_MOVE(impl_));
	}

	constexpr T& unwrap() & noexcept {
		RB_ENSURE_VALUE;
		return std::get<0>(impl_);
	}

	constexpr T&& unwrap() && noexcept {
		RB_ENSURE_VALUE;
		return std::get<0>(RB_MOVE(impl_));
	}

	constexpr T const& unwrap() const& noexcept {
		RB_ENSURE_VALUE;
		return std::get<0>(impl_);
	}

	constexpr T const&& unwrap() const&& noexcept {
		RB_ENSURE_VALUE;
		return std::get<0>(RB_MOVE(impl_));
	}

	constexpr E& error() & noexcept {
		RB_ENSURE_ERROR;
		return std::get<1>(impl_);
	}

	constexpr E&& error() && noexcept {
		RB_ENSURE_ERROR;
		return std::get<1>(RB_MOVE(impl_));
	}

	constexpr E const& error() const& noexcept {
		RB_ENSURE_ERROR;
		return std::get<1>(impl_);
	}

	constexpr E const&& error() const&& noexcept {
		RB_ENSURE_ERROR;
		return std::get<1>(RB_MOVE(impl_));
	}

	constexpr bool hasValue() const noexcept {
		return impl_.index() == 0;
	}

	constexpr bool hasError() const noexcept {
		return impl_.index() == 1;
	}

	constexpr T const& expect(LiteralString msg) const& noexcept {
		RB_ASSERT_CUSTOM_MSG(hasValue(), msg);
		return value();
	}

	constexpr T& expect(LiteralString msg) & noexcept {
		RB_ASSERT_CUSTOM_MSG(hasValue(), msg);
		return value();
	}

	constexpr T&& expect(LiteralString msg) && noexcept {
		RB_ASSERT_CUSTOM_MSG(hasValue(), msg);
		return RB_MOVE(value());
	}

	constexpr T const&& expect(LiteralString msg) const&& noexcept {
		RB_ASSERT_CUSTOM_MSG(hasValue(), msg);
		return RB_MOVE(value());
	}

	// clang-format off
	template <class U,
	    RB_REQUIRES_T(IsConvertible<U&&, T>)>
	constexpr T valueOr(U&& defaultValue) const&
		noexcept(isNothrowCopyConstructible<T> && noexcept(static_cast<T>(RB_FWD(defaultValue)))) {
		return hasValue() ? value() : static_cast<T>(RB_FWD(defaultValue));
	}

	template <class U,
	    RB_REQUIRES_T(IsConvertible<U&&, T>)>
	constexpr T valueOr(U&& defaultValue) &&
		noexcept(isNothrowMoveConstructible<T> && noexcept(static_cast<T>(RB_FWD(defaultValue)))) {
		return hasValue() ? RB_MOVE(value()) : static_cast<T>(RB_FWD(defaultValue));
	}

	template <bool _ = true, RB_REQUIRES(_&& isDefaultConstructible<T>)>
	constexpr T valueOrDefault() const&
		noexcept(isNothrowDefaultConstructible<T>&& isNothrowCopyConstructible<T>) {
		return hasValue() ? value() : T{};
	}

	template <bool _ = true, RB_REQUIRES(_&& isDefaultConstructible<T>)>
	constexpr T valueOrDefault() &&
		noexcept(isNothrowDefaultConstructible<T>&& isNothrowMoveConstructible<T>) {
		return hasValue() ? RB_MOVE(value()) : T{};
	}

	// clang-format on

#undef RB_ENSURE_ERROR
#undef RB_ENSURE_VALUE

	constexpr Option<T> toValue() const noexcept(isNothrowCopyConstructible<T>) {
		return hasValue() ? Option(value()) : kNone;
	}

	constexpr Option<E> toError() const noexcept(isNothrowCopyConstructible<E>) {
		return hasError() ? Option(error()) : kNone;
	}

	// Option-like methods

	constexpr explicit operator bool() const noexcept {
		return impl_.index() == 0;
	}

	constexpr T const* operator->() const noexcept {
		return &value();
	}

	constexpr T* operator->() noexcept {
		return &value();
	}

	constexpr T& operator*() & noexcept {
		return value();
	}

	constexpr T&& operator*() && noexcept {
		return RB_MOVE(value());
	}

	constexpr T const& operator*() const& noexcept {
		return value();
	}

	constexpr T const&& operator*() const&& noexcept {
		return RB_MOVE(value());
	}

	// functional-style extensions

	/// Calls @p f if `this` has a value; otherwise returns the `error()`.
	template <class F>
	constexpr auto andThen(F&& f) & {
		return impl::andThen(*this, RB_FWD(f));
	}

	template <class F>
	constexpr auto andThen(F&& f) const& {
		return impl::andThen(*this, RB_FWD(f));
	}

	template <class F>
	constexpr auto andThen(F&& f) && {
		return impl::andThen(RB_MOVE(*this), RB_FWD(f));
	}

	template <class F>
	constexpr auto andThen(F&& f) const&& {
		return impl::andThen(RB_MOVE(*this), RB_FWD(f));
	}

	/// Maps a `Expected<T, E>` to `Expected<U, E>` by applying a function @p f to a contained value,
	/// leaving an error untouched.
	template <class F>
	constexpr auto map(F&& f) & {
		return impl::map(*this, RB_FWD(f));
	}

	template <class F>
	constexpr auto map(F&& f) const& {
		return impl::map(*this, RB_FWD(f));
	}

	template <class F>
	constexpr auto map(F&& f) && {
		return impl::map(RB_MOVE(*this), RB_FWD(f));
	}

	template <class F>
	constexpr auto map(F&& f) const&& {
		return impl::map(RB_MOVE(*this), RB_FWD(f));
	}

	/// Calls @p f if `this` has an error; otherwise returns the `value()`.
	template <class F>
	constexpr auto orElse(F&& f) & {
		return impl::orElse(*this, RB_FWD(f));
	}

	template <class F>
	constexpr auto orElse(F&& f) const& {
		return impl::orElse(*this, RB_FWD(f));
	}

	template <class F>
	constexpr auto orElse(F&& f) && {
		return impl::orElse(RB_MOVE(*this), RB_FWD(f));
	}

	template <class F>
	constexpr auto orElse(F&& f) const&& {
		return impl::orElse(RB_MOVE(*this), RB_FWD(f));
	}

private:
	template <usize idx, class... Args>
	constexpr explicit Expected(std::in_place_index_t<idx> index, Args&&... args)
	    noexcept(isNothrowConstructible<Impl, std::in_place_index_t<idx>, Args...>)
	    : impl_(index, RB_FWD(args)...) {
	}

	Impl impl_;
};

template <class E>
class RB_EXPORT Expected<void, E> final {
	static_assert(!isVoid<E>, "Error type must not be void");

	using Impl = Option<E>;

public:
	constexpr Expected() noexcept = default;

	// NOLINTBEGIN(google-explicit-constructor)
	constexpr Expected(Ok<void> /*unused*/) noexcept {
	}

	constexpr Expected(E const& error) RB_NOEXCEPT_LIKE(E(error))
	    : impl_(error) {
	}

	constexpr Expected(E&& error) RB_NOEXCEPT_LIKE(E(RB_MOVE(error)))
	    : impl_(RB_MOVE(error)) {
	}

	constexpr Expected(Err<E const&> err) RB_NOEXCEPT_LIKE(Impl(err.value))
	    : impl_(err.value) {
	}

	constexpr Expected(Err<E&&> err) RB_NOEXCEPT_LIKE(Impl(RB_MOVE(err.value)))
	    : impl_(RB_MOVE(err.value)) {
	}

	// NOLINTEND(google-explicit-constructor)

	constexpr Expected(Expected const& rhs) noexcept(isNothrowCopyConstructible<Impl>) = default;
	constexpr Expected(Expected&& rhs) noexcept(isNothrowMoveConstructible<Impl>) = default;
	~Expected() = default;

	constexpr Expected& operator=(Expected const& rhs) noexcept(isNothrowCopyAssignable<Impl>) = default;
	constexpr Expected& operator=(Expected&& rhs) noexcept(isNothrowMoveAssignable<Impl>) = default;

	constexpr Expected& operator=(E const& error) RB_NOEXCEPT_LIKE(impl_ = error) {
		impl_ = error;
		return *this;
	}

	constexpr Expected& operator=(E&& error) RB_NOEXCEPT_LIKE(impl_ = RB_MOVE(error)) {
		impl_ = RB_MOVE(error);
		return *this;
	}

	void swap(Expected& rhs) noexcept(isNothrowSwappable<Impl>) {
		impl_.swap(rhs.impl_);
	}

#define RB_ENSURE_ERROR RB_ASSERT_MSG(hasError(), "Expected doesn't contain an error");

	constexpr E& error() & noexcept {
		RB_ENSURE_ERROR;
		return *impl_;
	}

	constexpr E&& error() && noexcept {
		RB_ENSURE_ERROR;
		return *RB_MOVE(impl_);
	}

	constexpr E const& error() const& noexcept {
		RB_ENSURE_ERROR;
		return *impl_;
	}

	constexpr E const&& error() const&& noexcept {
		RB_ENSURE_ERROR;
		return *RB_MOVE(impl_);
	}

#undef RB_ENSURE_ERROR

	constexpr bool hasValue() const noexcept {
		return !impl_.hasValue();
	}

	constexpr bool hasError() const noexcept {
		return impl_.hasValue();
	}

	constexpr Option<E> toError() const noexcept(isNothrowCopyConstructible<E>) {
		return hasError() ? Option(error()) : kNone;
	}

	// Option-like methods

	constexpr explicit operator bool() const noexcept {
		return hasValue();
	}

	// functional-style extensions

	/// Calls @p f if `this` has a value; otherwise returns the `error()`.
	template <class F>
	constexpr auto andThen(F&& f) & {
		return impl::andThen(*this, RB_FWD(f));
	}

	template <class F>
	constexpr auto andThen(F&& f) const& {
		return impl::andThen(*this, RB_FWD(f));
	}

	template <class F>
	constexpr auto andThen(F&& f) && {
		return impl::andThen(RB_MOVE(*this), RB_FWD(f));
	}

	template <class F>
	constexpr auto andThen(F&& f) const&& {
		return impl::andThen(RB_MOVE(*this), RB_FWD(f));
	}

	/// Maps a `Expected<T, E>` to `Expected<U, E>` by applying a function @p f to a contained value,
	/// leaving an error untouched.
	template <class F>
	constexpr auto map(F&& f) & {
		return impl::map(*this, RB_FWD(f));
	}

	template <class F>
	constexpr auto map(F&& f) const& {
		return impl::map(*this, RB_FWD(f));
	}

	template <class F>
	constexpr auto map(F&& f) && {
		return impl::map(RB_MOVE(*this), RB_FWD(f));
	}

	template <class F>
	constexpr auto map(F&& f) const&& {
		return impl::map(RB_MOVE(*this), RB_FWD(f));
	}

	/// Calls @p f if `this` has an error; otherwise returns the `value()`.
	template <class F>
	constexpr auto orElse(F&& f) & {
		return impl::orElse(*this, RB_FWD(f));
	}

	template <class F>
	constexpr auto orElse(F&& f) const& {
		return impl::orElse(*this, RB_FWD(f));
	}

	template <class F>
	constexpr auto orElse(F&& f) && {
		return impl::orElse(RB_MOVE(*this), RB_FWD(f));
	}

	template <class F>
	constexpr auto orElse(F&& f) const&& {
		return impl::orElse(RB_MOVE(*this), RB_FWD(f));
	}

private:
	template <class... Args>
	constexpr explicit Expected(Args&&... args) noexcept(isNothrowConstructible<E, Args...>)
	    : impl_(kInPlace, RB_FWD(args)...) {
	}

	Impl impl_;
};

template <class Exp>
using IsExpected = impl::IsExpected<Decay<Exp>>;

template <class Exp>
inline constexpr bool isExpected = IsExpected<Exp>::value;

template <class T, class E, class U, class F,
    RB_REQUIRES(isVoid<T>),
    RB_REQUIRES(isVoid<U>),
    RB_REQUIRES(isEqualityComparable<E const&, F const&>)>
constexpr bool operator==(Expected<T, E> const& lhs, Expected<U, F> const& rhs)
    RB_NOEXCEPT_LIKE(lhs.error() == rhs.error()) {
	return lhs.hasValue() == rhs.hasValue() && (lhs.hasValue() || lhs.error() == rhs.error());
}

template <class T, class E, class U, class F,
    RB_REQUIRES(isVoid<T>),
    RB_REQUIRES(!isVoid<U>),
    RB_REQUIRES(isEqualityComparable<E const&, F const&>)>
constexpr bool operator==(Expected<T, E> const& lhs, Expected<U, F> const& rhs)
    RB_NOEXCEPT_LIKE(lhs.error() == rhs.error()) {
	return !lhs.hasValue() && !rhs.hasValue() && lhs.error() == rhs.error();
}

template <class T, class E, class U, class F,
    RB_REQUIRES(!isVoid<T>),
    RB_REQUIRES(isVoid<U>),
    RB_REQUIRES(isEqualityComparable<E const&, F const&>)>
constexpr bool operator==(Expected<T, E> const& lhs, Expected<U, F> const& rhs)
    RB_NOEXCEPT_LIKE(lhs.error() == rhs.error()) {
	return !lhs.hasValue() && !rhs.hasValue() && lhs.error() == rhs.error();
}

template <class T, class E, class U, class F,
    RB_REQUIRES(!isVoid<T>),
    RB_REQUIRES(!isVoid<U>),
    RB_REQUIRES(isEqualityComparable<T const&, U const&>&& isEqualityComparable<E const&, F const&>)>
constexpr bool operator==(Expected<T, E> const& lhs, Expected<U, F> const& rhs)
    RB_NOEXCEPT_LIKE(true ? lhs.value() == rhs.value() : lhs.error() == rhs.error()) {
	return lhs.hasValue() == rhs.hasValue() && (lhs.hasValue() ? lhs.value() == rhs.value() : lhs.error() == rhs.error());
}

template <class T, class E, class U, class F>
constexpr bool operator!=(Expected<T, E> const& lhs, Expected<U, F> const& rhs) RB_NOEXCEPT_LIKE(lhs == rhs) {
	return !(lhs == rhs);
}

template <class T, class E, class U,
    RB_REQUIRES(!isVoid<T> && !isExpected<U> && isEqualityComparable<T const&, U const&>)>
constexpr bool operator==(Expected<T, E> const& lhs, U const& rhs) RB_NOEXCEPT_LIKE(lhs.value() == rhs) {
	return lhs.hasValue() && lhs.value() == rhs;
}

template <class T, class E, class U,
    RB_REQUIRES(isVoid<T> && !isExpected<U>)>
constexpr bool operator==(Expected<T, E> const& /*lhs*/, U const& /*rhs*/) noexcept {
	return false;
}

template <class T, class E, class U,
    RB_REQUIRES(!isExpected<U>)>
constexpr bool operator!=(Expected<T, E> const& lhs, U const& rhs) RB_NOEXCEPT_LIKE(lhs == rhs) {
	return !(lhs == rhs);
}

template <class T, class E, class U,
    RB_REQUIRES(isEqualityComparable<E const&, U>)>
constexpr bool operator==(Expected<T, E> const& lhs, Err<U> rhs) RB_NOEXCEPT_LIKE(lhs.error() == rhs.value) {
	return lhs.hasError() && lhs.error() == rhs.value;
}

template <class T, class E, class U>
constexpr bool operator!=(Expected<T, E> const& lhs, Err<U> rhs) RB_NOEXCEPT_LIKE(lhs == rhs) {
	return !(lhs == rhs);
}

template <class T, class E,
    RB_REQUIRES_T(And<IsWritableTo<T, std::ostream>, IsWritableTo<E, std::ostream>>)>
std::ostream& operator<<(std::ostream& os, Expected<T, E> const& expected) {
	return expected.hasValue()
	         ? os << expected.value()
	         : os << expected.error();
}

template <class E,
    RB_REQUIRES_T(IsWritableTo<E, std::ostream>)>
std::ostream& operator<<(std::ostream& os, Expected<void, E> const& expected) {
	return expected.hasError()
	         ? os << expected.error()
	         : os << "void";
}

} // namespace rb::core
