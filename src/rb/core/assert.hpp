#pragma once

#include <rb/core/SourceLocation.hpp>

namespace rb::core {

[[noreturn]] RB_EXPORT void throwAssert(char const* msg, RB_SOURCE_LOCATION_DECL);

} // namespace rb::core

#define RB_ASSERT(cond) \
	(cond) ? static_cast<void>(0) : rb::core::throwAssert(#cond, RB_SOURCE_LOCATION)

#define RB_ASSERT_MSG(cond, msg) \
	(cond) ? static_cast<void>(0) : rb::core::throwAssert(#cond " (" msg ")", RB_SOURCE_LOCATION)

#define RB_ASSERT_CUSTOM_MSG(cond, msg) \
	(cond) ? static_cast<void>(0) : rb::core::throwAssert(msg, RB_SOURCE_LOCATION)

#ifdef NDEBUG
	#define RB_DEBUG_ASSERT(cond)
	#define RB_DEBUG_ASSERT_MSG(cond, msg)
	#define RB_DEBUG_ASSERT_CUSTOM_MSG(cond, msg)
#else
	#define RB_DEBUG_ASSERT RB_ASSERT
	#define RB_DEBUG_ASSERT_MSG RB_ASSERT_MSG
	#define RB_DEBUG_ASSERT_CUSTOM_MSG RB_ASSERT_CUSTOM_MSG
#endif
