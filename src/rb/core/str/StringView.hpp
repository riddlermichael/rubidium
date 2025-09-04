#pragma once

#include <ostream>
#include <string_view>

#include <rb/core/slices/ContiguousView.hpp>
#include <rb/core/str/CharTraits.hpp>
#include <rb/core/traits/requirements.hpp>

namespace rb::core {

inline namespace str {
	template <class Char, usize n>
	class StringViewImpl;
} // namespace str

template <class Char, usize n>
struct ContainerTraits<StringViewImpl<Char, n>> {
	using Value = RemoveCv<Char>;
	using Iterator = Char*;
	using ConstIterator = Char const*;
	using Difference = isize;
	using Size = usize;
};

inline namespace str {

	template <class Char, usize n = kDynExtent>
	class StringViewImpl final
	    : public ContiguousView<Char const, StringViewImpl<Char>, StringViewImpl<Char>> {
		using Super = ContiguousView<Char const, StringViewImpl<Char>, StringViewImpl<Char>>;

	public:
		RB_USE_BASE_CONTAINER_TYPES(Super)

		using Traits = CharTraits<Char>;

		// ctor.1
		template <bool _ = true, RB_REQUIRES(_ && (n == kDynExtent || n == 0))>
		constexpr StringViewImpl() noexcept
		    : Super() {
		}

		// ctor.3
		constexpr StringViewImpl(Char const* s, usize size) noexcept
		    : Super(s, size) {
		}

		// ctor.4
		constexpr StringViewImpl(Char const* s) noexcept // NOLINT(google-explicit-constructor)
		    : Super(s, Traits::length(s)) {
		}

		// ctor.5
		template <class It, class End,
		    RB_REQUIRES(!isConvertible<End, usize>)>
		constexpr StringViewImpl(It first, End last) RB_NOEXCEPT_LIKE(last - first)
		    : Super(first, last) {
		}

		constexpr StringViewImpl(StringViewImpl const&) noexcept = default;
		constexpr StringViewImpl(StringViewImpl&&) noexcept = default;

		constexpr StringViewImpl& operator=(StringViewImpl const&) noexcept = default;
		constexpr StringViewImpl& operator=(StringViewImpl&&) noexcept = default;

		template <class Traits = std::char_traits<RemoveCv<Char>>>
		constexpr std::basic_string_view<RemoveCv<Char>, Traits> toStdStringView() const noexcept {
			return {data(), size()};
		}

		template <class Traits>
		constexpr explicit operator std::basic_string_view<RemoveCv<Char>, Traits>() const noexcept {
			return toStdStringView();
		}

		using Super::begin;
		using Super::data;
		using Super::end;
		using Super::size;

		// from STL

		template <usize pos,
		    bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
		constexpr StringViewImpl<Char> substr() const noexcept {
			RB_CHECK_RANGE_EXCL(pos, 0, size());
			return {data() + pos, size() - pos};
		}

		template <usize pos,
		    bool _ = true, RB_REQUIRES(_&& n != kDynExtent && pos <= n)>
		constexpr StringViewImpl<Char, n - pos> substr() const noexcept {
			return {data() + pos, n - pos};
		}

		template <usize pos, usize count,
		    bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
		constexpr StringViewImpl<Char, count> substr() const noexcept {
			RB_CHECK_RANGE_EXCL(pos + count, 0, size());
			return {data() + pos, count};
		}

		template <usize pos, usize count,
		    bool _ = true, RB_REQUIRES(_&& n != kDynExtent && pos + count <= n)>
		constexpr StringViewImpl<Char, count> substr() const noexcept {
			return {data() + pos, count};
		}

		constexpr StringViewImpl<Char> substr(usize pos = 0) const noexcept {
			RB_CHECK_RANGE_EXCL(pos, 0, size());
			return {data() + pos, size() - pos};
		}

		constexpr StringViewImpl<Char> substr(usize pos, usize count) const noexcept {
			RB_CHECK_RANGE_EXCL(pos + count, 0, size());
			return {data() + pos, count};
		}

		template <usize count,
		    bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
		constexpr StringViewImpl<Char, count> removePrefix() const noexcept {
			RB_CHECK_RANGE_EXCL(count, 0, size());
			return {data() + count, size() - count};
		}

		template <usize count,
		    bool _ = true, RB_REQUIRES(_&& n != kDynExtent && count <= n)>
		constexpr StringViewImpl<Char, count> removePrefix() const noexcept {
			return {data() + count, size() - count};
		}

		constexpr StringViewImpl<Char> removePrefix(usize count) const noexcept {
			RB_CHECK_RANGE_EXCL(count, 0, size());
			return {data() + count, size() - count};
		}

		template <usize count,
		    bool _ = true, RB_REQUIRES(_&& n == kDynExtent)>
		constexpr StringViewImpl<Char, count> removeSuffix() const noexcept {
			RB_CHECK_RANGE_EXCL(count, 0, size());
			return {data(), size() - count};
		}

		template <usize count,
		    bool _ = true, RB_REQUIRES(_&& n != kDynExtent && count <= n)>
		constexpr StringViewImpl<Char, count> removeSuffix() const noexcept {
			return {data(), size() - count};
		}

		constexpr StringViewImpl<Char> removeSuffix(usize count) const noexcept {
			RB_CHECK_RANGE_EXCL(count, 0, size());
			return {data(), size() - count};
		}

		template <usize m>
		constexpr bool startsWith(StringViewImpl<Char, m> sv) const noexcept {
			if constexpr (n != kDynExtent && m != kDynExtent && n < m) {
				return false;
			} else {
				if (size() < sv.size()) {
					return false;
				}
				return Traits::compare(data(), sv.data(), sv.size()) == 0;
			}
		}

		constexpr bool startsWith(Char ch) const noexcept {
			return size() && data()[0] == ch;
		}

		constexpr bool startsWith(Char const* s) const noexcept {
			return startsWith(StringViewImpl<Char>{s});
		}

		template <usize m>
		constexpr bool endsWith(StringViewImpl<Char, m> sv) const noexcept {
			if constexpr (n != kDynExtent && m != kDynExtent && n < m) {
				return false;
			} else {
				if (size() < sv.size()) {
					return false;
				}
				return !Traits::compare(data() + (size() - sv.size()), sv.data(), sv.size());
			}
		}

		constexpr bool endsWith(Char ch) const noexcept {
			return size() && this->back() == ch;
		}

		constexpr bool endsWith(Char const* s) const noexcept {
			return endsWith(StringViewImpl<Char>{s});
		}

		template <usize _>
		constexpr int compare(StringViewImpl<Char, _> sv) const noexcept {
			auto const len = size() < sv.size() ? size() : sv.size();
			int const cmp = Traits::compare(data(), sv.data(), len);
			if (cmp) {
				return cmp;
			}
			return (size() > sv.size()) - (size() < sv.size());
		}
	};

	template <class Char, usize n, usize m>
	constexpr bool operator==(StringViewImpl<Char, n> lhs, StringViewImpl<Char, m> rhs) noexcept {
		if constexpr (n != kDynExtent && m != kDynExtent && n != m) {
			return false;
		} else {
			if (lhs.size() != rhs.size()) {
				return false;
			}
			return CharTraits<Char>::compare(lhs.data(), rhs.data(), lhs.size());
		}
	}

	template <class Char, usize lhsSize, usize rhsSize>
	constexpr bool operator!=(StringViewImpl<Char, lhsSize> lhs, StringViewImpl<Char, rhsSize> rhs) noexcept {
		return !(lhs == rhs);
	}

	template <class Char, class Traits, usize n,
	    RB_REQUIRES_T(IsWritableTo<Char, std::ostream>)>
	std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, StringViewImpl<Char, n> sv) {
		return os << sv.template toStdStringView<Traits>();
	}

