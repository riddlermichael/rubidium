#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <rb/core/SourceLocation.hpp>

using Catch::Matchers::EndsWith;
using Catch::Matchers::Equals;

TEST_CASE("Current", "[core::SourceLocation]") {
	auto const line = __LINE__ + 1;
	auto const loc = RB_SOURCE_LOCATION;
	REQUIRE_THAT(loc.file(), EndsWith("SourceLocation.cpp"));
	REQUIRE(loc.line() == line);
	REQUIRE_THAT(loc.func(), Equals("CATCH2_INTERNAL_TEST_0")); // debatable
}
