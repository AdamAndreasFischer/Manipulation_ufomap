// UFO
#include <ufo/container/octree_map.hpp>

// Catch2
#include <catch2/catch_test_macros.hpp>

// STL
#include <iostream>

using namespace ufo;

TEST_CASE("Octree Map")
{
	OctreeMap<int> map;
	map.insert({{0, 0, 0}, 5});
	map.emplace({0, 0, 0}, 42);

	std::vector<std::pair<Vec3f, int>> values;
	values.emplace_back(Vec3f(0, 1, 2), 30);
	values.emplace_back(Vec3f(0, 3, 0), 35);

	map.insert(values);

	for (auto [k, v] : map) {
		std::cout << k << ": " << v << std::endl;
	}

	std::cout << "Erasing\n";
	// map.erase(map.query(pred::True()));
	// map.erase(std::as_const(map).query(pred::True()));
	map.erase(map.nearest(Vec3f{0, 2.9, 0}).begin());
	std::cout << "Erased\n";

	// for (auto [k, v] : map.query(pred::True())) {
	// 	std::cout << k << ": " << v << std::endl;
	// }

	std::cout << std::endl;

	for (auto& [k, v] : map.nearest(Vec3f{0, 2.9, 0})) {
		std::cout << k << ": " << v << std::endl;
	}


	std::cout << std::endl;

	for (auto [k, v] :
	     map.queryNearest(pred::Intersects(BS3({0, 1, 1.8}, 0.25)), Vec3f{0, 2.9, 0})) {
		std::cout << k << ": " << v << std::endl;
	}

	for (auto [k, v] : map.nearest(Vec3f{0, 2.9, 0})) {
		std::cout << k << ": " << v << std::endl;
	}

	// struct S {
	// 	int   a;
	// 	bool  b;
	// 	float c;

	// 	S(int a, bool b, float c) : a(a), b(b), c(c) {}
	// };

	// OctreeMap<S> map2;
	// map2.emplace({33, 44, 55}, 100, true, 3.14f);
}