	using StringView = StringViewImpl<char>;

	using StringViewW = StringViewImpl<wchar_t>;

#ifdef __cpp_char8_t
	using StringView8 = StringViewImpl<char8_t>;

	template <usize n = kDynExtent>
	using StaticStringView8 = StringViewImpl<char8_t, n>;
#endif

	using StringView16 = StringViewImpl<char16_t>;

	using StringView32 = StringViewImpl<char32_t>;

	template <usize n = kDynExtent>
	using StaticStringView = StringViewImpl<char, n>;

	template <usize n = kDynExtent>
	using StaticStringViewW = StringViewImpl<wchar_t, n>;

	template <usize n = kDynExtent>
	using StaticStringView16 = StringViewImpl<char16_t, n>;

	template <usize n = kDynExtent>
	using StaticStringView32 = StringViewImpl<char32_t, n>;

} // namespace str

inline namespace literals {

	constexpr StringView operator""_sv(char const* s, usize size) noexcept {
		return {s, size};
	}

	constexpr StringViewW operator""_sv(wchar_t const* s, usize size) noexcept {
		return {s, size};
	}

	constexpr StringView16 operator""_sv(char16_t const* s, usize size) noexcept {
		return {s, size};
	}

	constexpr StringView32 operator""_sv(char32_t const* s, usize size) noexcept {
		return {s, size};
	}

#ifdef __cpp_char8_t
	constexpr StringView8 operator""_sv(char8_t const* s, usize size) noexcept {
		return {s, size};
	}
#endif

} // namespace literals

} // namespace rb::core
