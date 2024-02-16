#pragma once

#include <rb/core/attributes.hpp>
#include <rb/core/enable_special_members.hpp>
#include <rb/core/in_place.hpp>
#include <rb/core/invoke.hpp>
#include <rb/core/memory/addressOf.hpp>
#include <rb/core/traits/IsScalar.hpp>
#include <rb/core/traits/requirements.hpp>

namespace rb::core {

/**
 * NoneOption is an empty class type used to indicate Option type with uninitialized state.
 *
 * NoneOption must be a non-aggregate [LiteralType](https://en.cppreference.com/w/cpp/named_req/LiteralType)
 * And cannot have a default constructor or an initializer-list constructor.
 * This constraints on NoneOption's constructors exist to support both `op = {};` and `op = kNone;`
 * as the syntax for disengaging an Option object.
 */
struct NoneOption {
	constexpr explicit NoneOption(int /*unused*/) noexcept {
	}
};

inline constexpr NoneOption kNone{0};

struct Empty {};

template <class T>
class Option;

namespace impl {

	template <class F>
	struct OptionFunc {
		F& func;
	};

	template <class T, bool = isTriviallyDestructible<T>>
	union OptionStorage {
		constexpr OptionStorage() noexcept
		    : empty() {
		}

		template <class... Args>
		constexpr OptionStorage(InPlace /*unused*/, Args&&... args) // NOLINT(google-explicit-constructor)
		    : value(RB_FWD(args)...) {
		}

		template <class U, class... Args>
		constexpr OptionStorage(std::initializer_list<U> il, Args&&... args)
		    : value(il, RB_FWD(args)...) {
		}

		template <class F, class Arg>
		constexpr OptionStorage(OptionFunc<F> f, Arg&& arg)
		    : value(invoke(RB_FWD(f.func), RB_FWD(arg))) {
		}

		Empty empty;
		T value;
	};

	template <class T>
	union OptionStorage<T, false> { // Not trivially destructible

		constexpr OptionStorage() noexcept
		    : empty() {
		}

		template <class... Args>
		constexpr OptionStorage(InPlace /*unused*/, Args&&... args) // NOLINT(google-explicit-constructor)
		    : value(RB_FWD(args)...) {
		}

		template <class U, class... Args>
		constexpr OptionStorage(std::initializer_list<U> il, Args&&... args)
		    : value(il, RB_FWD(args)...) {
		}

		template <class F, class Arg>
		constexpr OptionStorage(OptionFunc<F> f, Arg&& arg)
		    : value(invoke(RB_FWD(f.func), RB_FWD(arg))) {
		}

		// User-provided destructor is needed when T has non-trivial dtor
		~OptionStorage() { // NOLINT(modernize-use-equals-default)
		}

		Empty empty;
		T value;
	};

	template <class T, class This>
	class EngageImpl {
	public:
		constexpr bool engaged() const noexcept {
			return engaged_;
		}

		constexpr void setEngaged(bool engaged) noexcept {
			engaged_ = engaged;
		}

	private:
		bool engaged_ = false;
	};

	// This class template manages construction/destruction of the contained value for an Option
	template <class T>
	struct OptionPayloadBase : EngageImpl<T, OptionPayloadBase<T>> {
		using StoredType = RemoveConst<T>;

		OptionStorage<StoredType> payload;

		OptionPayloadBase() = default;
		~OptionPayloadBase() = default;

		template <class... Args>
		constexpr OptionPayloadBase(InPlace inPlace, Args&&... args) // NOLINT(google-explicit-constructor)
		    : payload(inPlace, RB_FWD(args)...) {
			this->setEngaged(true);
		}

		template <class U, class... Args>
		constexpr OptionPayloadBase(std::initializer_list<U> il, Args&&... args)
		    : payload(il, RB_FWD(args)...) {
			this->setEngaged(true);
		}

		// Constructor used by OptionBase copy constructor when the contained value is not trivially copy-constructible
		constexpr OptionPayloadBase(bool /*engaged*/, OptionPayloadBase const& rhs) {
			if (rhs.engaged()) {
				this->construct(rhs.get());
			}
		}

		// Constructor used by OptionBase move constructor when the contained value is not trivially move-constructible
		constexpr OptionPayloadBase(bool /*engaged*/, OptionPayloadBase&& rhs) {
			if (rhs.engaged()) {
				this->construct(RB_MOVE(rhs.get()));
			}
		}

		// Copy constructor is only used to when the contained value is trivially copy-constructible
		OptionPayloadBase(OptionPayloadBase const&) = default;

		// Move constructor is only used to when the contained value is trivially copy-constructible
		OptionPayloadBase(OptionPayloadBase&&) noexcept = default;

		OptionPayloadBase& operator=(OptionPayloadBase const&) = default;
		OptionPayloadBase& operator=(OptionPayloadBase&&) noexcept = default;

		// Used to perform non-trivial copy assignment
		constexpr void copyAssign(OptionPayloadBase const& rhs) {
			if (this->engaged() && rhs.engaged()) {
				this->get() = rhs.get();
			} else {
				if (rhs.engaged()) {
					this->construct(rhs.get());
				} else {
					this->resetImpl();
				}
			}
		}

