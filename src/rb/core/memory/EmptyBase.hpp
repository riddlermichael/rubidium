#pragma once

#include <rb/core/memory/Wrapper.hpp>
#include <rb/core/traits/builtins.hpp>

namespace rb::core {
inline namespace memory {

	template <class T, int tag = 0,
	    bool = isEmpty<T> && !isFinal<T>> // unable to use RB_REQUIRES because of the partial specialization below
	class EmptyBase : T {
	public:
		constexpr EmptyBase() noexcept(isNothrowDefaultConstructible<T>) = default;

		constexpr explicit EmptyBase(T const& value) noexcept(isNothrowCopyConstructible<T>)
		    : T(value) {
		}

		constexpr explicit EmptyBase(T&& value) noexcept(isNothrowMoveConstructible<T>)
		    : T(RB_MOVE(value)) {
		}

		constexpr T& get() & noexcept {
			return *this;
		}

		constexpr T const& get() const& noexcept {
			return *this;
		}

		constexpr T&& get() && noexcept {
			return *this;
		}

		constexpr T const&& get() const&& noexcept {
			return *this;
		}
	};

	template <class T, int tag>
	class EmptyBase<T, tag, false> : public Wrapper<T> {
		using Super = Wrapper<T>;

	public:
		using Super::Super;
	};

} // namespace memory
} // namespace rb::core
