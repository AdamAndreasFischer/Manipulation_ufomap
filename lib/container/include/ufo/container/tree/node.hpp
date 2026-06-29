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

#ifndef UFO_CONTAINER_TREE_NODE_HPP
#define UFO_CONTAINER_TREE_NODE_HPP

// UFO
#include <ufo/container/tree/code.hpp>
#include <ufo/container/tree/index.hpp>

// STL
#include <cstddef>
#include <functional>
#include <ostream>

namespace ufo
{
template <std::size_t Dim>
struct TreeNode {
	TreeCode<Dim> code;
	TreeIndex     index;

	constexpr TreeNode() = default;

	constexpr TreeNode(TreeCode<Dim> const& code, TreeIndex const& index) noexcept
	    : code(code), index(index)
	{
	}

	operator TreeCode<Dim>() const noexcept { return code; }

	explicit operator TreeIndex() const noexcept { return index; }
};

//
// Deduction guide
//

template <std::size_t Dim>
TreeNode(TreeCode<Dim>, TreeIndex) -> TreeNode<Dim>;

//
// Compare
//

template <std::size_t Dim>
constexpr bool operator==(TreeNode<Dim> const& lhs, TreeNode<Dim> const& rhs) noexcept
{
	return lhs.code == rhs.code;
}

template <std::size_t Dim>
constexpr bool operator!=(TreeNode<Dim> const& lhs, TreeNode<Dim> const& rhs) noexcept
{
	return !(lhs == rhs);
}

template <std::size_t Dim>
constexpr bool operator<(TreeNode<Dim> const& lhs, TreeNode<Dim> const& rhs) noexcept
{
	return lhs.code < rhs.code;
}

template <std::size_t Dim>
constexpr bool operator<=(TreeNode<Dim> const& lhs, TreeNode<Dim> const& rhs) noexcept
{
	return lhs.code <= rhs.code;
}

template <std::size_t Dim>
constexpr bool operator>(TreeNode<Dim> const& lhs, TreeNode<Dim> const& rhs) noexcept
{
	return lhs.code > rhs.code;
}

template <std::size_t Dim>
constexpr bool operator>=(TreeNode<Dim> const& lhs, TreeNode<Dim> const& rhs) noexcept
{
	return lhs.code >= rhs.code;
}

template <std::size_t Dim>
std::ostream& operator<<(std::ostream& os, TreeNode<Dim> const& node)
{
	return os << "Code: (" << node.code << "), Index: (" << node.index << ')';
}

using BinaryNode = TreeNode<1>;
using QuadNode   = TreeNode<2>;
using OctNode    = TreeNode<3>;
using HexNode    = TreeNode<4>;

}  // namespace ufo

namespace std
{
template <std::size_t Dim>
struct hash<ufo::TreeNode<Dim>> {
	std::size_t operator()(ufo::TreeNode<Dim> const& node) const
	{
		return hash<ufo::TreeCode<Dim>>()(node.code);
	}
};
}  // namespace std

#endif  // UFO_CONTAINER_TREE_NODE_HPP