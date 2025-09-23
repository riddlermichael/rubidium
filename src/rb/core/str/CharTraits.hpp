#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <cstring>

#include <rb/core/assert.hpp>
#include <rb/core/attributes.hpp>
#include <rb/core/traits/enums.hpp>
#include <rb/core/traits/IsSame.hpp>
#include <rb/core/types.hpp>

namespace rb::core {

namespace impl {

	template <class T, bool = isEnum<T>>
	struct IsU8BasedEnum : IsSame<UnderlyingType<T>, u8> {};

	template <class T>
	struct IsU8BasedEnum<T, false> : False {};

} // namespace impl

inline namespace str {

	template <class Char>
	struct CharTraits final {
	private:
#ifdef __cpp_char8_t
		using Char8 = char8_t;
#else
		using Char8 = unsigned char;
#endif

		using CharT = RemoveCv<Char>;

		static constexpr int cmp(Char const* lhs, Char const* rhs, usize count) noexcept {
			for (; count; --count, ++lhs, ++rhs) {
				if (*lhs != *rhs) {
					return *lhs < *rhs ? -1 : 1;
				}
			}
			return 0;
		}

	public:
		static constexpr int compare(
#ifdef RB_COMPILER_MSVC
		    _In_reads_(count) Char const* lhs, _In_reads_(count) Char const* rhs, usize count
#else
		    Char const* lhs, Char const* rhs, usize count
#endif
		    ) noexcept RB_NONNULL {
			RB_DEBUG_ASSERT(lhs && rhs);
			if constexpr (
			    isSame<CharT, unsigned char>
			    || isSame<CharT, Char8>
			    || isSame<CharT, char> && (static_cast<char>(-1) > '\0') // char is unsigned
			    || impl::IsU8BasedEnum<CharT>::value) //
			{
				return __builtin_memcmp(lhs, rhs, count);
			}
#if defined(RB_COMPILER_MSVC) || defined(RB_COMPILER_CLANG)
			else if constexpr (isSame<CharT, wchar_t>)
			{
				return __builtin_wmemcmp(lhs, rhs, count);
			}
#endif
			else
			{
				return cmp(lhs, rhs, count);
			}
		}

		static constexpr usize length(
#ifdef RB_COMPILER_MSVC
		    _In_z_
#endif
		        Char const* s) noexcept RB_NONNULL {
			RB_DEBUG_ASSERT(s);
			usize count = 0;
			while (*s++) {
				++count;
			}
			return count;
		}
	};

} // namespace str

} // namespace rb::core
