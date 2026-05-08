#include <iostream>

// ReSharper disable CppUnusedIncludeDirective
#include <rb/containers/package.hpp>
#include <rb/core/package.hpp>
#include <rb/ext/package.hpp>
#include <rb/fmt/package.hpp>
#include <rb/ranges/package.hpp>
#include <rb/sync/package.hpp>
#include <rb/time/package.hpp>

#include <rb/interval/Interval64.hpp>
#include <rb/math/float.hpp>

using std::cout;
using std::endl;

using namespace rb;
using namespace rb::core;
using namespace rb::interval;

int main() {
	auto const x = Interval64::fromCenter(4. / 3, core::inf<f64>);
	cout << x << endl;
}