		// Used to perform non-trivial move assignment
		constexpr void moveAssign(OptionPayloadBase&& rhs)
		    noexcept(isNothrowMoveConstructible<T> && isNothrowMoveAssignable<T>) {
			if (this->engaged() && rhs.engaged()) {
				this->get() = RB_MOVE(rhs.get());
			} else {
				if (rhs.engaged()) {
					this->construct(RB_MOVE(rhs.get()));
				} else {
					this->resetImpl();
				}
			}
		}

		template <class... Args>
		constexpr void construct(Args&&... args) noexcept(isNothrowConstructible<StoredType, Args...>) {
			::new (addressOf(this->payload.value)) T(RB_FWD(args)...);
			this->setEngaged(true);
		}

		constexpr void destroy() noexcept {
			payload.value.~StoredType();
			this->setEngaged(false);
		}

		template <class F, class U>
		constexpr void apply(OptionFunc<F> f, U&& arg) {
			::new (addressOf(this->payload)) OptionStorage<StoredType>(f, RB_FWD(arg));
			this->setEngaged(true);
		}

		// The get() operations have `engaged()` as a precondition.
		// They exist to access the contained value with the appropriate
		// const-qualification, because the payload has had the `const` removed.

		constexpr T& get() noexcept {
			return this->payload.value;
		}

		constexpr T const& get() const noexcept {
			return this->payload.value;
		}

		// `resetImpl` is a 'safe' operation with no precondition
		constexpr void resetImpl() noexcept {
			if (this->engaged()) {
				destroy();
			}
		}
	};

	// Class template that manages the payload for Options
	template <
	    class T,
	    bool hasTrivialDestructor = isTriviallyDestructible<T>,
	    bool hasTrivialCopy = isTriviallyCopyAssignable<T> && isTriviallyCopyConstructible<T>,
	    bool hasTrivialMove = isTriviallyMoveAssignable<T> && isTriviallyMoveConstructible<T>>
	struct OptionPayload;

	// Payload for potentially-constexpr optionals (trivial copy/move/destroy)
	template <class T>
	struct OptionPayload<T, true, true, true> : OptionPayloadBase<T> {
		using OptionPayloadBase<T>::OptionPayloadBase;

		OptionPayload() = default;
	};

	// Payload for optionals with non-trivial copy construction/assignment
	template <class T>
	struct OptionPayload<T, true, false, true> : OptionPayloadBase<T> {
		using OptionPayloadBase<T>::OptionPayloadBase;

		OptionPayload() = default;
		OptionPayload(OptionPayload const&) = default;
		OptionPayload(OptionPayload&&) noexcept = default;
		~OptionPayload() = default;

		OptionPayload& operator=(OptionPayload&&) noexcept = default;

		// Non-trivial copy assignment
		constexpr OptionPayload& operator=(OptionPayload const& rhs) {
			if (this != &rhs) {
				this->copyAssign(rhs);
			}
			return *this;
		}
	};

	// Payload for optionals with non-trivial move construction/assignment
	template <class T>
	struct OptionPayload<T, true, true, false> : OptionPayloadBase<T> {
		using OptionPayloadBase<T>::OptionPayloadBase;

		OptionPayload() = default;
		OptionPayload(OptionPayload const&) = default;
		OptionPayload(OptionPayload&&) noexcept = default;
		~OptionPayload() = default;

		OptionPayload& operator=(OptionPayload const&) = default;

		// Non-trivial move assignment
		constexpr OptionPayload& operator=(OptionPayload&& rhs) //
		    noexcept(isNothrowMoveConstructible<T> && isNothrowMoveAssignable<T>) {
			this->moveAssign(RB_MOVE(rhs));
			return *this;
		}
	};

	// Payload for optionals with non-trivial copy And move assignment
	template <class T>
	struct OptionPayload<T, true, false, false> : OptionPayloadBase<T> {
		using OptionPayloadBase<T>::OptionPayloadBase;

		OptionPayload() = default;
		OptionPayload(OptionPayload const&) = default;
		OptionPayload(OptionPayload&&) noexcept = default;
		~OptionPayload() = default;

		// Non-trivial copy assignment
		constexpr OptionPayload& operator=(OptionPayload const& rhs) {
			if (this != &rhs) {
				this->copyAssign(rhs);
			}
			return *this;
		}

		// Non-trivial move assignment
		constexpr OptionPayload& operator=(OptionPayload&& rhs) //
		    noexcept(isNothrowMoveConstructible<T> && isNothrowMoveAssignable<T>) {
			this->moveAssign(RB_MOVE(rhs));
			return *this;
		}
	};

	// Payload for optionals with non-trivial destructors
	template <class T, bool copy, bool move>
	struct OptionPayload<T, false, copy, move> : OptionPayload<T, true, false, false> {
		// Base class implements all the constructors And assignment operators
		using OptionPayload<T, true, false, false>::OptionPayload;

		OptionPayload() = default;
		OptionPayload(OptionPayload const&) = default;
		OptionPayload(OptionPayload&&) noexcept = default;

		OptionPayload& operator=(OptionPayload const&) = default;
		OptionPayload& operator=(OptionPayload&&) noexcept = default;

		~OptionPayload() {
			this->resetImpl();
		}
	};

