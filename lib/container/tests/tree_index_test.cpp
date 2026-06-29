// UFO
#include <ufo/container/tree/index.hpp>

// Catch2
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Tree Index")
{
	ufo::TreeIndex index;

	REQUIRE(ufo::TreeIndex::NULL_POS == index.pos);
	REQUIRE(!index.valid());
}