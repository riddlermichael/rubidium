#pragma once

namespace rb::core {
inline namespace memory {

	template <class T>
	[[nodiscard]] constexpr T* addressOf(T& ref) noexcept {
		return __builtin_addressof(ref);
	}

	template <class T>
	T const* addressOf(T const&&) = delete;

} // namespace memory
} // namespace rb::core
