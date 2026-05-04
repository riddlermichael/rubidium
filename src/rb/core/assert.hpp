#pragma once

#include <rb/core/error/Error.hpp>

namespace rb::core {

class RB_EXPORT AssertError final : public Error {
public:
	using Error::Error;
};

class RB_HIDDEN TerminateHandlerSetter {
public:
	TerminateHandlerSetter() noexcept;
};

[[maybe_unused]] inline TerminateHandlerSetter const terminateHandlerSetter;

[[noreturn]] RB_EXPORT void throwAssert(czstring msg, RB_SOURCE_LOCATION_DECL);

} // namespace rb::core

#define RB_ASSERT_CUSTOM_MSG(msg, ...)     \
	do {                                   \
		if (RB_UNLIKELY(!(__VA_ARGS__))) { \
			rb::core::throwAssert(         \
			    msg,                       \
			    RB_SOURCE_LOCATION);       \
		}                                  \
	} while (0)

#define RB_ASSERT_MSG(msg, ...) RB_ASSERT_CUSTOM_MSG(#__VA_ARGS__ " (" msg ")", __VA_ARGS__)

#define RB_ASSERT(...) RB_ASSERT_CUSTOM_MSG(#__VA_ARGS__, __VA_ARGS__)

#ifdef NDEBUG
	#define RB_DEBUG_ASSERT(...)
	#define RB_DEBUG_ASSERT_MSG(msg, ...)
	#define RB_DEBUG_ASSERT_CUSTOM_MSG(msg, ...)
#else
	#define RB_DEBUG_ASSERT RB_ASSERT
	#define RB_DEBUG_ASSERT_MSG RB_ASSERT_MSG
	#define RB_DEBUG_ASSERT_CUSTOM_MSG RB_ASSERT_CUSTOM_MSG
#endif

#define RB_UNREACHABLE_ASSERT()                         \
	do {                                                \
		RB_ASSERT_MSG("RB_UNREACHABLE was reached", 0); \
		RB_UNREACHABLE();                               \
	} while (false)