	// Common base class for OptionBase<T> to avoid repeating these member functions in each specialization
	template <class T, class U>
	class OptionBaseImpl {
	protected:
		// The construct operation has !engaged() as a precondition while destruct has engaged() as a precondition
		template <class... Args>
		constexpr void construct(Args&&... args)
		    RB_NOEXCEPT_LIKE(static_cast<U*>(this) -> payload.construct(RB_FWD(args)...)) {
			static_cast<U*>(this)->payload.construct(RB_FWD(args)...);
		}

		constexpr void destruct() noexcept {
			static_cast<U*>(this)->payload.destroy();
		}

		// resetImpl is a 'safe' operation with no precondition
		constexpr void resetImpl() noexcept {
			static_cast<U*>(this)->payload.resetImpl();
		}

		constexpr bool engaged() const noexcept {
			return static_cast<U const*>(this)->payload.engaged();
		}

		constexpr T& get() noexcept {
			RB_ASSERT(this->engaged());
			return static_cast<U*>(this)->payload.get();
		}

		constexpr T const& get() const noexcept {
			RB_ASSERT(this->engaged());
			return static_cast<U const*>(this)->payload.get();
		}
	};

	/**
	 * Class template that provides copy/move constructors of Option.
	 *
	 * Such a separate base class template is necessary in order to conditionally make copy/move constructors trivial.
	 *
	 * When the contained value is trivially copy/move constructible,
	 * the copy/move constructors of OptionBase will invoke the trivial copy/move constructor of OptionPayload.
	 * Otherwise, they will invoke `OptionPayload(bool, const OptionPayload&)` or `OptionPayload(bool, OptionPayload&&)`
	 * to initialize the contained value, if copying/moving an engaged Option.
	 *
	 * Whether the other special members are trivial is determined by the OptionPayload<T> specialization
	 * used for the payload member.
	 */
	template <
	    class T,
	    bool = isTriviallyCopyConstructible<T>,
	    bool = isTriviallyMoveConstructible<T>>
	struct OptionBase : OptionBaseImpl<T, OptionBase<T>> {
		constexpr OptionBase() = default;

		template <class... Args,
		    RB_REQUIRES(isConstructible<T, Args...>)>
		constexpr explicit OptionBase(InPlace inPlace, Args&&... args)
		    : payload(inPlace, RB_FWD(args)...) {
		}

		template <class U, class... Args,
		    RB_REQUIRES(isConstructible<T, std::initializer_list<U>&, Args...>)>
		constexpr explicit OptionBase(InPlace inPlace, std::initializer_list<U> il, Args&&... args)
		    : payload(inPlace, il, RB_FWD(args)...) {
		}

		constexpr OptionBase(OptionBase const& rhs)
		    : payload(rhs.payload.engaged(), rhs.payload) {
		}

		constexpr OptionBase(OptionBase&& rhs) noexcept(isNothrowMoveConstructible<T>)
		    : payload(rhs.payload.engaged(), RB_MOVE(rhs.payload)) {
		}

		OptionBase& operator=(OptionBase const&) = default;
		OptionBase& operator=(OptionBase&&) noexcept = default;

		OptionPayload<T> payload;
	};

	template <class T>
	struct OptionBase<T, false, true> : OptionBaseImpl<T, OptionBase<T>> {
		constexpr OptionBase() = default;

		template <class... Args,
		    RB_REQUIRES(isConstructible<T, Args...>)>
		constexpr explicit OptionBase(InPlace inPlace, Args&&... args)
		    : payload(inPlace, RB_FWD(args)...) {
		}

		template <class U, class... Args,
		    RB_REQUIRES(isConstructible<T, std::initializer_list<U>&, Args...>)>
		constexpr explicit OptionBase(InPlace inPlace, std::initializer_list<U> il, Args&&... args)
		    : payload(inPlace, il, RB_FWD(args)...) {
		}

		constexpr OptionBase(OptionBase const& rhs)
		    : payload(rhs.payload.engaged(), rhs.payload) {
		}

		constexpr OptionBase(OptionBase&& rhs) noexcept = default;

		OptionBase& operator=(OptionBase const&) = default;
		OptionBase& operator=(OptionBase&&) noexcept = default;

		OptionPayload<T> payload;
	};

	template <class T>
	struct OptionBase<T, true, false> : OptionBaseImpl<T, OptionBase<T>> {
		constexpr OptionBase() = default;

		template <class... Args,
		    RB_REQUIRES(isConstructible<T, Args...>)>
		constexpr explicit OptionBase(InPlace inPlace, Args&&... args)
		    : payload(inPlace, RB_FWD(args)...) {
		}

		template <class U, class... Args,
		    RB_REQUIRES(isConstructible<T, std::initializer_list<U>&, Args...>)>
		constexpr explicit OptionBase(InPlace inPlace, std::initializer_list<U> il, Args&&... args)
		    : payload(inPlace, il, RB_FWD(args)...) {
		}

		constexpr OptionBase(OptionBase const& rhs) = default;

		constexpr OptionBase(OptionBase&& rhs) noexcept(isNothrowMoveConstructible<T>)
		    : payload(rhs.payload.engaged(), RB_MOVE(rhs.payload)) {
		}

