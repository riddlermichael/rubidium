#include "sanitizers.hpp"

#if RB_ENABLED(ADDRESS_SANITIZER)
// no_sanitize_address on GCC simply doesn't work if we have invalid pointer pairs; see fmt::toChars
[[maybe_unused]] char const* __asan_default_options(void) { // NOLINT
	return "check_initialization_order=1"
	       ":detect_stack_use_after_return=1"
	       ":strict_init_order=1"
	       ":strict_string_checks=1"
	#ifndef RB_COMPILER_GCC
	       ":detect_invalid_pointer_pairs=2"
	#endif
	#ifdef RB_COMPILER_MSVC
	       ":windows_hook_rtl_allocators=1"
	#endif
	    ;
}
#endif

#if RB_ENABLED(MEMORY_SANITIZER)
[[maybe_unused]] char const* __msan_default_options(void) { // NOLINT
	return "poison_in_dtor=1";
}
#endif

#if RB_ENABLED(UB_SANITIZER)
// see random::bitGen
[[maybe_unused]] char const* __ubsan_default_options(void) { // NOLINT
	return "silence_unsigned_overflow=1";
}
#endif
