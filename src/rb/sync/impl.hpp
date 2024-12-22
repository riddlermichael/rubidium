#pragma once

#include <rb/core/error/OsError.hpp>
#include <rb/core/helpers.hpp>
#include <rb/core/os.hpp>
#include <rb/sync.hpp>

#if RB_USE(PTHREADS)
	#include <pthread.h>
#elif RB_USE(WIN32_THREADS)
	#include <Windows.h>
#endif

/// Check expression as an errno value.
#define RB_SYNC_CHECK_ERRNO(expr)          \
	do {                                   \
		int const _err = expr;             \
		if (_err) {                        \
			throw rb::core::OsError(       \
			    rb::core::fromErrno(_err), \
			    _err,                      \
			    RB_SOURCE_LOCATION);       \
		}                                  \
	} while (0)

/// Check expression as a result of a system function call.
#ifdef RB_OS_WIN
	#define RB_SYNC_CHECK_LAST_ERROR(expr)                                \
		do {                                                              \
			DWORD const _err = expr;                                      \
			if (!_err) {                                                  \
				throw rb::core::OsError::lastOsError(RB_SOURCE_LOCATION); \
			}                                                             \
		} while (0)
#else
	#define RB_SYNC_CHECK_LAST_ERROR(expr)                                \
		do {                                                              \
			int const _err = expr;                                        \
			if (_err) {                                                   \
				throw rb::core::OsError::lastOsError(RB_SOURCE_LOCATION); \
			};                                                            \
		} while (0)
#endif

#define RB_SYNC_IMPL &(*pImpl_).impl