		OptionBase& operator=(OptionBase const&) = default;
		OptionBase& operator=(OptionBase&&) noexcept = default;

		OptionPayload<T> payload;
	};

	template <class T>
	struct OptionBase<T, true, true> : OptionBaseImpl<T, OptionBase<T>> {
		constexpr OptionBase() = default;

		template <class... Args,
		    RB_REQUIRES(isConstructible<T, Args...>)>
		constexpr explicit OptionBase(InPlace inPlace, Args&&... args)
		    : payload(inPlace, RB_FWD(args)...) {
		}

		template <class U, class... Args,
		    RB_REQUIRES(isConstructible<T, std::initializer_list<U>&, Args...>)>
		constexpr explicit OptionBase(InPlace inPlace, std::initializer_list<U> il, Args&&... args)
		    : payload(inPlace, il, RB_FWD(args)...) {
		}

		constexpr OptionBase(OptionBase const& rhs) = default;
		constexpr OptionBase(OptionBase&& rhs) noexcept = default;

		OptionBase& operator=(OptionBase const&) = default;
		OptionBase& operator=(OptionBase&&) noexcept = default;

		OptionPayload<T> payload;
	};

	template <class T, class U>
	using ConvertsFromOption = Or<
	    IsConstructible<T, Option<U> const&>,
	    IsConstructible<T, Option<U>&>,
	    IsConstructible<T, Option<U> const&&>,
	    IsConstructible<T, Option<U>&&>,
	    IsConvertible<Option<U> const&, T>,
	    IsConvertible<Option<U>&, T>,
	    IsConvertible<Option<U> const&&, T>,
	    IsConvertible<Option<U>&&, T>>;

