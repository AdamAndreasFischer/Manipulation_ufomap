// UFO
#include <ufo/geometry/frustum.hpp>
#include <ufo/geometry/fun.hpp>
#include <ufo/math/vec.hpp>

// STL
#include <array>
#include <iostream>

// Catch2
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "ufo/geometry/contains.hpp"

TEST_CASE("[Frustum] Constructor")
{
	SECTION("Corners")
	{
		ufo::Vec2f a(2, 2);
		ufo::Vec2f b(-2, 2);
		ufo::Vec2f c(-1, 1);
		ufo::Vec2f d(1, 1);

		ufo::Frustum<2> f(a, b, c, d);

		auto cs = ufo::corners(f);
		// std::cout << "corners:\n";
		// for (auto co : cs) {
		// 	std::cout << co << "\n";
		// }

		REQUIRE(cs[0].x == Catch::Approx(a.x));
		REQUIRE(cs[0].y == Catch::Approx(a.y));

		REQUIRE(cs[1].x == Catch::Approx(b.x));
		REQUIRE(cs[1].y == Catch::Approx(b.y));

		REQUIRE(cs[2].x == Catch::Approx(c.x));
		REQUIRE(cs[2].y == Catch::Approx(c.y));

		REQUIRE(cs[3].x == Catch::Approx(d.x));
		REQUIRE(cs[3].y == Catch::Approx(d.y));

		// std::cout << "contains " << ufo::contains(f, ufo::AABB2f(ufo::Vec2f(0, 1.5),
		// 0.1f)); std::cout << "contains " << ufo::contains(f, ufo::BS2(ufo::Vec2f(0, 1.5),
		// 0.1f));
	}

	SECTION("min/max")
	{
		ufo::Vec2f a(2, 2);
		ufo::Vec2f b(-2, 2);
		ufo::Vec2f c(-1, 1);
		ufo::Vec2f d(1, 1);

		ufo::Frustum<2> f(a, b, c, d);

		auto min_result = ufo::min(f);
		auto max_result = ufo::max(f);

		// std::cout << "Min: " << ufo::min(f) << "\n";
		// std::cout << "Max: " << ufo::max(f) << "\n";

		ufo::Vec2f min_expected(-2, 1);
		ufo::Vec2f max_expected(2, 2);

		REQUIRE(min_result.x == Catch::Approx(min_expected.x));
		REQUIRE(min_result.y == Catch::Approx(min_expected.y));

		REQUIRE(max_result.x == Catch::Approx(max_expected.x));
		REQUIRE(max_result.y == Catch::Approx(max_expected.y));
	}

	SECTION("Position and target from origin")
	{
		ufo::Vec2f eye(0, 0);
		ufo::Vec2f target(0, 2);
		float      fov  = ufo::radians(90.0f);
		float      near = 1.0f;
		float      far  = 3.0f;

		ufo::Frustum<2> f(eye, target, fov, near, far);

		auto                      cs = ufo::corners(f);
		std::array<ufo::Vec2f, 4> expected{ufo::Vec2f(3, 3), ufo::Vec2f(-3, 3),
		                                   ufo::Vec2f(-1, 1), ufo::Vec2f(1, 1)};
		// std::cout << "corners:\n";
		// for (auto co : cs) {
		// 	std::cout << co << "\n";
		// }

		// x: 3 y: 3
		// x: -3 y: 3
		// x: -1 y: 1
		// x: 1 y: 1

		REQUIRE(cs[0].x == Catch::Approx(expected[0].x));
		REQUIRE(cs[0].y == Catch::Approx(expected[0].y));
		REQUIRE(cs[1].x == Catch::Approx(expected[1].x));
		REQUIRE(cs[1].y == Catch::Approx(expected[1].y));
		REQUIRE(cs[2].x == Catch::Approx(expected[2].x));
		REQUIRE(cs[2].y == Catch::Approx(expected[2].y));
		REQUIRE(cs[3].x == Catch::Approx(expected[3].x));
		REQUIRE(cs[3].y == Catch::Approx(expected[3].y));
	}

	SECTION("Position and target rotated")
	{
		ufo::Vec2f eye(0.5, 0.5);
		ufo::Vec2f target(1, 1);
		float      fov  = ufo::radians(90.0f);
		float      near = 1.0f;
		float      far  = 3.0f;

		ufo::Frustum<2> f(eye, target, fov, near, far);

		auto cs = ufo::corners(f);

		std::array<ufo::Vec2f, 4> expected{
		    ufo::Vec2f(4.74264f, 0.5f), ufo::Vec2f(0.5f, 4.74264f),
		    ufo::Vec2f(0.5f, 1.91421f), ufo::Vec2f(1.91421f, 0.5f)};

		// std::cout << "corners:\n";
		// for (auto co : cs) {
		// 	std::cout << co << "\n";
		// }

		//  x: 4.74264 y: 0.5
		//  x: 0.5 y: 4.74264
		//  x: 0.5 y: 1.91421
		//  x: 1.91421 y: 0.5

		REQUIRE(cs[0].x == Catch::Approx(expected[0].x));
		REQUIRE(cs[0].y == Catch::Approx(expected[0].y));
		REQUIRE(cs[1].x == Catch::Approx(expected[1].x));
		REQUIRE(cs[1].y == Catch::Approx(expected[1].y));
		REQUIRE(cs[2].x == Catch::Approx(expected[2].x));
		REQUIRE(cs[2].y == Catch::Approx(expected[2].y));
		REQUIRE(cs[3].x == Catch::Approx(expected[3].x));
		REQUIRE(cs[3].y == Catch::Approx(expected[3].y));
	}
}