#pragma once

#include <iomanip>

#include <rb/core/move.hpp>
#include <rb/fmt/StreamStateSaver.hpp>

namespace rb::fmt {

template <class Char, class Traits>
struct HexHelper {
	std::basic_ostream<Char, Traits>& os;
	int width = 0;
};

struct Hex {
	int width = 0;
};

template <class Char, class Traits>
constexpr HexHelper<Char, Traits> operator<<(std::basic_ostream<Char, Traits>& os, Hex hex) {
	return {os, hex.width};
}

template <class Char, class Traits, class T>
std::basic_ostream<Char, Traits>& operator<<(HexHelper<Char, Traits> hexHelper, T&& value) {
	auto& os = hexHelper.os;
	StreamStateSaver _(os);
	auto const o = os.widen('0');
	auto const x = os.widen('x');
	int const w = hexHelper.width > 2 ? hexHelper.width : 2;
	os << o << x << std::hex << std::setfill(o) << std::setw(w) << RB_FWD(value);
	return os;
}

constexpr Hex hex;

constexpr Hex toHex(int width = 0) noexcept {
	return {width};
}

} // namespace rb::fmt
