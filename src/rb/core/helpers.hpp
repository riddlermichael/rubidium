#pragma once

/// Used for macro's arguments.
#define RB_STRINGIFY_X(x) RB_STRINGIFY(x)

///
#define RB_STRINGIFY(x) #x

/// Used to mark (possibly) unused variables.
#define RB_UNUSED(x) static_cast<void>(x)

/// Replacement for annoying `noexcept(noexcept(expr))`.
#define RB_NOEXCEPT_LIKE(...) noexcept(noexcept(__VA_ARGS__))

///
#define RB_DISABLE_COPY(T) \
	T(T const&) = delete;  \
	T& operator=(T const&) = delete;

///
#define RB_DISABLE_MOVE(T)    \
	T(T&&) noexcept = delete; \
	T& operator=(T&&) noexcept = delete;

///
#define RB_DISABLE_COPY_MOVE(T) \
	RB_DISABLE_COPY(T)          \
	RB_DISABLE_MOVE(T)

///
#define RB_CHECK_COMPLETENESS(T) static_assert(sizeof(T) > 0, "incomplete type")