	template <class T, class U>
	using AssignsFromOption = Or<
	    IsAssignable<T&, Option<U> const&>,
	    IsAssignable<T&, Option<U>&>,
	    IsAssignable<T&, Option<U> const&&>,
	    IsAssignable<T&, Option<U>&&>>;

} // namespace impl

template <class>
struct IsOption : False {};

template <class T>
struct IsOption<Option<T>> : True {};

template <class T>
inline constexpr bool isOption = IsOption<T>::value;

/**
 * The class template Option manages an @em optional contained value, i.e.\ a value that may or may not be present.
 * @tparam T the type of the value to manage initialization state for.
 *
 * Some implementation details of Option based on [`tl::Option`](https://github.com/TartanLlama/Option).
 * Idea of this class inspired by Rust [`std::option`](https://doc.rust-lang.org/std/option/index.html).
 */
template <class T>
class RB_EXPORT Option final
    : private impl::OptionBase<T>
    , private EnableCopyMove<
          isCopyConstructible<T>,
          isCopyConstructible<T> && isCopyAssignable<T>,
          isMoveConstructible<T>,
          isMoveConstructible<T> && isMoveAssignable<T>,
          Option<T>> {
	static_assert(!isSame<RemoveCv<T>, NoneOption>);
	static_assert(!isSame<RemoveCv<T>, InPlace>);

	using Super = impl::OptionBase<T>;

	template <class U>
	using NotSelf = Not<IsSame<Option, RemoveCvRef<U>>>;

	template <class U>
	using NotTag = Not<IsSame<InPlace, RemoveCvRef<U>>>;

public:
	constexpr Option() noexcept { // NOLINT(modernize-use-equals-default)
	}

	// NOLINTBEGIN(google-explicit-constructor,bugprone-forwarding-reference-overload)
	constexpr Option(NoneOption /*unused*/) noexcept {
	}

	template <class U = T,
	    RB_REQUIRES_T(And<NotSelf<U>, NotTag<U>, IsConstructible<T, U>, IsConvertible<T, U>>)>
	constexpr Option(U&& value) noexcept(isNothrowConstructible<T, U>)
	    : Super(kInPlace, RB_FWD(value)) {
	}

	template <class U = T,
	    RB_REQUIRES_T(And<NotSelf<U>, NotTag<U>, IsConstructible<T, U>, Not<IsConvertible<T, U>>>)>
	constexpr explicit Option(U&& value) noexcept(isNothrowConstructible<T, U>)
	    : Super(kInPlace, RB_FWD(value)) {
	}

	template <class U,
	    RB_REQUIRES_T(And<
	        Not<IsSame<T, U>>,
	        IsConstructible<T, U const&>,
	        IsConvertible<U const&, T>,
	        Not<impl::ConvertsFromOption<T, U>>>)>
	constexpr Option(Option<U> const& rhs) noexcept(isNothrowConstructible<T, U const&>) {
		if (rhs) {
			emplace(*rhs);
		}
	}

	template <class U,
	    RB_REQUIRES_T(And<
	        Not<IsSame<T, U>>,
	        IsConstructible<T, U const&>,
	        Not<IsConvertible<U const&, T>>,
	        Not<impl::ConvertsFromOption<T, U>>>)>
	constexpr explicit Option(Option<U> const& rhs) noexcept(isNothrowConstructible<T, U const&>) {
		if (rhs) {
			emplace(*rhs);
		}
	}

	template <class U,
	    RB_REQUIRES_T(And<
	        Not<IsSame<T, U>>,
	        IsConstructible<T, U>,
	        IsConvertible<U, T>,
	        Not<impl::ConvertsFromOption<T, U>>>)>
	constexpr Option(Option<U>&& rhs) noexcept(isNothrowConstructible<T, U>) {
		if (rhs) {
			emplace(RB_MOVE(*rhs));
		}
	}

	template <class U,
	    RB_REQUIRES_T(And<
	        Not<IsSame<T, U>>,
	        IsConstructible<T, U>,
	        Not<IsConvertible<U, T>>,
	        Not<impl::ConvertsFromOption<T, U>>>)>
	constexpr explicit Option(Option<U>&& rhs) noexcept(isNothrowConstructible<T, U>) {
		if (rhs) {
			emplace(RB_MOVE(*rhs));
		}
	}

	template <class... Args,
	    RB_REQUIRES(isConstructible<T, Args...>)>
	constexpr explicit Option(InPlace /*kInPlace*/, Args&&... args) noexcept(isNothrowConstructible<T, Args...>)
	    : Super(kInPlace, RB_FWD(args)...) {
	}

	template <class U, class... Args,
	    RB_REQUIRES(isConstructible<T, std::initializer_list<U>&, Args...>)>
	constexpr explicit Option(InPlace /*kInPlace*/, std::initializer_list<U> il, Args&&... args) //
	    noexcept(isNothrowConstructible<T, std::initializer_list<U>&, Args...>)
	    : Super(kInPlace, il, RB_FWD(args)...) {
	}

	// NOLINTEND(google-explicit-constructor,bugprone-forwarding-reference-overload)

	Option& operator=(NoneOption /*unused*/) noexcept {
		this->reset();
		return *this;
	}

	template <class U = T,
	    RB_REQUIRES_T(And<NotSelf<U>,
	        IsConstructible<T, U>,
	        IsAssignable<T&, U>,
	        Or<Not<IsScalar<T>>, IsSame<T, Decay<U>>>>)>
	Option& operator=(U&& rhs) noexcept(isNothrowConstructible<T, U> && isNothrowAssignable<T&, U>) {
		if (this->engaged()) {
			this->get() = RB_FWD(rhs);
		} else {
			this->construct(RB_FWD(rhs));
		}
		return *this;
	}

	template <
	    class U,
	    RB_REQUIRES_T(And<
	        Not<IsSame<T, U>>,
	        IsConstructible<T, U const&>,
	        IsAssignable<T&, U const&>,
	        Not<impl::ConvertsFromOption<T, U>>,
	        Not<impl::AssignsFromOption<T, U>>>)>
	Option& operator=(Option<U> const& rhs) //
	    noexcept(isNothrowConstructible<T, U const&> && isNothrowAssignable<T&, U const&>) {
		if (rhs) {
			if (this->engaged()) {
				this->get() = *rhs;
			} else {
				this->construct(*rhs);
			}
		} else {
			this->reset();
		}
		return *this;
	}

	template <class U,
	    RB_REQUIRES_T(And<
	        Not<IsSame<T, U>>,
	        IsConstructible<T, U>,
	        IsAssignable<T&, U>,
	        Not<impl::ConvertsFromOption<T, U>>,
	        Not<impl::AssignsFromOption<T, U>>>)>
	Option& operator=(Option<U>&& rhs) noexcept(isNothrowConstructible<T, U> && isNothrowAssignable<T&, U>) {
		if (rhs) {
			if (this->engaged()) {
				this->get() = RB_MOVE(*rhs);
			} else {
				this->construct(RB_MOVE(*rhs));
			}
		} else {
			this->reset();
		}

		return *this;
	}

	template <class... Args,
	    RB_REQUIRES(isConstructible<T, Args...>)>
	T& emplace(Args&&... args) noexcept(isNothrowConstructible<T, Args...>) {
		this->reset();
		this->construct(RB_FWD(args)...);
		return this->get();
	}

	template <class U, class... Args,
	    RB_REQUIRES(isConstructible<T, std::initializer_list<U>&, Args...>)>
	T& emplace(std::initializer_list<U> il, Args&&... args) //
	    noexcept(isNothrowConstructible<T, std::initializer_list<U>&, Args...>) {
		this->reset();
		this->construct(il, RB_FWD(args)...);
		return this->get();
	}

	void swap(Option& rhs) noexcept(isNothrowMoveConstructible<T> && isNothrowSwappable<T>) {
		using rb::core::swap;

		if (this->engaged() && rhs.engaged()) {
			swap(this->get(), rhs.get());
		} else if (this->engaged()) {
			rhs.construct(RB_MOVE(this->get()));
			this->destruct();
		} else if (rhs.engaged()) {
			this->construct(RB_MOVE(rhs.get()));
			rhs.destruct();
		}
	}

	constexpr T const* operator->() const noexcept {
		return addressOf(this->get());
	}

	constexpr T* operator->() noexcept {
		return addressOf(this->get());
	}

	constexpr T const& operator*() const& noexcept {
		return this->get();
	}

	constexpr T& operator*() & noexcept {
		return this->get();
	}

	constexpr T&& operator*() && noexcept {
		return RB_MOVE(this->get());
	}

	constexpr T const&& operator*() const&& noexcept {
		return RB_MOVE(this->get());
	}

	constexpr explicit operator bool() const noexcept {
		return this->engaged();
	}

	constexpr bool hasValue() const noexcept {
		return this->engaged();
	}

	constexpr T const& value() const& noexcept {
		return this->get();
	}

	constexpr T& value() & noexcept {
		return this->get();
	}

	constexpr T&& value() && noexcept {
		return RB_MOVE(this->get());
	}

	constexpr T const&& value() const&& noexcept {
		return RB_MOVE(this->get());
	}

	constexpr T const& unwrap() const& noexcept {
		return this->get();
	}

	constexpr T& unwrap() & noexcept {
		return this->get();
	}

	constexpr T&& unwrap() && noexcept {
		return RB_MOVE(this->get());
	}

	constexpr T const&& unwrap() const&& noexcept {
		return RB_MOVE(this->get());
	}

	constexpr T const& expect(char const* msg) const& noexcept {
		RB_ASSERT_CUSTOM_MSG(hasValue(), msg);
		return this->get();
	}

	constexpr T& expect(char const* msg) & noexcept {
		RB_ASSERT_CUSTOM_MSG(hasValue(), msg);
		return this->get();
	}

	constexpr T&& expect(char const* msg) && noexcept {
		RB_ASSERT_CUSTOM_MSG(hasValue(), msg);
		return RB_MOVE(this->get());
	}

	constexpr T const&& expect(char const* msg) const&& noexcept {
		RB_ASSERT_CUSTOM_MSG(hasValue(), msg);
		return RB_MOVE(this->get());
	}

	template <class U>
	constexpr T valueOr(U&& rhs) const& {
		static_assert(isCopyConstructible<T>);
		static_assert(isConvertible<U&&, T>);

		if (this->engaged()) {
			return this->get();
		}
		return static_cast<T>(RB_FWD(rhs));
	}

	template <class U>
	constexpr T valueOr(U&& rhs) && {
		static_assert(isMoveConstructible<T>);
		static_assert(isConvertible<U&&, T>);

		if (this->engaged()) {
			return RB_MOVE(this->get());
		}
		return static_cast<T>(RB_FWD(rhs));
	}

	template <class F>
	constexpr auto andThen(F&& f) & {
		using U = RemoveCvRef<InvokeResult<F, T&>>;
		static_assert(isOption<U>);
		if (hasValue()) {
			return invoke(RB_FWD(f), **this);
		}
		return U();
	}

	template <class F>
	constexpr auto andThen(F&& f) const& {
		using U = RemoveCvRef<InvokeResult<F, T const&>>;
		static_assert(isOption<U>);
		if (hasValue()) {
			return invoke(RB_FWD(f), **this);
		}
		return U();
	}

	template <class F>
	constexpr auto andThen(F&& f) && {
		using U = RemoveCvRef<InvokeResult<F, T>>;
		static_assert(isOption<U>);
		if (hasValue()) {
			return invoke(RB_FWD(f), RB_MOVE(**this));
		}
		return U();
	}

	template <class F>
	constexpr auto andThen(F&& f) const&& {
		using U = RemoveCvRef<InvokeResult<F, T const>>;
		static_assert(isOption<U>);
		if (hasValue()) {
			return invoke(RB_FWD(f), RB_MOVE(**this));
		}
		return U();
	}

	template <class F>
	constexpr auto map(F&& f) & {
		using U = InvokeResult<F, T&>;
		if (hasValue()) {
			return Option<U>(impl::OptionFunc<F>{f}, **this);
		}
		return Option<U>();
	}

	template <class F>
	constexpr auto map(F&& f) const& {
		using U = InvokeResult<F, T const&>;
		if (hasValue()) {
			return Option<U>(impl::OptionFunc<F>{f}, **this);
		}
		return Option<U>();
	}

	template <class F>
	constexpr auto map(F&& f) && {
		using U = InvokeResult<F, T>;
		if (hasValue()) {
			return Option<U>(impl::OptionFunc<F>{f}, RB_MOVE(**this));
		}
		return Option<U>();
	}

	template <class F>
	constexpr auto map(F&& f) const&& {
		using U = InvokeResult<F, T const>;
		if (hasValue()) {
			return Option<U>(impl::OptionFunc<F>{f}, RB_MOVE(**this));
		}
		return Option<U>();
	}

	template <class F,
	    RB_REQUIRES(isInvocable<F>&& isCopyConstructible<T>)>
	constexpr Option orElse(F&& f) const& {
		using U = InvokeResult<F>;
		static_assert(isSame<RemoveCvRef<U>, Option>);

		if (hasValue()) {
			return *this;
		}
		return RB_FWD(f)();
	}

	template <class F,
	    RB_REQUIRES(isInvocable<F>&& isMoveConstructible<T>)>
	constexpr Option orElse(F&& f) && {
		using U = InvokeResult<F>;
		static_assert(isSame<RemoveCvRef<U>, Option>);

		if (hasValue()) {
			return RB_MOVE(*this);
		}
		return RB_FWD(f)();
	}

	constexpr void reset() noexcept {
		this->resetImpl();
	}

private:
	template <class>
	friend class Option;

	template <class F, class Arg>
	constexpr explicit Option(impl::OptionFunc<F> f, Arg&& arg) noexcept(isNothrowInvocable<F, Arg>) {
		this->payload.apply(f, RB_FWD(arg));
	}
};

template <class T>
class RB_EXPORT Option<T&> final {
	template <class U>
	using NotSelf = Not<IsSame<Option, RemoveCvRef<U>>>;

public:
	constexpr Option() noexcept = default;

