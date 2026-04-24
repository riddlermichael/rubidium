#pragma once

#include <rb/core/os.hpp>

#ifdef RB_OS_WIN

	#include <rb/core/warnings.hpp>

RB_WARNING_PUSH
RB_WARNING_POTENTIALLY_THROWING_FUNCTION
	#include <Windows.h>
RB_WARNING_POP

#endif
