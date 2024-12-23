#pragma once

#include <initializer_list>

#include <rb/core/enums.hpp>
#include <rb/core/requires.hpp>
#include <rb/fmt/hex.hpp>

namespace rb::core {

template <class E,
    RB_REQUIRES(isEnum<E>)>
class Flags {
public:
	constexpr Flags() noexcept = default;

	constexpr explicit Flags(E flag) noexcept
	    : value_(toUnderlying(flag)) {
	}

	constexpr Flags(std::initializer_list<E> flags) noexcept {
		for (auto flag : flags) {
			value_ |= toUnderlying(flag);
		}
	}

	constexpr explicit operator bool() const noexcept {
		return value_;
	}

	constexpr bool operator!() const noexcept {
		return !value_;
	}

	constexpr Flags operator~() const noexcept {
		return {~value_};
	}

	constexpr Flags& operator|=(Flags rhs) noexcept {
		value_ |= rhs.value_;
		return *this;
	}

	constexpr Flags& operator|=(E flag) noexcept {
		value_ |= toUnderlying(flag);
		return *this;
	}

	constexpr Flags& operator&=(Flags rhs) noexcept {
		value_ &= rhs.value_;
		return *this;
	}

	constexpr Flags& operator&=(E flag) noexcept {
		value_ &= toUnderlying(flag);
		return *this;
	}

	constexpr Flags& operator^=(Flags rhs) noexcept {
		value_ ^= rhs.value_;
		return *this;
	}

	constexpr Flags& operator^=(E flag) noexcept {
		value_ ^= toUnderlying(flag);
		return *this;
	}

	constexpr Flags operator|(Flags rhs) const noexcept {
		return Flags(*this) |= rhs;
	}

	constexpr Flags operator|(E flag) const noexcept {
		return Flags(*this) |= flag;
	}

	constexpr Flags operator&(Flags rhs) const noexcept {
		return Flags(*this) &= rhs;
	}

	constexpr Flags operator&(E flag) const noexcept {
		return Flags(*this) &= flag;
	}

	constexpr Flags operator^(Flags rhs) const noexcept {
		return Flags(*this) ^= rhs;
	}

	constexpr Flags operator^(E flag) const noexcept {
		return Flags(*this) ^= flag;
	}

	constexpr Flags& setFlag(E flag, bool on = true) noexcept {
		auto const value = toUnderlying(flag);
		on ? (value_ |= value) : (value_ &= ~value);
		return *this;
	}

	constexpr bool testFlag(E flag) const noexcept {
		auto const value = toUnderlying(flag);
		return value ? ((value_ & value) == value) : !value_;
	}

	friend std::ostream& operator<<(std::ostream& os, Flags flags) {
		return os << fmt::hex << flags.value_;
	}

private:
	UnderlyingType<E> value_ = {};
};

} // namespace rb::core