	// NOLINTBEGIN(google-explicit-constructor,bugprone-forwarding-reference-overload)

	constexpr Option(NoneOption /*unused*/) noexcept {
	}

	template <class U,
	    RB_REQUIRES_T(And<NotSelf<U>, IsConvertible<U*, T*>>)>
	constexpr Option(InPlace /*unused*/, U& value RB_LIFETIME_BOUND) noexcept
	    : ptr_(addressOf(value)) {
	}

	template <class U>
	constexpr Option(U& value RB_LIFETIME_BOUND) noexcept
	    : Option(kInPlace, value) {
	}

	template <class U,
	    RB_REQUIRES_T(And<
	        Not<IsSame<T&, U>>,
	        IsConvertible<U const*, T*>,
	        Not<impl::ConvertsFromOption<T&, U>>>)>
	constexpr Option(Option<U> const& rhs) noexcept {
		if (rhs) {
			emplace(*rhs);
		}
	}

	template <class U,
	    RB_REQUIRES_T(And<
	        Not<IsSame<T&, U>>,
	        IsConvertible<U*, T*>,
	        Not<impl::ConvertsFromOption<T&, U>>>)>
	constexpr Option(Option<U>& rhs) noexcept {
		if (rhs) {
			emplace(*rhs);
		}
	}

