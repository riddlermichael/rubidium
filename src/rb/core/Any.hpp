#pragma once

#include <typeinfo>

#include <rb/core/memory/UniquePtr.hpp>
#include <rb/core/Option.hpp>

namespace rb::core {

/// The class Any describes a type-safe container for single values of any copy constructible type.
class RB_EXPORT Any final {
public:
	template <class T>
	using Storable = IsCopyConstructible<Decay<T>>;

	template <class T>
	static inline constexpr bool kStorable = Storable<T>::value;

	using TypeInfo = std::type_info;

	constexpr Any() noexcept = default;

	Any(Any const& rhs)
	    : storage_(rhs.storage_->clone()) {
	}

	Any(Any&& rhs) noexcept = default;

	template <class T,
	    RB_REQUIRES(!isSame<Decay<T>, Any> && !impl::isInPlaceType<Decay<T>> && kStorable<T>)>
	Any(T&& value) // NOLINT(*-explicit-constructor, *-forwarding-reference-overload)
	    : Any(kInPlaceType<T>, RB_FWD(value)) {
	}

	template <class T, class... Args,
	    RB_REQUIRES(kStorable<T>&& isConstructible<Decay<T>, Args...>)>
	explicit Any(InPlaceType<T> /*unused*/, Args&&... args)
	    : storage_(StoragePtr<T>::from(RB_FWD(args)...)) {
	}

	Any& operator=(Any const& rhs) {
		if (this != &rhs) {
			storage_ = rhs.storage_->clone();
		}
		return *this;
	}

	Any& operator=(Any&& rhs) noexcept = default;

	template <class T,
	    RB_REQUIRES(!isSame<Decay<T>, Any> && kStorable<T>)>
	Any& operator=(T&& value) {
		Any(RB_FWD(value)).swap(*this);
		return *this;
	}

	[[nodiscard]] constexpr explicit operator bool() const noexcept {
		return hasValue();
	}

	[[nodiscard]] constexpr bool hasValue() const noexcept {
		return static_cast<bool>(storage_);
	}

#if __cpp_rtti
	/// @return The `typeid` of the contained value if instance is non-empty, otherwise `typeid(void)`.
	constexpr TypeInfo const& type() const noexcept {
		return hasValue() ? storage_->typeInfo() : typeid(void);
	}
#endif

	/// @return A reference to the new contained object.
	template <class T, class... Args,
	    RB_REQUIRES(kStorable<T>&& isConstructible<Decay<T>, Args...>)>
	Decay<T>& emplace(Args&&... args) {
		storage_ = StoragePtr<T>::from(RB_FWD(args)...);
		return *static_cast<Decay<T>*>(storage_->get());
	}

	void reset() noexcept {
		storage_.reset();
	}

	constexpr void swap(Any& rhs) noexcept {
		storage_.swap(rhs.storage_);
	}

private:
	template <class T>
	friend constexpr T const* cast(Any const* any) noexcept;

	friend std::ostream& operator<<(std::ostream& os, Any const& any);

	struct IStorage;
	using IStoragePtr = UniquePtr<IStorage>;

	struct IStorage {
		virtual ~IStorage() = default;

		virtual void const* get() const noexcept = 0;

		virtual IStoragePtr clone() const = 0;

#if __cpp_rtti
		virtual TypeInfo const& typeInfo() const noexcept = 0;
#endif

		virtual void print(std::ostream& os) const = 0;
	};

	template <class T,
	    RB_REQUIRES(isCopyConstructible<T>)>
	struct Storage : IStorage {
		explicit Storage(T const& data)
		    : data(data) {
		}

		explicit Storage(T&& data)
		    : data(RB_MOVE(data)) {
		}

		void const* get() const noexcept override {
			return addressOf(data);
		}

		IStoragePtr clone() const override {
			return UniquePtr<Storage>::from(data);
		}

#if __cpp_rtti
		TypeInfo const& typeInfo() const noexcept override {
			return typeid(T);
		}
#endif

		void print(std::ostream& os) const override {
			if constexpr (isWritableTo<T, std::ostream>) {
				os << data;
			} else {
				os << "Any{?}";
			}
		}

		T data;
	};

	template <class T>
	using StoragePtr = UniquePtr<Storage<Decay<T>>>;

	template <class T>
	constexpr void const* cast() const noexcept {
		// cast<T> returns non-null if type() == typeid(T) and typeid(T) ignores cv-qualifiers so remove them
		using U = RemoveCv<T>;
		// - the contained value has a decayed type, so if Decay<U> is not U,
		//   then it's not possible to have a contained value of type U
		// - only copy constructible types can be used for contained values
		if constexpr (!isSame<Decay<U>, U> || !isCopyConstructible<U>) {
			return nullptr;
		}
#if __cpp_rtti
		else if (storage_ && type() == typeid(T)) {
			return storage_->get();
		}
#endif
		return nullptr;
	}

	void print(std::ostream& os) const {
		if (storage_) {
			storage_->print(os);
		} else {
			os << "Any{}";
		}
	}

	IStoragePtr storage_;
};

template <class T,
    RB_REQUIRES(isConstructible<T, RemoveCvRef<T> const&>)>
constexpr Option<T> cast(Any const& any) {
	using U = RemoveCvRef<T>;
	U const* ptr = cast<U>(&any);
	if (ptr) {
		return *ptr;
	}
	return kNone;
}

template <class T,
    RB_REQUIRES(isConstructible<T, RemoveCvRef<T>&>)>
constexpr Option<T> cast(Any& any) {
	using U = RemoveCvRef<T>;
	U* ptr = cast<U>(&any);
	if (ptr) {
		return static_cast<T>(*ptr);
	}
	return kNone;
}

template <class T,
    RB_REQUIRES(isConstructible<T, RemoveCvRef<T>> && !isRValueRef<T>)>
constexpr Option<T> cast(Any&& any) {
	using U = RemoveCvRef<T>;
	U* ptr = cast<U>(&any);
	if (!ptr) {
		return kNone;
	}

	if constexpr (isMoveConstructible<U> && !isLValueRef<T>) {
		return RB_MOVE(*ptr);
	} else {
		return *ptr;
	}
}

// currently we have no Option<T&&>
template <class T,
    RB_REQUIRES(isConstructible<T, RemoveCvRef<T>>&& isRValueRef<T>)>
constexpr T cast(Any&& any) {
	using U = RemoveCvRef<T>;
	U* ptr = cast<U>(&any);
	RB_ASSERT(ptr);
	return static_cast<T>(*ptr);
}

template <class T>
constexpr T const* cast(Any const* any) noexcept {
	if constexpr (isObject<T>) {
		if (any) {
			return static_cast<T const*>(any->cast<T>());
		}
	}
	return nullptr;
}

template <class T>
constexpr T* cast(Any* any) noexcept {
	return const_cast<T*>(cast<T>(static_cast<Any const*>(any))); // NOLINT(*-pro-type-const-cast)
}

template <class T, class... Args>
Any any(Args&&... args) noexcept(isNothrowConstructible<Any, InPlaceType<T>, Args...>) {
	return Any(kInPlaceType<T>, RB_FWD(args)...);
}

constexpr void swap(Any& lhs, Any& rhs) noexcept {
	lhs.swap(rhs);
}

inline std::ostream& operator<<(std::ostream& os, Any const& any) {
	any.print(os);
	return os;
}

} // namespace rb::core
