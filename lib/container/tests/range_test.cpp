// UFO
#include <ufo/container/range.hpp>

// Catch2
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Range")
{
	ufo::Range<float> r(0.0f, 1.0f);
	REQUIRE(r.contains(0.1f));
}