#ifndef UFO_PLAN_EDGE_HPP
#define UFO_PLAN_EDGE_HPP

// UFO
#include <ufo/math/vec.hpp>

// STL
#include <cstddef>
#include <cstdint>

namespace ufo
{
template <std::size_t Dim, class T>
struct PlanEdge {
	T cost{};
	T distance{};

	std::uint32_t start;
	std::uint32_t end;

	PlanEdge() = default;

	PlanEdge(unsigned from, unsigned to, T const& cost, T const& distance)
	    : from_(from), to_(to), cost(cost), distance(distance)
	{
	}

 private:
	unsigned from_;
	unsigned to_;
};
}  // namespace ufo

#endif  // UFO_PLAN_EDGE_HPP