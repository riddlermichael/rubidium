#pragma once

#include <rb/core/error/OsError.hpp>
#include <rb/core/os.hpp>
#include <rb/sync.hpp>

#if RB_USE(PTHREADS)
	#include <pthread.h>
#elif RB_USE(WIN32_THREADS)
	#include <Windows.h>
#endif

#define RB_SYNC_CHECK(expr)                                         \
	do {                                                            \
		int const _err = expr;                                      \
		if (_err) {                                                 \
			auto const ec = static_cast<rb::core::ErrorCode>(_err); \
			throw rb::core::OsError(                                \
			    ec,                                                 \
			    _err,                                               \
			    RB_SOURCE_LOCATION);                                \
		}                                                           \
	} while (0)

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