	// NOLINTEND(google-explicit-constructor,bugprone-forwarding-reference-overload)

	constexpr Option(Option const&) noexcept = default;
	constexpr Option(Option&&) noexcept = default;
	~Option() = default;

	Option& operator=(NoneOption /*unused*/) noexcept {
		ptr_ = nullptr;
		return *this;
	}

	template <class U,
	    RB_REQUIRES_T(And<NotSelf<U>, IsConvertible<U*, T*>>)>
	Option& operator=(U& rhs RB_LIFETIME_BOUND) noexcept {
		ptr_ = addressOf(rhs);
		return *this;
	}

	template <
	    class U,
	    RB_REQUIRES_T(And<
	        Not<IsSame<T&, U>>,
	        IsConvertible<U const*, T*>,
	        Not<impl::ConvertsFromOption<T&, U>>,
	        Not<impl::AssignsFromOption<T&, U>>>)>
	Option& operator=(Option<U> const& rhs) noexcept {
		if (rhs) {
			emplace(*rhs);
		} else {
			reset();
		}
		return *this;
	}

	template <
	    class U,
	    RB_REQUIRES_T(And<
	        Not<IsSame<T&, U>>,
	        IsConvertible<U*, T*>,
	        Not<impl::ConvertsFromOption<T&, U>>,
	        Not<impl::AssignsFromOption<T&, U>>>)>
	Option& operator=(Option<U>& rhs) noexcept {
		if (rhs) {
			emplace(*rhs);
		} else {
			reset();
		}
		return *this;
	}

	template <class U,
	    RB_REQUIRES_T(IsConvertible<U*, T*>)>
	T& emplace(U& value RB_LIFETIME_BOUND) noexcept {
		ptr_ = addressOf(value);
		return *ptr_;
	}

	void swap(Option& rhs) noexcept {
		core::swap(ptr_, rhs.ptr_);
	}

	constexpr T const* operator->() const noexcept {
		return ptr_;
	}

	constexpr T* operator->() noexcept {
		return ptr_;
	}

	constexpr T const& operator*() const noexcept {
		RB_ASSERT(hasValue());
		return *ptr_;
	}

	constexpr T& operator*() noexcept {
		RB_ASSERT(hasValue());
		return *ptr_;
	}

	constexpr explicit operator bool() const noexcept {
		return hasValue();
	}

	constexpr bool hasValue() const noexcept {
		return ptr_;
	}

	constexpr T const& value() const noexcept {
		RB_ASSERT(hasValue());
		return *ptr_;
	}

	constexpr T& value() noexcept {
		RB_ASSERT(hasValue());
		return *ptr_;
	}

	constexpr T const& unwrap() const noexcept {
		return value();
	}

	constexpr T& unwrap() noexcept {
		return value();
	}

	constexpr T const& expect(char const* msg) const noexcept {
		RB_ASSERT_CUSTOM_MSG(hasValue(), msg);
		return *ptr_;
	}

	constexpr T& expect(char const* msg) noexcept {
		RB_ASSERT_CUSTOM_MSG(hasValue(), msg);
		return *ptr_;
	}

	template <class U,
	    RB_REQUIRES(isCopyConstructible<T>&& isConvertible<U&&, T>)>
	constexpr T valueOr(U&& rhs) const& {
		if (hasValue()) {
			return value();
		}
		return static_cast<T>(RB_FWD(rhs));
	}

	template <class U,
	    RB_REQUIRES(isMoveConstructible<T>&& isConvertible<U&&, T>)>
	constexpr T valueOr(U&& rhs) && {
		if (hasValue()) {
			return RB_MOVE(value());
		}
		return static_cast<T>(RB_FWD(rhs));
	}

