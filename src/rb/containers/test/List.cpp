#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <rb/containers/List.hpp>

using namespace rb::core;
using namespace rb::containers;

TEMPLATE_TEST_CASE("ctor.1", "[containers::List]", int) {
	List<TestType> const list;
	REQUIRE(list.empty());
	REQUIRE(list.size() == 0); // NOLINT(*-container-size-empty)
	REQUIRE_THROWS_AS(list.front(), AssertError);
	REQUIRE_THROWS_AS(list.back(), AssertError);
}
