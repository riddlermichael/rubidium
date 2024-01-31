#pragma once

/*
 * This file uses preprocessor #defines to set various RB_PROCESSOR_* #defines based on the following patterns:
 * RB_PROCESSOR_{FAMILY}
 * RB_PROCESSOR_{FAMILY}_{VARIANT}
 * RB_PROCESSOR_{FAMILY}_{REVISION}
 * The first is always defined. Defines for the various revisions/variants are optional
 * and usually dependent on how the compiler was invoked.
 * Variants that are a superset of another should have a #define for the superset.
 */

// ARM family, known revisions: V5, V6, V7, V8
#if defined(__arm__) || defined(__TARGET_ARCH_ARM) || defined(_M_ARM) \
    || defined(_M_ARM64) || defined(__aarch64__) || defined(__ARM64__)
	#if defined(__aarch64__) || defined(__ARM64__) || defined(_M_ARM64)
		#define RB_PROCESSOR_ARM_64
		#define RB_PROCESSOR_WORDSIZE 8
	#else
		#define RB_PROCESSOR_ARM_32
	#endif
	#if defined(__ARM_ARCH) && __ARM_ARCH > 1
		#define RB_PROCESSOR_ARM __ARM_ARCH
	#elif defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM > 1
		#define RB_PROCESSOR_ARM __TARGET_ARCH_ARM
	#elif defined(_M_ARM) && _M_ARM > 1
		#define RB_PROCESSOR_ARM _M_ARM
	#elif defined(__ARM64_ARCH_8__) || defined(__aarch64__) || defined(__ARMv8__) || defined(__ARMv8_A__) || defined(_M_ARM64)
		#define RB_PROCESSOR_ARM 8
	#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) \
	    || defined(__ARM_ARCH_7S__) || defined(_ARM_ARCH_7) || defined(__CORE_CORTEXA__)
		#define RB_PROCESSOR_ARM 7
	#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6T2__) || defined(__ARM_ARCH_6Z__) \
	    || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_6M__)
		#define RB_PROCESSOR_ARM 6
	#elif defined(__ARM_ARCH_5TEJ__) || defined(__ARM_ARCH_5TE__)
		#define RB_PROCESSOR_ARM 5
	#else
		#define RB_PROCESSOR_ARM 0
	#endif
	#if RB_PROCESSOR_ARM >= 8
		#define RB_PROCESSOR_ARM_V8
	#endif
	#if RB_PROCESSOR_ARM >= 7
		#define RB_PROCESSOR_ARM_V7
	#endif
	#if RB_PROCESSOR_ARM >= 6
		#define RB_PROCESSOR_ARM_V6
	#endif
	#if RB_PROCESSOR_ARM >= 5
		#define RB_PROCESSOR_ARM_V5
	#else
		#error "ARM architecture too old"
	#endif
// X86 family, known variants: 32- and 64-bit
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
	#define RB_PROCESSOR_X86_32
	#define RB_PROCESSOR_WORDSIZE 4
    /*
     * We define RB_PROCESSOR_X86 == 6 for anything above a equivalent or better than a Pentium Pro
     * (the processor whose architecture was called P6) or an Athlon.
     * All processors since the Pentium III and the Athlon 4 have SSE support,
     * so we use that to detect. That leaves the original Athlon, Pentium Pro and Pentium II.
     */
	#if defined(_M_IX86)
		#define RB_PROCESSOR_X86 (_M_IX86 / 100)
	#elif defined(__i686__) || defined(__athlon__) || defined(__SSE__) || defined(__pentiumpro__)
		#define RB_PROCESSOR_X86 6
	#elif defined(__i586__) || defined(__k6__) || defined(__pentium__)
		#define RB_PROCESSOR_X86 5
	#elif defined(__i486__) || defined(__80486__)
		#define RB_PROCESSOR_X86 4
	#else
		#define RB_PROCESSOR_X86 3
	#endif
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
	#define RB_PROCESSOR_X86 6
	#define RB_PROCESSOR_X86_64
	#define RB_PROCESSOR_WORDSIZE 8
// Itanium (IA-64) family, no revisions or variants
#elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
	#define RB_PROCESSOR_IA64
	#define RB_PROCESSOR_WORDSIZE 8
