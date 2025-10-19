#include <iostream>

// ReSharper disable CppUnusedIncludeDirective
#include <rb/containers/package.hpp>
#include <rb/core/package.hpp>
#include <rb/ext/package.hpp>
#include <rb/fmt/package.hpp>
#include <rb/ranges/package.hpp>
#include <rb/sync/package.hpp>
#include <rb/time/package.hpp>

using std::cout;
using std::endl;

using namespace rb;
using namespace rb::core;

#ifndef RB_GL_FRAMEBUFFER_SRG
	#define RB_GL_FRAMEBUFFER_SRGB 0x8DB9
#endif

int main() {
	int x = 42;
	cout << CompilerInfo::kThis << endl;
}
