/*!
 * UFOMap: An Efficient Probabilistic 3D Mapping Framework That Embraces the Unknown
 *
 * @author Daniel Duberg (dduberg@kth.se)
 * @see https://github.com/UnknownFreeOccupied/ufomap
 * @version 1.0
 * @date 2022-05-13
 *
 * @copyright Copyright (c) 2022, Daniel Duberg, KTH Royal Institute of Technology
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Daniel Duberg, KTH Royal Institute of Technology
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UFO_CONTAINER_TREE_DISTANCE_NODE_HPP
#define UFO_CONTAINER_TREE_DISTANCE_NODE_HPP

// UFO
#include <ufo/container/tree/node.hpp>

// STL
#include <cstddef>
#include <functional>

namespace ufo
{
template <std::size_t Dim>
struct TreeDistanceNode : public TreeNode<Dim> {
	float distance{};

	constexpr TreeDistanceNode() = default;

	constexpr TreeDistanceNode(TreeNode<Dim> const& node, float distance = 0.0f)
	    : TreeNode<Dim>(node), distance(distance)
	{
	}

	constexpr TreeDistanceNode(TreeCode<Dim> const& code, TreeIndex const& index,
	                           float distance = 0.0f) noexcept
	    : TreeNode<Dim>(code, index), distance(distance)
	{
	}
};

//
// Deduction guide
//

template <std::size_t Dim>
TreeDistanceNode(TreeNode<Dim>) -> TreeDistanceNode<Dim>;

template <std::size_t Dim>
TreeDistanceNode(TreeNode<Dim>, float) -> TreeDistanceNode<Dim>;

template <std::size_t Dim>
TreeDistanceNode(TreeCode<Dim>, TreeIndex) -> TreeDistanceNode<Dim>;

template <std::size_t Dim>
TreeDistanceNode(TreeCode<Dim>, TreeIndex, float) -> TreeDistanceNode<Dim>;

template <std::size_t Dim>
constexpr bool operator==(TreeDistanceNode<Dim> const& lhs,
                          TreeDistanceNode<Dim> const& rhs)
{
	return lhs.distance == rhs.distance &&
	       static_cast<TreeNode<Dim> const&>(lhs) == static_cast<TreeNode<Dim> const&>(rhs);
}

template <std::size_t Dim>
constexpr bool operator!=(TreeDistanceNode<Dim> const& lhs,
                          TreeDistanceNode<Dim> const& rhs)
{
	return !(lhs == rhs);
}

template <std::size_t Dim>
constexpr bool operator<(TreeDistanceNode<Dim> const& lhs,
                         TreeDistanceNode<Dim> const& rhs)
{
	return lhs.distance < rhs.distance;
}

template <std::size_t Dim>
constexpr bool operator<=(TreeDistanceNode<Dim> const& lhs,
                          TreeDistanceNode<Dim> const& rhs)
{
	return lhs.distance <= rhs.distance;
}

template <std::size_t Dim>
constexpr bool operator>(TreeDistanceNode<Dim> const& lhs,
                         TreeDistanceNode<Dim> const& rhs)
{
	return lhs.distance > rhs.distance;
}

template <std::size_t Dim>
constexpr bool operator>=(TreeDistanceNode<Dim> const& lhs,
                          TreeDistanceNode<Dim> const& rhs)
{
	return lhs.distance >= rhs.distance;
}

template <std::size_t Dim>
std::ostream& operator<<(std::ostream& os, TreeDistanceNode<Dim> const& node)
{
	return os << "Node: (" << static_cast<TreeNode<Dim> const&>(node)
	          << "), Distance: " << node.distance;
}

using BinaryDistanceNode = TreeDistanceNode<1>;
using QuadDistanceNode   = TreeDistanceNode<2>;
using OctDistanceNode    = TreeDistanceNode<3>;
using HexDistanceNode    = TreeDistanceNode<4>;
}  // namespace ufo

template <std::size_t Dim>
struct std::hash<ufo::TreeDistanceNode<Dim>> {
	std::size_t operator()(ufo::TreeDistanceNode<Dim> const& node) const
	{
		return hash(static_cast<ufo::TreeNode<Dim> const&>(node));
	}
};

#endif  // UFO_CONTAINER_TREE_DISTANCE_NODE_HPP