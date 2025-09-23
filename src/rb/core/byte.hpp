#pragma once

// ReSharper disable CppInconsistentNaming, CppUnusedIncludeDirective

#include <ostream>

#include <rb/core/enums.hpp>

namespace rb::core {

/**
 * @c rb::core::byte is a distinct type that implements the concept of byte as specified in the C++ language definition.
 *
 * Like @c char and <tt>unsigned char</tt>, it can be used to access object representation, but unlike those types,
 * it is not a character type and is not an arithmetic type.
 * A @c byte is only a collection of bits, and the only operators defined for it are the bitwise ones.
 */
enum class byte : unsigned char { // NOLINT(readability-identifier-naming)
};

/**
 * Equivalent to <tt>return T(value);</tt>.
 * This overload only participates in overload resolution if @c core::isIntegral&lt;T&gt; is @c true.
 */
template <class T,
    RB_REQUIRES(isIntegral<T>)>
constexpr T toInt(byte value) noexcept {
	return static_cast<T>(value);
}

template <class T,
    RB_REQUIRES(isIntegral<T>)>
constexpr byte operator<<(byte b, T shift) noexcept {
	return static_cast<byte>(toInt(b) << shift);
}

template <class T,
    RB_REQUIRES(isIntegral<T>)>
constexpr byte& operator<<=(byte& b, T shift) noexcept {
	return b = b << shift;
}

template <class T,
    RB_REQUIRES(isIntegral<T>)>
constexpr byte operator>>(byte b, T shift) noexcept {
	return static_cast<byte>(toInt(b) >> shift);
}

template <class T,
    RB_REQUIRES(isIntegral<T>)>
constexpr byte& operator>>=(byte& b, T shift) noexcept {
	return b = b >> shift;
}

constexpr byte operator|(byte lhs, byte rhs) noexcept {
	return static_cast<byte>(toInt(lhs) | toInt(rhs));
}

constexpr byte& operator|=(byte& lhs, byte rhs) noexcept {
	return lhs = lhs | rhs;
}

constexpr byte operator&(byte lhs, byte rhs) noexcept {
	return static_cast<byte>(toInt(lhs) & toInt(rhs));
}

constexpr byte& operator&=(byte& lhs, byte rhs) noexcept {
	return lhs = lhs & rhs;
}

constexpr byte operator^(byte lhs, byte rhs) noexcept {
	return static_cast<byte>(toInt(lhs) ^ toInt(rhs));
}

constexpr byte& operator^=(byte& lhs, byte rhs) noexcept {
	return lhs = lhs ^ rhs;
}

constexpr byte operator~(byte b) noexcept {
	return static_cast<byte>(~toInt(b));
}

template <class Char, class Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, byte value) {
	// slightly optimized version without `std::hex`, `std::setw`, and `std::setfill`
	using Int = typename Traits::int_type;

	auto toChar = [&os](Int i) {
		return Traits::to_char_type(
		    i < 10
		        ? Traits::to_int_type(os.widen('0')) + i
		        : Traits::to_int_type(os.widen('a')) + i - 10);
	};

	auto const v = static_cast<unsigned>(value);
	os << os.widen('0') << os.widen('x');
	os << toChar(v / 16);
	os << toChar(v % 16);
	return os;
}

} // namespace rb::core