	template <class F>
	constexpr auto andThen(F&& f) & {
		using U = RemoveCvRef<InvokeResult<F, T&>>;
		static_assert(isOption<U>);
		if (hasValue()) {
			return invoke(RB_FWD(f), **this);
		}
		return U();
	}

	template <class F>
	constexpr auto andThen(F&& f) const& {
		using U = RemoveCvRef<InvokeResult<F, T const&>>;
		static_assert(isOption<U>);
		if (hasValue()) {
			return invoke(RB_FWD(f), **this);
		}
		return U();
	}

	template <class F>
	constexpr auto andThen(F&& f) && {
		using U = RemoveCvRef<InvokeResult<F, T>>;
		static_assert(isOption<U>);
		if (hasValue()) {
			return invoke(RB_FWD(f), RB_MOVE(**this));
		}
		return U();
	}

	template <class F>
	constexpr auto andThen(F&& f) const&& {
		using U = RemoveCvRef<InvokeResult<F, T const>>;
		static_assert(isOption<U>);
		if (hasValue()) {
			return invoke(RB_FWD(f), RB_MOVE(**this));
		}
		return U();
	}

	template <class F>
	constexpr auto map(F&& f) & {
		using U = InvokeResult<F, T&>;
		if (hasValue()) {
			return Option<U>(impl::OptionFunc<F>{f}, **this);
		}
		return Option<U>();
	}

	template <class F>
	constexpr auto map(F&& f) const& {
		using U = InvokeResult<F, T const&>;
		if (hasValue()) {
			return Option<U>(impl::OptionFunc<F>{f}, **this);
		}
		return Option<U>();
	}

	template <class F>
	constexpr auto map(F&& f) && {
		using U = InvokeResult<F, T>;
		if (hasValue()) {
			return Option<U>(impl::OptionFunc<F>{f}, RB_MOVE(**this));
		}
		return Option<U>();
	}

	template <class F>
	constexpr auto map(F&& f) const&& {
		using U = InvokeResult<F, T const>;
		if (hasValue()) {
			return Option<U>(impl::OptionFunc<F>{f}, RB_MOVE(**this));
		}
		return Option<U>();
	}

	template <class F,
	    RB_REQUIRES(isInvocable<F>)>
	constexpr Option orElse(F&& f) const& {
		using U = InvokeResult<F>;
		static_assert(isSame<RemoveCvRef<U>, Option>);

		if (hasValue()) {
			return *this;
		}
		return RB_FWD(f)();
	}

	template <class F,
	    RB_REQUIRES(isInvocable<F>)>
	constexpr Option orElse(F&& f) && {
		using U = InvokeResult<F>;
		static_assert(isSame<RemoveCvRef<U>, Option>);

		if (hasValue()) {
			return RB_MOVE(*this);
		}
		return RB_FWD(f)();
	}

	constexpr void reset() noexcept {
		ptr_ = nullptr;
	}

private:
	template <class>
	friend class Option;

	template <class F, class Arg>
	constexpr explicit Option(impl::OptionFunc<F> f, Arg&& arg) noexcept(isNothrowInvocable<F, Arg>)
	    : Option(invoke(RB_FWD(f.func), RB_FWD(arg))) {
	}

	T* ptr_ = nullptr;
};

// deduction guides
template <class T>
Option(T) -> Option<T>;

// rel ops
template <class T, class U, RB_REQUIRES_T(IsEqualityComparable<T const&, U const&>)>
constexpr bool operator==(Option<T> const& lhs, Option<U> const& rhs) RB_NOEXCEPT_LIKE(*lhs == *rhs) {
	return lhs ? (rhs && *lhs == *rhs) : !rhs;
}

template <class T, class U, RB_REQUIRES_T(IsInequalityComparable<T const&, U const&>)>
constexpr bool operator!=(Option<T> const& lhs, Option<U> const& rhs) RB_NOEXCEPT_LIKE(*lhs != *rhs) {
	return lhs ? (!rhs || *lhs != *rhs) : rhs;
}

// FIXME add other rel ops
template <class T, class U, RB_REQUIRES_T(IsLessThanComparable<T const&, U const&>)>
constexpr bool operator<(Option<T> const& lhs, Option<U> const& rhs) RB_NOEXCEPT_LIKE(*lhs < *rhs) {
	return rhs && (!lhs || *lhs < *rhs);
}

template <class T>
constexpr bool operator==(Option<T> const& lhs, NoneOption /*rhs*/) noexcept {
	return !lhs;
}

template <class T>
constexpr bool operator==(NoneOption /*lhs*/, Option<T> const& rhs) noexcept {
	return !rhs;
}

template <class T>
constexpr bool operator!=(Option<T> const& lhs, NoneOption /*rhs*/) noexcept {
	return lhs;
}

template <class T>
constexpr bool operator!=(NoneOption /*lhs*/, Option<T> const& rhs) noexcept {
	return rhs;
}

template <class T, class Char, class Traits,
    RB_REQUIRES_T(IsWritableTo<T, std::basic_ostream<Char, Traits>>)>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, Option<T> const& opt) {
	return opt ? (os << *opt) : (os << "none");
}

} // namespace rb::core
