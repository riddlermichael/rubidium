#pragma once

#include <rb/core/processor.hpp>

// Machine byte-order, reuse preprocessor provided macros when available
#ifdef __ORDER_BIG_ENDIAN__
	#define RB_BIG_ENDIAN __ORDER_BIG_ENDIAN__
#else
	#define RB_BIG_ENDIAN 4321
#endif

#ifdef __ORDER_LITTLE_ENDIAN__
	#define RB_LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#else
	#define RB_LITTLE_ENDIAN 1234
#endif

#if defined(RB_PROCESSOR_ARM)
	#if defined(__ARMEL__) || defined(_M_ARM64)
		#define RB_BYTE_ORDER RB_LITTLE_ENDIAN
	#elif defined(__ARMEB__)
		#define RB_BYTE_ORDER RB_BIG_ENDIAN
	#endif
#elif defined(RB_PROCESSOR_MIPS)
	#if defined(__MIPSEL__)
		#define RB_BYTE_ORDER RB_LITTLE_ENDIAN
	#elif defined(__MIPSEB__)
		#define RB_BYTE_ORDER RB_BIG_ENDIAN
	#endif
#elif defined(RB_PROCESSOR_X86) || defined(RB_PROCESSOR_RISCV) || defined(RB_PROCESSOR_WASM)
	#define RB_BYTE_ORDER RB_LITTLE_ENDIAN
#elif defined(RB_PROCESSOR_S390) || defined(RB_PROCESSOR_SPARC)
	#define RB_BYTE_ORDER RB_BIG_ENDIAN
#endif

// Some processors support either endian format, try to detect which we are using
#ifndef RB_BYTE_ORDER
	#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == RB_BIG_ENDIAN || __BYTE_ORDER__ == RB_LITTLE_ENDIAN)
	    // Reuse __BYTE_ORDER__ as-is, since our RB_*_ENDIAN #defines match the preprocessor defaults
		#define RB_BYTE_ORDER __BYTE_ORDER__
	#elif defined(__BIG_ENDIAN__) || defined(_big_endian__) || defined(_BIG_ENDIAN)
		#define RB_BYTE_ORDER RB_BIG_ENDIAN
	#elif defined(__LITTLE_ENDIAN__) || defined(_little_endian__) || defined(_LITTLE_ENDIAN) || defined(WINAPI_FAMILY)
		#define RB_BYTE_ORDER RB_LITTLE_ENDIAN
	#else
		#error "Unable to determine byte order"
	#endif
#endif

#define RB_IS_LITTLE_ENDIAN (RB_BYTE_ORDER == RB_LITTLE_ENDIAN)
#define RB_IS_BIG_ENDIAN (RB_BYTE_ORDER == RB_BIG_ENDIAN)

namespace rb::core {

enum class Endian {
	kLittle = RB_LITTLE_ENDIAN,
	kBig = RB_BIG_ENDIAN,
	kNative = RB_BYTE_ORDER
};

enum : bool { // NOLINT(*-use-enum-class)
	kIsLittleEndian = RB_IS_LITTLE_ENDIAN,
	kIsBigEndian = RB_IS_BIG_ENDIAN,
};

} // namespace rb::core
