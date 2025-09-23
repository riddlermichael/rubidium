#pragma once

#include <initializer_list>
#include <string>

#include <rb/core/str/StringView.hpp>
#include <rb/core/warnings.hpp>
#include <rb/fmt/charconv.hpp>

// ReSharper disable CppMemberFunctionMayBeStatic

namespace rb::core {

inline namespace str {
	template <usize kSize, class Char>
	class StaticString;
} // namespace str

template <usize kSize, class Char>
struct ContainerTraits<StaticString<kSize, Char>> {
	using ConstIterator = Char const*;
	using Difference = isize;
	using Iterator = Char*;
	using Size = usize;
	using Value = Char;
};

inline namespace str {

	template <usize kSize, class Char = char>
	class StaticString final
	    : public Sliceable<StaticString<kSize, Char>, StringViewImpl<Char>, StringViewImpl<Char>> {
		Char data_[kSize + 1] = {};

	public:
		using Self = StaticString;
		using Super = Sliceable<Self, StringViewImpl<Char>, StringViewImpl<Char>>;

		using ConstIterator = Char const*;
		using Difference = isize;
		using Iterator = Char*;
		using Size = usize;
		using Value = Char;

		/// Constructs empty string.
		constexpr StaticString() noexcept = default;

		/// Constructs the string with @p count copies of character @p ch.
		constexpr StaticString(usize count, Char ch) noexcept {
			if (count > kSize) {
				count = kSize;
			}
			for (usize pos = 0; pos < count; ++pos) {
				data_[pos] = ch;
			}
		}

		/// Constructs the string with the first @p count characters of character string pointed to by @p s.
		/// @p s can contain null characters.
		constexpr StaticString(Char const* s, usize count) noexcept {
			usize const size = count < kSize ? count : kSize;
			for (usize pos = 0; pos < size; ++pos) {
				data_[pos] = s[pos];
			}
		}

		/// Constructs the string with the contents initialized with a copy
		/// of the null-terminated character string pointed to by @p s.
		/// The length of the string is determined by the first null character.
		constexpr explicit StaticString(Char const* s) noexcept {
			for (usize pos = 0; (pos < kSize) && s[pos]; ++pos) {
				data_[pos] = s[pos];
			}
		}

		/// Constructs the string with the contents of the range [@p first, @p last).
		template <class InputIt>
		constexpr StaticString(InputIt first, InputIt last)
		    noexcept(noexcept(first != last) && noexcept(++first) && noexcept(*first)) { // NOLINT(*-inc-dec-in-conditions)
			for (usize pos = 0; (pos < kSize) && (first != last); ++pos, ++first) {
				data_[pos] = *first;
			}
		}

		/// Constructs the string with the contents of the initializer list @p il.
		constexpr StaticString(std::initializer_list<Char> il) noexcept
		    : StaticString(il.begin(), il.end()) {
		}

		template <class Traits>
		constexpr explicit StaticString(std::basic_string_view<Char, Traits> const& sv)
		    : StaticString(sv.data(), sv.size()) {
		}

		/// StaticString cannot be constructed from `nullptr`.
		// ReSharper disable once CppNonExplicitConvertingConstructor
		StaticString(std::nullptr_t) = delete;

		template <usize n>
		constexpr explicit StaticString(Char const (&data)[n]) noexcept
		    : StaticString(data, n) {
		}

		template <usize _>
		constexpr explicit StaticString(StaticString<_, Char> const& rhs) noexcept
		    : StaticString(rhs.data(), rhs.size()) {
		}

		using Super::operator[];

		constexpr Char operator[](usize pos) const noexcept { // NOLINT(bugprone-exception-escape)
			RB_CHECK_RANGE(pos, 0, kSize);
			return data_[pos];
		}

		constexpr explicit operator std::basic_string<Char>() const noexcept {
			return std::basic_string<Char>(data_, kSize);
		}

		constexpr explicit operator std::basic_string_view<Char>() const noexcept {
			return std::basic_string_view<Char>(data_, kSize);
		}

		constexpr ConstIterator begin() const noexcept {
			return data_;
		}

		constexpr Iterator begin() noexcept {
			return data_;
		}

		constexpr ConstIterator cbegin() const noexcept {
			return data_;
		}

		constexpr ConstIterator end() const noexcept {
			return data_ + kSize;
		}

		constexpr Iterator end() noexcept {
			return data_ + kSize;
		}

		constexpr ConstIterator cend() const noexcept {
			return data_ + kSize;
		}

		template <usize pos,
		    RB_REQUIRES(pos < kSize)>
		constexpr Char at() const noexcept {
			return data_[pos];
		}

		constexpr Char const* data() const noexcept {
			return data_;
		}

		constexpr Char const* c_str() const noexcept { // NOLINT(readability-identifier-naming)
			return data_;
		}

		constexpr usize size() const noexcept {
			return kSize;
		}

		constexpr usize length() const noexcept {
			usize len = 0;
			while ((len < kSize) && data_[len]) {
				++len;
			}
			return len;
		}

		[[nodiscard]] constexpr bool empty() const noexcept {
			return kSize == 0;
		}

		template <bool _ = true, RB_REQUIRES(_&& kSize != 0)>
		constexpr Char front() const noexcept {
			return data_[0];
		}

		template <bool _ = true, RB_REQUIRES(_&& kSize != 0)>
		constexpr Char back() const noexcept {
			return data_[kSize - 1];
		}

		template <usize pos>
		constexpr auto substr() const noexcept -> EnableIf<pos <= kSize, StaticString<kSize - pos, Char>> {
			return {data_ + pos, kSize - pos};
		}

		template <usize pos, usize count>
		constexpr auto substr() const noexcept -> EnableIf<pos + count <= kSize, StaticString<count, Char>> {
			return {data_ + pos, count};
		}

		template <usize _>
		constexpr bool startsWith(StaticString<_, Char> const& prefix) const noexcept {
			if (size() < prefix.size()) {
				return false;
			}

			usize pos = 0;
			for (auto ch : prefix) {
				if (data_[pos++] != ch) {
					return false;
				}
			}
			return true;
		}

		/// Replaces the part of the string indicated by [@p pos, @p pos + @p count) with a new string @p s.
		template <usize count,
		    RB_REQUIRES(count <= kSize)>
		constexpr Self& replace(usize pos, StaticString<count, Char> const& s) noexcept { // NOLINT(bugprone-exception-escape)
			RB_CHECK_RANGE(pos, 0, kSize - count + 1);
			auto* ptr = data_ + pos;
			for (auto ch : s) {
				*ptr++ = ch;
			}
			return *this;
		}

		/// Replaces the part of the string indicated by [@p pos, @p pos + @p count) with a new string @p s.
		template <usize pos, usize count,
		    RB_REQUIRES(pos + count <= kSize)>
		constexpr Self& replace(StaticString<count, Char> const& s) noexcept {
			auto* ptr = data_ + pos;
			for (auto ch : s) {
				*ptr++ = ch;
			}
			return *this;
		}
	};

	template <usize kSize>
	using StaticStringW = StaticString<kSize, wchar_t>;

#ifdef __cpp_char8_t
	template <usize kSize>
	using StaticString8 = StaticString<kSize, char8_t>;
#endif

	template <usize kSize>
	using StaticString16 = StaticString<kSize, char16_t>;

	template <usize kSize>
	using StaticString32 = StaticString<kSize, char32_t>;

	template <class Char, usize kSize>
	StaticString(Char const (&)[kSize]) -> StaticString<kSize, Char>;

} // namespace str

inline namespace literals {

#ifndef RB_COMPILER_MSVC

	RB_WARNING_PUSH
	RB_STRING_LITERAL_OPERATOR_TEMPLATE

	template <class Char, Char... chars>
	constexpr auto operator""_ss() noexcept {
		return StaticString<sizeof...(chars), Char>({chars...});
	}

	RB_WARNING_POP

#endif

	// 123_ss -> "123"
	template <char... chars>
	constexpr auto operator""_ss() noexcept {
		return StaticString({chars...});
	}

} // namespace literals

inline namespace str {

	template <class Char, class Traits, usize kSize>
	std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, StaticString<kSize, Char> const& ss) {
		auto const fill = [&os](auto count) {
			for (; count > 0; --count) {
				os.rdbuf()->sputc(os.fill());
			}
		};

		bool const adjustLeft = (os.flags() & std::ios_base::adjustfield) == std::ios_base::left;
		usize const w = os.width();
		auto count = ss.size() < w ? w - ss.size() : 0;
		if (count && !adjustLeft) {
			fill(count);
		}
		os.rdbuf()->sputn(ss.data(), ss.size());
		if (count && adjustLeft) {
			fill(count);
		}
		os.width(0);
		return os;
	}

