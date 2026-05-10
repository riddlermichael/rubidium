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

namespace {

constexpr f64 eps = std::numeric_limits<f64>::epsilon();

void epsilon() noexcept {
	Interval64 const sum = Interval64{1} + eps / 2;
	RB_ASSERT(sum == Interval64{1, 1 + eps});
}

void dirty() {
	constexpr Interval64 x = 1 + eps;
	Interval64 const y = x * x;
	RB_ASSERT(y.lo() == 1 + 2 * eps);
	RB_ASSERT(y.hi() == 1 + 3 * eps);
}

void cancellation() {
	constexpr Interval64 lhs = {1.0, 1.0 + eps};
	Interval64 const diff = lhs - lhs;
	RB_ASSERT(-diff.lo() == diff.hi());
	RB_ASSERT(diff.radius() == eps);
}

void edges() {
	constexpr Interval64 m = max<f64>;
	Interval64 const sum = m + m;
	RB_ASSERT(sum == Interval64{max<f64>, inf<f64>});
}

} // namespace

int main() {
	constexpr Interval64 x = {-2, 2};
	Interval64 const y = x * x;
	cout << x << endl;
	cout << y << endl;
}
