// Catch2
#include <catch2/catch_test_macros.hpp>

// UFO
#include <ufo/plan/graph.hpp>

// STL
#include <iostream>

using namespace ufo;

TEST_CASE("Plan")
{
	SECTION("Something")
	{
		PlanGraph<2> plan;
		auto         n_1 = plan.addNode(Vec2f(0, 0), 0);
		auto         n_2 = plan.addNode(Vec2f(1, 0), 0);
		auto         n_3 = plan.addNode(Vec2f(0, 1), 0);
		auto         n_4 = plan.addNode(Vec2f(2, 0), 0);

		plan.addEdge(n_1, n_2, 0);
		plan.addEdge(n_1, n_3, 0);
		plan.addEdge(n_2, n_4, 0);
		plan.addEdge(n_3, n_4, 0);

		auto path = plan.plan(n_1, n_4);

		if (path.nodes.empty()) {
			std::cout << "No path found\n";
		} else {
			std::cout << "Path: (";
			for (std::size_t i{}; path.nodes.size() - 1 > i; ++i) {
				std::cout << plan.coord(path.nodes[i]) << ") -> (";
			}
			std::cout << plan.coord(path.nodes.back()) << ")\n";
			std::cout << "Cost: " << path.cost << '\n';
			std::cout << "Distance: " << path.distance << '\n';
		}
	}
}