	template <class Char, usize kSize>
	constexpr auto toStdString(StaticString<kSize, Char> const& ss) {
		return std::basic_string<Char>(ss.data(), ss.size());
	}

	template <class Char, usize lhsSize, usize rhsSize>
	constexpr auto operator+(
	    StaticString<lhsSize, Char> const& lhs,
	    StaticString<rhsSize, Char> const& rhs) noexcept {
		return StaticString<lhsSize + rhsSize, Char>{lhs}.replace(lhs.size(), rhs);
	}

	template <auto value, unsigned base = 10, class Char = char>
	constexpr auto toStaticString() noexcept {
		constexpr auto kSize = fmt::countChars<base>(value);
		using String = StaticString<kSize, Char>;
		String repr;
		(void) fmt::toChars<base>(repr.begin(), repr.end(), value);
		return repr;
	}

	template <unsigned base = 10, class Char = char, class T>
	constexpr auto toStaticString(T value) noexcept {
		constexpr auto kSize = fmt::maxChars<T, base>();
		using String = StaticString<kSize, Char>;
		String repr;
		(void) fmt::toChars<base>(repr.begin(), repr.end(), value);
		return repr;
	}

	template <class Char>
	constexpr bool isSpace(Char ch) noexcept {
		switch (ch) {
			case ' ':
			case '\f':
			case '\n':
			case '\r':
			case '\t':
			case '\v':
				return true;

			default:
				return false;
		}
	}

	template <class T = u64, unsigned base = 10, class Char, usize kSize>
	constexpr fmt::FromCharsResult<T> toInt(StaticString<kSize, Char> const& ss, usize* pos = nullptr) noexcept {
		Char const* first = ss.data();
		Char const* last = ss.end();
		while (first < last && isSpace(*first)) {
			++first;
		}
		Char const* ptr = nullptr;
		auto const result = fmt::fromChars<T, base>(first, last, &ptr);
		if (pos) {
			*pos = ptr - ss.data();
		}
		return result;
	}

} // namespace str

} // namespace rb::core

#define RB_TO_SS(s) rb::core::StaticString<rb::core::size(s) - 1, rb::core::RemoveCvRef<decltype(s[0])>>(s)
