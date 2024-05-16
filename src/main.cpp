#include <iostream>

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

int main() {
	cout << CompilerInfo::kThis << endl;
	return 0;
}
