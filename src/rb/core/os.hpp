#pragma once

/**
 * The operating system, must be one of (RB_OS_*):
 * AIX      - AIX
 * ANDROID  - Android platform (also RB_OS_LINUX defined)
 * CYGWIN   - Cygwin
 * DARWIN   - Any Darwin system (macOS, iOS, watchOS, tvOS)
 * FREEBSD  - FreeBSD
 * HAIKU    - Haiku
 * HPUX     - HP-UX
 * HURD     - GNU Hurd
 * INTERIX  - Interix
 * IOS      - iOS
 * LINUX    - Linux
 * LYNX     - LynxOS
 * MACOS    - macOS
 * NETBSD   - NetBSD
 * OPENBSD  - OpenBSD
 * QNX      - QNX
 * QNX6     - QNX RTP 6.1
 * SOLARIS  - Sun Solaris
 * TVOS     - tvOS
 * WATCHOS  - watchOS
 * WIN32    - Windows (WIN64 on 64-bit platforms also defined)
 * WINRT    - WinRT
 */

#if defined(__APPLE__)
	#include <TargetConditionals.h>

	#define RB_OS_DARWIN

	#if defined(TARGET_OS_OSX) && TARGET_OS_OSX
		#define RB_OS_MACOS
	#elif defined(TARGET_OS_IOS) && TARGET_OS_IOS
		#define RB_OS_IOS
	#elif defined(TARGET_OS_WATCH) && TARGET_OS_WATCH
		#define RB_OS_WATCHOS
	#elif defined(TARGET_OS_TV) && TARGET_OS_TV
		#define RB_OS_TVOS
	#elif defined(TARGET_OS_MAC) && TARGET_OS_MAC
		#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
			#define RB_OS_IOS
		#else
			#define RB_OS_MACOS
		#endif
	#else
		#error "Unknown Apple platform"
	#endif

	#ifdef __LP64__
		#define RB_OS_DARWIN64
	#else
		#define RB_OS_DARWIN32
	#endif
#elif defined(__ANDROID__) || defined(ANDROID)
	#define RB_OS_ANDROID
	#define RB_OS_LINUX
#elif defined(__CYGWIN__)
	#define RB_OS_CYGWIN
#elif defined(_WIN32) || defined(_WIN64) || defined(__NT__)
	#define RB_OS_WIN32

	#if defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
		#define RB_OS_WIN64
	#endif

	#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP)
		#define RB_OS_WINRT
	#endif
#elif defined(__sun) || defined(sun)
	#define RB_OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
	#define RB_OS_HPUX
#elif defined(__native_client__)
	#define RB_OS_NACL
#elif defined(__EMSCRIPTEN__)
	#define RB_OS_WASM
#elif defined(__linux__) || defined(__linux)
	#define RB_OS_LINUX
#elif defined(__FreeBSD__) || defined(__DragonFly__)
	#define RB_OS_FREEBSD
#elif defined(__NetBSD__)
	#define RB_OS_NETBSD
#elif defined(__OpenBSD__)
	#define RB_OS_OPENBSD
#elif defined(__INTERIX)
	#define RB_OS_INTERIX
#elif defined(_AIX)
	#define RB_OS_AIX
#elif defined(__Lynx__)
	#define RB_OS_LYNX
#elif defined(__GNU__)
	#define RB_OS_HURD
#elif defined(__QNXNTO__)
	#define RB_OS_QNX
#elif defined(__INTEGRITY)
	#define RB_OS_INTEGRITY
#elif defined(__HAIKU__)
	#define RB_OS_HAIKU
#else
	#error "Unsupported OS"
#endif

#if defined(RB_OS_WIN32) || defined(RB_OS_WIN64) || defined(RB_OS_WINRT) \
    || defined(RB_OS_CYGWIN)
	#define RB_OS_WIN
#endif
