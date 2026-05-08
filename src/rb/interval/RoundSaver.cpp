#include "RoundSaver.hpp"

#include <cfenv>

#include <rb/core/compiler.hpp>

#ifndef RB_COMPILER_MSVC
	#pragma STDC FENV_ACCESS ON
#endif

using namespace rb::interval;

RoundSaver::RoundSaver() noexcept
    : roundMode{std::fegetround()} {
}

RoundSaver::~RoundSaver() {
	std::fesetround(roundMode);
}
