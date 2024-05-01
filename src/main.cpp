#include <iostream>

#include <rb/core/package.hpp>
#include <rb/ext/package.hpp>
#include <rb/fmt/package.hpp>
#include <rb/sync/package.hpp>
#include <rb/time/package.hpp>

using std::cout;
using std::endl;

using namespace rb;
using namespace rb::core;

int main() {
	using namespace rb::time::literals;

	auto const dur = 0_min;
	cout << dur.isPositive() << endl;
	cout << dur.isNaN() << endl;
	cout << (dur / 0).isNaN() << endl;
	return 0;
}
