#pragma once

#include <ostream>

#include <rb/core/traits/IsEnumerable.hpp>

namespace rb::fmt {

template <class String,
    RB_REQUIRES(core::isEnumerable<String>)>
struct QuotedString {
	using Traits = core::EnumerableTraits<String>;
	using Char = typename Traits::Value;

	typename Traits::Begin begin;
	typename Traits::End end;
	Char delim;
	Char escape;
};

template <class String, class Char = typename QuotedString<String>::Char>
constexpr auto quoted(String const& s, Char delim = Char('"'), Char escape = Char('\\')) noexcept {
	return QuotedString<String const>{core::begin(s), core::end(s), delim, escape};
}

template <class String, class Char, class Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, QuotedString<String> const& qs) {
	os << qs.delim;
	for (auto it = qs.begin; it != qs.end; ++it) {
		auto ch = *it;
		if (Traits::eq(ch, qs.delim) || Traits::eq(ch, qs.escape)) {
			os << qs.escape;
		}
		os << ch;
	}
	return os << qs.delim;
}

} // namespace rb::fmt
