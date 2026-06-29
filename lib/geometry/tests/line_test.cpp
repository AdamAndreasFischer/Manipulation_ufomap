// UFO
#include <ufo/geometry/distance.hpp>
#include <ufo/geometry/intersects.hpp>
#include <ufo/geometry/line.hpp>

// STL
#include <iostream>

// Catch2
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("[Line 2D]")
{
	ufo::Vec2f a(-1, -1);
	ufo::Vec2f b(1, 1);

	ufo::Line<2> l(a, b);

	std::cout << "Normal: " << l.normal << "\n";
	std::cout << "Normal len: " << norm(l.normal) << "\n";
}

TEST_CASE("[Line 2D] distance")
{
	ufo::Vec2f a(-1, -1);
	ufo::Vec2f b(1, 1);

	ufo::Line<2> l(a, b);

	auto d1 = ufo::distance(l, a);
	REQUIRE(d1 == 0);
	// std::cout << "Distance: " << d1 << "\n";

	auto d2 = ufo::distance(l, ufo::Vec2f(0, 1));
	REQUIRE(d2 == Catch::Approx(std::sqrt(2) / 2.0f));
	// std::cout << "Distance: " << d2 << "\n";
}

TEST_CASE("[Line 2D] intersects")
{
	SECTION("origin")
	{
		ufo::Vec2f   a1(-1, -1);
		ufo::Vec2f   b1(1, 1);
		ufo::Line<2> l1(a1, b1);

		ufo::Vec2f   a2(-1, 1);
		ufo::Vec2f   b2(1, -1);
		ufo::Line<2> l2(a2, b2);

		REQUIRE(ufo::intersects(l1, l2));
		REQUIRE(ufo::intersectionPoint(l1, l2) == ufo::Vec2f(0, 0));
	}

	SECTION("not origin")
	{
		ufo::Vec2f   a1(0, -1);
		ufo::Vec2f   b1(1, 0);
		ufo::Line<2> l1(a1, b1);

		std::cout << "1: normal: " << l1.normal << ", distance: " << l1.distance << "\n";

		ufo::Vec2f   a2(2, 1);
		ufo::Vec2f   b2(3, 0);
		ufo::Line<2> l2(a2, b2);

		std::cout << "2: normal: " << l2.normal << ", distance: " << l2.distance << "\n";

		REQUIRE(ufo::intersects(l1, l2));
		REQUIRE(ufo::intersectionPoint(l1, l2) == ufo::Vec2f(2, 1));
	}
}
