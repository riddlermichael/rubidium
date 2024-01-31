#pragma once

#include <iomanip>
#include <ostream>

#include <rb/core/move.hpp>
#include <rb/fmt/StreamStateSaver.hpp>

namespace rb::fmt {

template <class Char, class Traits>
struct ToHexHelper {
	std::basic_ostream<Char, Traits>& os;
};

struct ToHex {
};

template <class Char, class Traits>
constexpr ToHexHelper<Char, Traits> operator<<(std::basic_ostream<Char, Traits>& os, ToHex /*unused*/) {
	return {os};
}

template <class Char, class Traits, class T>
std::basic_ostream<Char, Traits>& operator<<(ToHexHelper<Char, Traits> toHexHelper, T&& value) {
	auto& os = toHexHelper.os;
	StreamStateSaver _(os);
	auto const o = os.widen('0');
	auto const x = os.widen('x');
	os << o << x << std::hex << std::setfill(o) << std::setw(2) << RB_FWD(value);
	return os;
}

constexpr ToHex toHex;

} // namespace rb::fmt
