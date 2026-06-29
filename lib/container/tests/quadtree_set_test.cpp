// UFO
#include <ufo/container/quadtree_set.hpp>
#include <ufo/geometry/fun.hpp>

// Catch2
#include <catch2/catch_test_macros.hpp>

// STL
#include <iostream>

TEST_CASE("Quadtree Set")
{
	ufo::QuadtreeSet set;

	auto b = set.bounds();

	for (std::size_t i{}; 2 > i; ++i) {
		std::cout << min(b)[i] << " and " << max(b)[i] << std::endl;
		REQUIRE(min(b)[i] > max(b)[i]);
	}

	set.insert({0, 0});
	set.insert({{100, -234}, {-102.3, 200}});
	set.insert(ufo::Vec2f(1, 0));

	b = set.bounds();

	REQUIRE(all(ufo::Vec2f(-102.3, -234) == min(b)));
	REQUIRE(all(ufo::Vec2f(100, 200) == max(b)));

	std::cout << "Before" << std::endl;
	for (auto x : set) {
		std::cout << x << std::endl;
	}

	auto it = set.cbegin();
	set.erase(it);

	b = set.bounds();

	REQUIRE(all(ufo::Vec2f(-102.3, -234) == min(b)));
	REQUIRE(all(ufo::Vec2f(100, 200) == max(b)));

	std::cout << "After" << std::endl;
	for (auto x : set) {
		std::cout << x << std::endl;
	}

	set.erase(std::next(set.begin()), set.end());

	b = set.bounds();

	REQUIRE(all(ufo::Vec2f(100, -234) == min(b)));
	REQUIRE(all(ufo::Vec2f(100, -234) == max(b)));

	std::cout << "After 2" << std::endl;
	for (auto x : set) {
		std::cout << x << std::endl;
	}

	set.clear();

	b = set.bounds();

	for (std::size_t i{}; 2 > i; ++i) {
		REQUIRE(min(b)[i] > max(b)[i]);
	}

	std::cout << "After 3" << std::endl;
	for (auto x : set) {
		std::cout << x << std::endl;
	}

	set.insert({0, 0});
	set.insert({{100, -234}, {-102.3, 200}});
	set.insert(ufo::Vec2f(1, 0));

	std::cout << "After 4" << std::endl;
	for (auto x : set) {
		std::cout << x << std::endl;
	}

	std::cout << "After 5" << std::endl;
	for (auto [p, d] : set.nearest({0, 0})) {
		std::cout << p << " with distance: " << d << std::endl;
	}

	auto nearest_it = set.beginNearest({0, 0});

	set.erase(nearest_it, std::next(nearest_it, 2));

	std::cout << "After 6" << std::endl;
	for (auto x : set) {
		std::cout << x << std::endl;
	}

	std::cout << "Query iterator" << std::endl;
	for (auto x : set.query(ufo::pred::Satisfies([](auto e) { return 100 != e.x; }))) {
		std::cout << x << std::endl;
	}
}