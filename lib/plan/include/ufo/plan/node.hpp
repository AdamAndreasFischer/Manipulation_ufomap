#ifndef UFO_PLAN_NODE_HPP
#define UFO_PLAN_NODE_HPP

// UFO
#include <ufo/math/vec.hpp>
#include <ufo/plan/edge.hpp>

// STL
#include <cstddef>
#include <cstdint>

namespace ufo
{
template <std::size_t Dim, class T>
struct PlanNode : public Vec<Dim, T> {
	T             cost{};
	std::uint32_t first_edge{};
	std::uint32_t last_edge{};

	PlanNode() = default;

	PlanNode(Vec<Dim, T> const& position, T const& cost) : Vec<Dim, T>(position), cost(cost)
	{
	}
};
}  // namespace ufo

#endif  // UFO_PLAN_NODE_HPP