// MIPS family, known revisions: I, II, III, IV, 32, 64
#elif defined(__mips) || defined(__mips__) || defined(_M_MRX000)
	#define RB_PROCESSOR_MIPS
	#if defined(_MIPS_ARCH_MIPS1) || (defined(__mips) && __mips - 0 >= 1)
		#define RB_PROCESSOR_MIPS_I
	#endif
	#if defined(_MIPS_ARCH_MIPS2) || (defined(__mips) && __mips - 0 >= 2)
		#define RB_PROCESSOR_MIPS_II
	#endif
	#if defined(_MIPS_ARCH_MIPS3) || (defined(__mips) && __mips - 0 >= 3)
		#define RB_PROCESSOR_MIPS_III
	#endif
	#if defined(_MIPS_ARCH_MIPS4) || (defined(__mips) && __mips - 0 >= 4)
		#define RB_PROCESSOR_MIPS_IV
	#endif
	#if defined(_MIPS_ARCH_MIPS5) || (defined(__mips) && __mips - 0 >= 5)
		#define RB_PROCESSOR_MIPS_V
	#endif
	#if defined(_MIPS_ARCH_MIPS32) || defined(__mips32) || (defined(__mips) && __mips - 0 >= 32)
		#define RB_PROCESSOR_MIPS_32
	#endif
	#if defined(_MIPS_ARCH_MIPS64) || defined(__mips64)
		#define RB_PROCESSOR_MIPS_64
		#define RB_PROCESSOR_WORDSIZE 8
	#endif
// Power family, known variants: 32- and 64-bit
#elif defined(__ppc__) || defined(__ppc) || defined(__powerpc__) || defined(_ARCH_COM) || defined(_ARCH_PWR) \
    || defined(_ARCH_PPC) || defined(_M_MPPC) || defined(_M_PPC)
	#define RB_PROCESSOR_POWER
	#if defined(__ppc64__) || defined(__powerpc64__) || defined(__64BIT__)
		#define RB_PROCESSOR_POWER_64
		#define RB_PROCESSOR_WORDSIZE 8
	#else
		#define RB_PROCESSOR_POWER_32
	#endif
// RISC-V family, known variants: 32- and 64-bit
#elif defined(__riscv)
	#define RB_PROCESSOR_RISCV
	#if __riscv_xlen == 64
		#define RB_PROCESSOR_RISCV_64
	#else
		#define RB_PROCESSOR_RISCV_32
	#endif
// S390 family, known variant: S390X (64-bit)
#elif defined(__s390__)
	#define RB_PROCESSOR_S390
	#if defined(__s390x__)
		#define RB_PROCESSOR_S390_X
	#endif
// SPARC family, optional revision: V9
#elif defined(__sparc__)
	#define RB_PROCESSOR_SPARC
	#if defined(__sparc_v9__)
		#define RB_PROCESSOR_SPARC_V9
	#endif
	#if defined(__sparc64__)
		#define RB_PROCESSOR_SPARC_64
	#endif
// Web Assembly
#elif defined(__EMSCRIPTEN__)
	#define RB_PROCESSOR_WASM
	#define RB_PROCESSOR_WORDSIZE 8
#endif

/*
 * Size of a pointer and the machine register size. We detect a 64-bit system by:
 * - GCC and compatible compilers (Clang, ICC on OS X and Windows) always define __SIZEOF_POINTER__.
 * This catches all known cases of ILP32 builds on 64-bit processors.
 * - Most other Unix compilers define __LP64__ or _LP64 in 64-bit mode
 * - If RB_PROCESSOR_WORDSIZE was defined above, it's assumed to match the pointer size
 * - Otherwise, we assume to be 32-bit
 */
#if defined __SIZEOF_POINTER__
	#define RB_POINTER_SIZE __SIZEOF_POINTER__
#elif defined(__LP64__) || defined(_LP64)
	#define RB_POINTER_SIZE 8
#elif defined(RB_PROCESSOR_WORDSIZE)
	#define RB_POINTER_SIZE RB_PROCESSOR_WORDSIZE
#else
	#define RB_POINTER_SIZE 4
#endif

#if (RB_POINTER_SIZE == 8)
	#define RB_IS_32BIT 0
	#define RB_IS_64BIT 1
#else
	#define RB_IS_32BIT 1
	#define RB_IS_64BIT 0
#endif
