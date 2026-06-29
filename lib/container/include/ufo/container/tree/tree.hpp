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

#ifndef UFO_CONTAINER_TREE_HPP
#define UFO_CONTAINER_TREE_HPP

// UFO
#include <ufo/container/tree/code.hpp>
#include <ufo/container/tree/container.hpp>
#include <ufo/container/tree/coord.hpp>
#include <ufo/container/tree/data.hpp>
#include <ufo/container/tree/distance_node.hpp>
#include <ufo/container/tree/index.hpp>
#include <ufo/container/tree/iterator.hpp>
#include <ufo/container/tree/key.hpp>
#include <ufo/container/tree/nearest_iterator.hpp>
#include <ufo/container/tree/node.hpp>
#include <ufo/container/tree/predicate.hpp>
#include <ufo/container/tree/query_iterator.hpp>
#include <ufo/container/tree/query_nearest_iterator.hpp>
#include <ufo/container/tree/trace_result.hpp>
#include <ufo/execution/execution.hpp>
#include <ufo/geometry/aabb.hpp>
#include <ufo/geometry/ray.hpp>
#include <ufo/math/math.hpp>
#include <ufo/math/vec.hpp>
#include <ufo/utility/bit_set.hpp>
#include <ufo/utility/io/buffer.hpp>
#include <ufo/utility/iterator_wrapper.hpp>
#include <ufo/utility/macros.hpp>
#include <ufo/utility/type_traits.hpp>

// STL
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <deque>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace ufo
{
enum class NearestSearchAlgorithm { DEPTH_FIRST, A_STAR };

/*!
 * @brief
 *
 * Utilizing curiously recurring template pattern (CRTP)
 *
 * \tparam Derived ...
 * \tparam Dim ...
 * \tparam Ts ...
 */
template <class Derived, std::size_t Dim, class Block, class... Blocks>
class Tree : public TreeData<Derived, Block, Blocks...>
{
 protected:
	//
	// Friends
	//

	template <class, std::size_t, class, class...>
	friend class Tree;

	static constexpr std::size_t const BF = ipow(std::size_t(2), Dim);

	using Data = TreeData<Derived, Block, Blocks...>;

 public:
	//
	// Tags
	//

	using length_t = double;
	using depth_t  = unsigned;
	using coord_t  = float;
	using ray_t    = coord_t;

	using Code   = TreeCode<Dim>;
	using Key    = TreeKey<Dim>;
	using Coord  = TreeCoord<Dim, coord_t>;
	using Coord2 = TreeCoord<Dim, double>;
	using Point  = Vec<Dim, coord_t>;
	using Point2 = Vec<Dim, double>;
	using Bounds = AABB<Dim, coord_t>;
	using Length = Vec<Dim, length_t>;

	using Index        = TreeIndex;
	using Node         = TreeNode<Dim>;
	using DistanceNode = TreeDistanceNode<Dim>;

	using pos_t    = typename TreeIndex::pos_t;
	using offset_t = typename TreeIndex::offset_t;
	using key_t    = typename Key::key_t;
	using code_t   = typename Code::code_t;

	// Iterators

	using const_iterator = TreeIterator<Derived>;

	template <class Predicate>
	using const_query_iterator_pred = TreeQueryIterator<Derived, Predicate>;
	using const_query_iterator      = TreeQueryIterator<Derived>;

	template <class Geometry>
	using const_nearest_iterator_geom = TreeNearestIterator<Derived, Geometry>;
	using const_nearest_iterator      = TreeNearestIterator<Derived>;

	template <class Predicate, class Geometry>
	using const_query_nearest_iterator_pred_geom =
	    TreeQueryNearestIterator<Derived, Predicate, Geometry>;
	using const_query_nearest_iterator = TreeQueryNearestIterator<Derived>;

	template <class Predicate>
	using ConstQuery =
	    IteratorWrapper<const_query_iterator_pred<Predicate>, const_query_iterator>;

	template <class Geometry>
	using ConstNearest =
	    IteratorWrapper<const_nearest_iterator_geom<Geometry>, const_nearest_iterator>;

	template <class Predicate, class Geometry>
	using ConstQueryNearest =
	    IteratorWrapper<const_query_nearest_iterator_pred_geom<Predicate, Geometry>,
	                    const_query_nearest_iterator>;

	template <class T>
	struct is_node_type
	    : contains_convertible_type<remove_cvref_t<T>, Index, Node, Code, Key, Coord, Point,
	                                // We also add the double versions of Coord and Point
	                                Coord2, Point2> {
	};

	template <class T>
	static constexpr inline bool is_node_type_v = is_node_type<T>::value;

 public:
	/**************************************************************************************
	|                                                                                     |
	|                                        Tree                                         |
	|                                                                                     |
	**************************************************************************************/

	/*!
	 * @brief Returns the branching factor of the tree (i.e., 2 = binary tree, 4 = quadtree,
	 * 8 = octree, 16 = hextree).
	 *
	 * @return The branching factor of the tree.
	 */
	[[nodiscard]] static constexpr std::size_t branchingFactor() noexcept { return BF; }

	/*!
	 * @brief Returns the number of dimensions of the tree (i.e., 1 = binary tree, 2 =
	 * quadtree, 3 = octree, 4 = hextree).
	 *
	 * @return The number of dimensions of the tree.
	 */
	[[nodiscard]] static constexpr std::size_t dimensions() noexcept { return Dim; }

	/*!
	 * @brief Returns the number of nodes in the tree.
	 *
	 * @return The number of nodes in the tree.
	 */
	[[nodiscard]] std::size_t size() const { return Data::size() * BF - (BF - 1); }

	/*!
	 * @brief Increase the capacity of the tree to at least hold `num_nodes` nodes.
	 *
	 * @param num_nodes The new capacity.
	 */
	void reserve(std::size_t num_nodes) { Data::reserve(num_nodes / BF); }

	/*!
	 * @brief Erases all nodes from the tree.
	 */
	void clear()
	{
		Data::clear();
		createRoot();
		derived().onInitRoot();
	}

	void clear(Length const& leaf_node_length, depth_t num_depth_levels)
	{
		init(leaf_node_length, num_depth_levels);
		clear();
	}

	//
	// Depth
	//

	/*!
	 * @brief Returns the number of depth levels of the tree, i.e. `depth() + 1`.
	 *
	 * @return The number of depth levels of the tree.
	 */
	[[nodiscard]] constexpr depth_t numDepthLevels() const noexcept
	{
		return num_depth_levels_;
	}

	/*!
	 * @brief Returns the minimum number of depth levels a tree must have.
	 *
	 * @return The minimum number of depth levels a tree must have.
	 */
	[[nodiscard]] static constexpr depth_t minNumDepthLevels() noexcept { return 2; }

	/*!
	 * @brief Returns the maximum number of depth levels a tree can have.
	 *
	 * @return The maximum number of depth levels a tree can have.
	 */
	[[nodiscard]] static constexpr depth_t maxNumDepthLevels() noexcept
	{
		return Code::maxDepth() + 1;
	}

	/*!
	 * @brief Returns the depth of the root node, i.e. `numDepthLevels() - 1`.
	 *
	 * @note The tree's depth levels are `[0..depth()]`.
	 *
	 * @return The depth of the root node.
	 */
	[[nodiscard]] depth_t depth() const { return numDepthLevels() - 1; }

	/*!
	 * @brief Returns the depth of the block.
	 *
	 * @note The tree's depth levels are `[0..depth()]`.
	 *
	 * @param block the block
	 * @return The depth of the block.
	 */
	[[nodiscard]] depth_t depth(pos_t block) const
	{
		assert(valid(block));

		return treeBlock(block).depth();
	}

	/*!
	 * @brief Returns the depth of the node.
	 *
	 * @note The tree's depth levels are `[0..depth()]`.
	 *
	 * @param node the node
	 * @return The depth of the node.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] constexpr depth_t depth(NodeType node) const
	{
		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			return depth(node.pos);
		} else if constexpr (std::is_same_v<T, Node>) {
			return depth(node.code);
		} else if constexpr (std::is_same_v<T, Code>) {
			return node.depth();
		} else if constexpr (std::is_same_v<T, Key>) {
			return node.depth();
		} else if constexpr (contains_type_v<T, Coord, Coord2>) {
			return node.depth;
		} else if constexpr (contains_type_v<T, Point, Point2>) {
			return 0;
		} else {
			static_assert(is_node_type_v<NodeType>, "Not one of the node types");
		}
	}

	//
	// Length
	//

	/*!
	 * @brief Returns the length of the tree (/ root node), i.e. `leaf_node_length *
	 * 2^depth()`.
	 *
	 * @return The length of the tree (/ root node).
	 */
	[[nodiscard]] Length length() const { return length(depth()); }

	/*!
	 * @brief Returns the length of nodes at `depth`, i.e. `leaf_node_length *
	 * 2^depth`.
	 *
	 * @note The tree's depth levels are `[0..depth()]`.
	 *
	 * @param depth the depth
	 * @return The length of nodes at `depth`.
	 */
	[[nodiscard]] Length length(depth_t depth) const
	{
		assert(numDepthLevels() > depth);
		return node_half_length_[depth + 1];
	}

	/*!
	 * @brief Returns the length of `node`, i.e. `leaf_node_length * 2^depth(node)`.
	 *
	 * @param node the node
	 * @return The length of the node.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] Length length(NodeType node) const
	{
		return length(depth(node));
	}

	/*!
	 * @brief Returns the half length of the tree (/ root node), i.e. `length() / 2`.
	 *
	 * @note The half length is often used, therefore this function exists for improved
	 * performance and precision.
	 *
	 * @return The half length of the tree (/ root node).
	 */
	[[nodiscard]] Length halfLength() const { return halfLength(depth()); }

	/*!
	 * @brief Returns the half length of nodes at `depth`, i.e. `length(depth) / 2`.
	 *
	 * @note The half length is often used, therefore this function exists for improved
	 * performance and precision.
	 *
	 * @note The tree's depth levels are `[0..depth()]`.
	 *
	 * @param depth the depth
	 * @return The half length of nodes at `depth`.
	 */
	[[nodiscard]] Length halfLength(depth_t depth) const
	{
		assert(numDepthLevels() > depth);
		return node_half_length_[depth];
	}

	/*!
	 * @brief Returns the half length of `node`, i.e. `length(node) / 2`.
	 *
	 * @note The half length is often used, therefore this function exists for improved
	 * performance and precision.
	 *
	 * @param node the node
	 * @return The half length of the node.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] constexpr Length halfLength(NodeType node) const
	{
		return halfLength(depth(node));
	}

	/*!
	 * @brief Returns the reciprocal of the length of the tree (/ root node), i.e. `1 /
	 * length()`.
	 *
	 * @note The reciprocal of the length is often used, therefore this function exists for
	 * improved performance and precision.
	 *
	 * @return The reciprocal of the length of the tree (/ root node).
	 */
	[[nodiscard]] Length lengthReciprocal() const { return lengthReciprocal(depth()); }

	/*!
	 * @brief Returns the reciprocal of the length of nodes at `depth`, i.e. `1 /
	 * length(depth)`.
	 *
	 * @note The reciprocal of the length is often used, therefore this function exists for
	 * improved performance and precision.
	 *
	 * @note The tree's depth levels are `[0..depth()]`.
	 *
	 * @param depth the depth
	 * @return The reciprocal of the length of nodes at `depth`.
	 */
	[[nodiscard]] Length lengthReciprocal(depth_t depth) const
	{
		assert(numDepthLevels() > depth + 1);
		return node_half_length_reciprocal_[depth + 1];
	}

	/*!
	 * @brief Returns the reciprocal of the length of `node`, i.e. `1 / length(node)`.
	 *
	 * @note The reciprocal of the length is often used, therefore this function exists for
	 * improved performance and precision.
	 *
	 * @param node the node
	 * @return The reciprocal of the length of the node.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] Length lengthReciprocal(NodeType node) const
	{
		return lengthReciprocal(depth(node));
	}

	/*!
	 * @brief Returns the reciprocal of the half length of the tree (/ root node), i.e. `1 /
	 * (length() / 2) = 2 / length()`.
	 *
	 * @note The reciprocal of the half length is often used, therefore this function exists
	 * for improved performance and precision.
	 *
	 * @return The reciprocal of the half length of the tree (/ root node).
	 */
	[[nodiscard]] Length halfLengthReciprocal() const
	{
		return halfLengthReciprocal(depth());
	}

	/*!
	 * @brief Returns the reciprocal of the half length of nodes at `depth`, i.e. `1 /
	 * (length(depth) / 2) = 2 / length(depth)`.
	 *
	 * @note The reciprocal of the half length is often used, therefore this function exists
	 * for improved performance and precision.
	 *
	 * @note The tree's depth levels are `[0..depth()]`.
	 *
	 * @param depth the depth
	 * @return The reciprocal of the half length of nodes at `depth`.
	 */
	[[nodiscard]] Length halfLengthReciprocal(depth_t depth) const
	{
		assert(numDepthLevels() > depth);
		return node_half_length_reciprocal_[depth];
	}

	/*!
	 * @brief Returns the reciprocal of the half length of `node`, i.e. `1 / (length(node) /
	 * 2) = 2 / length(node)`.
	 *
	 * @note The reciprocal of the half length is often used, therefore this function exists
	 * for improved performance and precision.
	 *
	 * @param node the node
	 * @return The reciprocal of the half length of the node.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] Length halfLengthReciprocal(NodeType node) const
	{
		return halfLengthReciprocal(depth(node));
	}

	//
	// Bounds
	//

	/*!
	 * @brief Returns the bounds of the tree (/ root node).
	 *
	 * @return The bounds of the tree (/ root node).
	 */
	[[nodiscard]] Bounds bounds() const
	{
		Point c  = center();
		auto  hl = cast<coord_t>(halfLength());
		return Bounds(c - hl, c + hl);
	}

	/*!
	 * @brief Returns the bounds of `node`.
	 *
	 * @param node the node
	 * @return The bounds of the node.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] Bounds bounds(NodeType node) const
	{
		Point c  = center(node);
		auto  hl = cast<coord_t>(halfLength(node));
		return Bounds(c - hl, c + hl);
	}

	//
	// Inside
	//

	/*!
	 * @brief Checks if a coordinate is inside the tree bounds, i.e. inside `bounds()`.
	 *
	 * @param coord the coordinate
	 * @return `true` if the coordinate is inside the bounds, `false` otherwise.
	 */
	[[nodiscard]] bool isInside(Point coord) const
	{
		auto const hl = halfLength(depth());
		for (std::size_t i{}; coord.size() > i; ++i) {
			if (-hl[i] > coord[i] || hl[i] <= coord[i]) {
				return false;
			}
		}
		return true;
	}

	/**************************************************************************************
	|                                                                                     |
	|                                       Access                                        |
	|                                                                                     |
	**************************************************************************************/

	//
	// Center
	//

	/*!
	 * @brief Returns the center of the tree (/ root node).
	 *
	 * @return The center of the tree (/ root node).
	 */
	[[nodiscard]] Coord center() const { return Coord(Point(), depth()); }

	/*!
	 * @brief Returns the center of `node`.
	 *
	 * @param node the node
	 * @return The center of the node.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] constexpr Coord center(NodeType node) const
	{
		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			if constexpr (Block::HasCenter) {
				return isRoot(node) ? center()
				                    : treeBlock(node).center(node.offset, halfLength(node));
			} else {
				return center(treeBlock(node).code(node.offset));
			}
		} else if constexpr (std::is_same_v<T, Node>) {
			if constexpr (Block::HasCenter) {
				// LOOKAT: Check performance
				return center(index(node));
			} else {
				return center(code(node));
			}
		} else if constexpr (std::is_same_v<T, Code>) {
			return center(key(node));
		} else if constexpr (std::is_same_v<T, Key>) {
			assert(valid(node));

			auto node_depth = depth(node);

			if (depth() == node_depth) {
				return center();
			}

			// LOOKAT: Check performance, might be a lot faster to have float here and in rest
			// of method
			Length            l = length(node_depth);
			std::int_fast64_t hmv =
			    static_cast<std::int_fast64_t>(half_max_value_ >> node_depth);

			Point coord = cast<coord_t>((cast<length_t>(cast<std::int_fast64_t>(node) - hmv) +
			                             static_cast<length_t>(0.5)) *
			                            l);

			return Coord(coord, node_depth);
		} else if constexpr (contains_type_v<T, Coord, Coord2>) {
			return center(key(node));
		} else if constexpr (contains_type_v<T, Point, Point2>) {
			return center(Coord(node, 0u));
		} else {
			static_assert(is_node_type_v<NodeType>, "Not one of the node types");
		}
	}

	/*!
	 * @brief Returns the center of `node` if the node is valid, i.e. `valid(node)`.
	 *
	 * @param node the node
	 * @return The center of the node if the node is valid, null otherwise.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] std::optional<Coord> centerChecked(NodeType node) const
	{
		return valid(node) ? std::optional<Coord>(center(node)) : std::nullopt;
	}

	//
	// Center axis
	//

	/*!
	 * @brief Returns the center of the tree (/ root node) for the `axis` specified.
	 *
	 * @param axis the axis
	 * @return The center of the tree (/ root node) for the `axis` specified.
	 */
	[[nodiscard]] coord_t centerAxis(std::size_t axis) const
	{
		assert(Dim > axis);
		return coord_t(0);
	}

	/*!
	 * @brief Returns the center of `node` for the `axis` specified.
	 *
	 * @param node the node
	 * @param axis the axis
	 * @return The center of the node for the `axis` specified.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] coord_t centerAxis(NodeType node, std::size_t axis) const
	{
		assert(valid(node));
		assert(Dim > axis);

		key_t   k;
		depth_t d;

		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			if constexpr (Block::HasCenter) {
				return isRoot(node)
				           ? centerAxis(axis)
				           : treeBlock(node).centerAxis(node.offset, halfLength(node), axis);
			} else {
				return centerAxis(treeBlock(node).code(node.offset), axis);
			}
		} else if constexpr (std::is_same_v<T, Node>) {
			if constexpr (Block::HasCenter) {
				// LOOKAT: Check performance
				return centerAxis(index(node), axis);
			} else {
				return centerAxis(code(node), axis);
			}
		} else if constexpr (std::is_same_v<T, Code>) {
			k = node[axis];
			d = node.depth();
		} else if constexpr (std::is_same_v<T, Key>) {
			k = node[axis];
			d = node.depth();
		} else {
			if constexpr (contains_type_v<T, Coord, Coord2>) {
				d = depth(node);
			} else if constexpr (contains_type_v<T, Point, Point2>) {
				d = 0;
			} else {
				static_assert(is_node_type_v<NodeType>, "Not one of the node types");
			}

			auto p = node[axis];

			// LOOKAT: Check performance, might be a lot faster to have float here
			Length lr = lengthReciprocal(0);

			k = static_cast<key_t>(static_cast<std::make_signed_t<key_t>>(
			        std::floor(static_cast<length_t>(p) * lr))) +
			    half_max_value_;

			if constexpr (contains_type_v<T, Coord, Coord2>) {
				k >>= d;
			}
		}

		if (depth() == d) {
			return centerAxis(axis);
		}

		// LOOKAT: Check performance, might be a lot faster to have float here and in rest of
		// method
		length_t          l   = length(d)[axis];
		std::int_fast64_t hmv = static_cast<std::int_fast64_t>(half_max_value_ >> d);

		return static_cast<coord_t>(
		    (static_cast<length_t>(static_cast<std::int_fast64_t>(k) - hmv) +
		     static_cast<length_t>(0.5)) *
		    l);
	}

	/*!
	 * @brief Returns the center of `node` for the `axis` specified, if the node is valid
	 * (i.e., `valid(node)`).
	 *
	 * @param node the node
	 * @param axis the axis
	 * @return The center of the node for the `axis` specified if the node is valid, null
	 * otherwise.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] std::optional<coord_t> centerAxisChecked(NodeType    node,
	                                                       std::size_t axis) const
	{
		assert(Dim > axis);
		return valid(node) ? std::optional<coord_t>(centerAxis(node, axis)) : std::nullopt;
	}

	//
	// Block
	//

	/*!
	 * @brief Returns the block position of the root node.
	 *
	 * @return The block position of the root node.
	 */
	[[nodiscard]] pos_t block() const { return 0; }

	/*!
	 * @brief Returns the block position of `node`.
	 *
	 * @param node the node
	 * @return The block position of the node.
	 */

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] pos_t block(NodeType node) const
	{
		if constexpr (std::is_same_v<Index, remove_cvref_t<NodeType>>) {
			return node.pos;
		} else {
			return block(index(node));
		}
	}

	//
	// Index
	//

	/*!
	 * @brief Returns the index of the root node.
	 *
	 * @return The index of the root node.
	 */
	[[nodiscard]] Index index() const { return Index(block(), 0); }

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] constexpr Index index(NodeType const& node) const
	{
		assert(valid(node));

		auto fun = [this](Code code, Index node) {
			depth_t min_depth = this->depth(code);
			depth_t depth     = this->depth(node);
			while (min_depth < depth && isParent(node)) {
				node = child(node, code.offset(--depth));
			}
			return node;
		};

		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			return node;
		} else if constexpr (std::is_same_v<T, Node>) {
			// LOOKAT: Benchmark if this is actually faster than going down the tree

			if (!valid(node.index) || depth(node.index) < depth(node) ||
			    !Code::equalAtDepth(code(node.index), node.code, depth(node.index))) {
				return index(node.code);
			}

			if (code(node.index) == node.code || isLeaf(node.index)) {
				return node.index;
			}

			return fun(node.code, node.index);
		} else if constexpr (std::is_same_v<T, Code>) {
			return fun(node, index());
		} else {
			return index(code(node));
		}
	}

	//
	// Node
	//

	/*!
	 * @brief Returns the root node.
	 *
	 * @return The root node.
	 */
	[[nodiscard]] Node node() const { return Node(code(), index()); }

	/*!
	 * @brief Returns the node corresponding to `node`.
	 *
	 * @param node the node
	 * @return The node.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] Node node(NodeType node) const
	{
		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			assert(valid(node));
			return Node(code(node), node);
		} else if constexpr (std::is_same_v<T, Node>) {
			return Node(node.code, index(node));
		} else if constexpr (std::is_same_v<T, Code>) {
			return Node(node, index(node));
		} else {
			return this->node(code(node));
		}
	}

	/*!
	 * @brief Get the node corresponding to a code.
	 *
	 * @note The node can be higher up the tree than the specified depth. This happens if
	 * the node at a higher depth has no children. If it is neccessary that the node is at
	 * the specified depth, then the corresponding 'createNode' function can be used. The
	 * data inside the nodes returned by this function and 'createNode' will be the same, so
	 * it is only neccessary to use 'createNode' if you intend to alter what the node
	 * stores.
	 *
	 * @param node The node.
	 * @return The node.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] Node operator[](NodeType node) const
	{
		return this->node(node);
	}

	//
	// Code
	//

	[[nodiscard]] Code code() const { return Code(std::array<code_t, 3>{}, depth()); }

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] Code code(NodeType const& node) const
	{
		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			assert(valid(node));
			return treeBlock(node).code(node.offset);
		} else if constexpr (std::is_same_v<T, Node>) {
			return node.code;
		} else if constexpr (std::is_same_v<T, Code>) {
			return node;
		} else if constexpr (std::is_same_v<T, Key>) {
			return Code(node);
		} else if constexpr (contains_type_v<T, Coord, Coord2, Point, Point2>) {
			return code(key(node));
		} else if constexpr (std::is_convertible_v<T, Index>) {
			Index n = static_cast<Index>(node);
			assert(valid(n));
			return treeBlock(n).code(n.offset);
		} else if constexpr (std::is_convertible_v<T, Node>) {
			return static_cast<Node const&>(node).code;
		} else if constexpr (std::is_convertible_v<T, Code>) {
			return static_cast<Code const&>(node);
		} else if constexpr (std::is_convertible_v<T, Key>) {
			return Code(static_cast<Key const&>(node));
		} else {
			// FIXME: Point is error?
		}
	}

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] std::optional<Code> codeChecked(NodeType const& node) const
	{
		return valid(node) ? std::optional<Code>(Code(node)) : std::nullopt;
	}

	//
	// Key
	//

	[[nodiscard]] Key key() const { return Key(Vec<Dim, key_t>(0), depth()); }

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] Key key(NodeType const& node) const
	{
		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			return key(treeBlock(node).code(node.offset));
		} else if constexpr (std::is_same_v<T, Node>) {
			return key(node.code);
		} else if constexpr (std::is_same_v<T, Code>) {
			return Key(node);
		} else if constexpr (std::is_same_v<T, Key>) {
			return node;
		} else if constexpr (contains_type_v<T, Coord, Coord2>) {
			assert(valid(node));

			auto  d = depth(node);
			Point p = node;

			// LOOKAT: Check performance, might be a lot faster to have float here
			Length lr = lengthReciprocal(0);

			auto k =
			    cast<key_t>(cast<std::make_signed_t<key_t>>(floor(cast<length_t>(p) * lr))) +
			    half_max_value_;

			return {k >> d, d};
		} else if constexpr (contains_type_v<T, Point, Point2>) {
			return key(Coord(node, 0u));
		}
	}

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] std::optional<Key> keyChecked(NodeType node) const
	{
		return valid(node) ? std::optional<Key>(key(node)) : std::nullopt;
	}

	/**************************************************************************************
	|                                                                                     |
	|                                      Modified                                       |
	|                                                                                     |
	**************************************************************************************/

	/*!
	 * @brief Check if the root of the tree is modified.
	 *
	 * @return Whether the root of the tree is in a modified state.
	 */
	[[nodiscard]] bool modified() const { return modified(index()); }

	/*!
	 * @brief Check if a node of the tree is in a modified state.
	 *
	 * @param node The node to check.
	 * @return Whether the node is in a modified state.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool modified(NodeType node) const
	{
		Index n = index(node);
		return treeBlock(n.pos).modified(n.offset);
	}

	void modifiedSet(bool value) { return value ? modifiedSet() : modifiedReset(); }

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	void modifiedSet(NodeType node, bool value)
	{
		return value ? modifiedSet(node) : modifiedReset(node);
	}

	void modifiedSet() { modifiedSet(index()); }

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	void modifiedSet(NodeType node)
	{
		auto node_f = [this](Index node) { treeBlock(node.pos).modifiedSet(node.offset); };

		auto block_f = [this](pos_t block) { treeBlock(block).modifiedSet(); };

		auto update_f = [this](Index node, pos_t children) {
			treeBlock(node.pos).modifiedSet(node.offset, treeBlock(children).modifiedAny());
		};

		recursParentFirst(node, node_f, block_f, update_f, !modified(parent(node)));
	}

	void modifiedReset() { modifiedReset(index()); }

	// template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	// void modifiedReset(NodeType node)
	// {
	// 	auto node_f = [this](Index node) { treeBlock(node.pos).modifiedReset(node.offset);
	// };

	// 	auto block_f = [this](pos_t block) { treeBlock(block).modifiedReset(); };

	// 	auto update_f = [this](Index /* node */, pos_t /* children */) {};

	// 	recursParentFirst(node, node_f, block_f, update_f, false);
	// }

	void modifiedReset(pos_t block)
	{
		assert(valid(block));

		auto m = treeBlock(block).modified();

		if (0u == m) {
			return;
		}

		treeBlock(block).modifiedReset();

		for (std::size_t i{}; BF > i; ++i) {
			auto n = Index(block, i);
			auto c = children(n);
			if (0u == (m & (1u << i)) || !valid(c)) {
				continue;
			}

			modifiedReset(c);
		}
	}

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	void modifiedReset(NodeType const& node)
	{
		assert(valid(node));

		auto n = index(node);

		if (!modified(n)) {
			return;
		}

		treeBlock(n.pos).modifiedReset(n.offset);

		auto c = children(n);

		if (!valid(c)) {
			return;
		}

		modifiedReset(c);
	}

	/**************************************************************************************
	|                                                                                     |
	|                                        Lock                                         |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] Spinlock& chicken(pos_t block) noexcept(
	    noexcept(treeBlock(block).chicken()))
	{
		return treeBlock(block).chicken();
	}

	void lock(pos_t block) noexcept(noexcept(treeBlock(block).lock()))
	{
		treeBlock(block).lock();
	}

	[[nodiscard]] bool try_lock(pos_t block) noexcept(noexcept(treeBlock(block).try_lock()))
	{
		return treeBlock(block).try_lock();
	}

	void unlock(pos_t block) noexcept(noexcept(treeBlock(block).unlock()))
	{
		treeBlock(block).unlock();
	}

	/**************************************************************************************
	|                                                                                     |
	|                                 Create/erase nodes                                  |
	|                                                                                     |
	**************************************************************************************/

	//
	// Create
	//

	// TODO: Add proper guards for the templates

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	Index create(NodeType node)
	{
		assert(valid(node));

		if constexpr (std::is_same_v<Index, remove_cvref_t<NodeType>>) {
			Index      res        = node;
			auto const root_depth = depth();
			for (auto d = depth(node); root_depth > d; ++d) {
				auto const children = node.pos;
				node                = parent(node);
				// NOTE: It is important that this check is here so it does not check the incoming
				// node. This would mess up the `recursLeaves` and `recursParentFirst` otherwise.
				if (modified(node)) {
					return res;
				}
				treeBlock(node.pos).modifiedSet(node.offset);
			}
			return res;
		} else {
			Code code         = this->code(node);
			auto wanted_depth = depth(code);
			auto cur_node     = index();
			auto cur_depth    = depth();
			while (wanted_depth < cur_depth) {
				cur_node = createChild(cur_node, code.offset(--cur_depth));
			}
			return cur_node;
		}
	}

	template <class InputIt, class OutputIt>
	OutputIt create(InputIt first, InputIt last, OutputIt d_first)
	{
		using value_type = remove_cvref_t<typename std::iterator_traits<InputIt>::value_type>;

		if constexpr (std::is_same_v<Index, value_type>) {
			// TODO: Should be marked modified
			return std::copy(first, last, d_first);
		} else {
			Index node      = this->index();
			Code  node_code = this->code();

			return std::transform(first, last, d_first,
			                      [this, &node, &node_code](auto const& x) {
				                      Code    d_code = code(x);
				                      depth_t d      = Code::depthWhereEqual(node_code, d_code);

				                      node      = ancestor(node, d);
				                      node_code = d_code;
				                      for (depth_t d_depth = depth(d_code); d_depth < d; --d) {
					                      node = createChild(node, d_code.offset(d - 1));
				                      }

				                      return node;
			                      });
		}
	}

	template <class InputIt>
	std::vector<Index> create(InputIt first, InputIt last)
	{
		std::vector<Index> nodes;
		create(first, last, std::back_inserter(nodes));
		return nodes;
	}

	template <
	    class ExecutionPolicy, class RandomIt1, class RandomIt2,
	    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
	RandomIt2 create(ExecutionPolicy&& policy, RandomIt1 first, RandomIt1 last,
	                 RandomIt2 d_first)
	{
		using value_type =
		    remove_cvref_t<typename std::iterator_traits<RandomIt1>::value_type>;

		if constexpr (std::is_same_v<Index, value_type>) {
			// TODO: Should be marked modified
			// FIXME: Can be parallelize
			return std::copy(first, last, d_first);
		} else {
			// NOTE: Possible (although, highly unlikely) problem. If this function is called
			// more than max value of `std::size_t`, so `create_call_num` overflows, *AND* a
			// thread has persisted but not been used for a multiple of max value of
			// `std::size_t` iterations in the `transform` call below; then `node` and
			// `node_code` would not be reset to the root node as they should. This means that
			// invalid memory is being accessed.
			static std::size_t create_call_num{};
			++create_call_num;

			return transform(std::forward<ExecutionPolicy>(policy), first, last, d_first,
			                 [this, ccn = create_call_num](auto const& x) {
				                 thread_local Index node      = index();
				                 thread_local Code  node_code = code();

				                 thread_local std::size_t thread_create_call_num = 1;
				                 if (ccn != thread_create_call_num) {
					                 thread_create_call_num = ccn;
					                 node                   = index();
					                 node_code              = code();
				                 }

				                 Code    d_code = code(x);
				                 depth_t d      = Code::depthWhereEqual(node_code, d_code);

				                 node      = ancestor(node, d);
				                 node_code = d_code;
				                 for (depth_t d_depth = depth(d_code); d_depth < d; --d) {
					                 node = createChildThreadSafe(node, d_code.offset(d - 1));
				                 }

				                 return node;
			                 });

			// return transform(std::forward<ExecutionPolicy>(policy), first, last, d_first,
			//                  [this](auto const& x) {
			// 	                 thread_local Index node = index();

			// 	                 // NOTE: `node` can be from last call to `create` (if the same
			// 	                 // thread still persists), so we need to check if the node is
			// 	                 // valid (i.e., has not been deleted). If it has been deleted,
			// 	                 // we set it to the root node.
			// 	                 // FIXME: Note sure if `valid` is thread safe
			// 	                 node           = valid(node) ? node : index();
			// 	                 Code node_code = code(node);

			// 	                 Code    d_code = code(x);
			// 	                 depth_t d      = Code::depthWhereEqual(node_code, d_code);

			// 	                 node      = ancestor(node, d);
			// 	                 node_code = d_code;
			// 	                 for (depth_t d_depth = depth(d_code); d_depth < d; --d) {
			// 		                 node = createChildThreadSafe(node, d_code.offset(d - 1));
			// 	                 }

			// 	                 return node;
			//                  });
		}
	}

	template <
	    class ExecutionPolicy, class RandomIt,
	    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
	std::vector<Index> create(ExecutionPolicy&& policy, RandomIt first, RandomIt last)
	{
		__block std::vector<Index> nodes(std::distance(first, last));
		create(std::forward<ExecutionPolicy>(policy), first, last, nodes.begin());
		return nodes;
	}

	template <
	    class Range, class OutputIt,
	    std::enable_if_t<!is_node_type_v<Range> && !execution::is_execution_policy_v<Range>,
	                     bool> = true>
	OutputIt create(Range const& r, OutputIt d_first)
	{
		using std::begin;
		using std::end;
		return create(begin(r), end(r), d_first);
	}

	template <class Range, std::enable_if_t<!is_node_type_v<Range>, bool> = true>
	std::vector<Index> create(Range const& r)
	{
		std::vector<Index> nodes;
		create(r, std::back_inserter(nodes));
		return nodes;
	}

	template <
	    class ExecutionPolicy, class Range, class RandomIt,
	    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true,
	    std::enable_if_t<!is_node_type_v<Range>, bool>                            = true>
	RandomIt create(ExecutionPolicy&& policy, Range const& r, RandomIt d_first)
	{
		using std::begin;
		using std::end;
		return create(std::forward<ExecutionPolicy>(policy), begin(r), end(r), d_first);
	}

	template <
	    class ExecutionPolicy, class Range,
	    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true,
	    std::enable_if_t<!is_node_type_v<Range>, bool>                            = true>
	std::vector<Index> create(ExecutionPolicy&& policy, Range const& r)
	{
		using std::size;
		__block std::vector<Index> nodes(std::size(r));
		create(std::forward<ExecutionPolicy>(policy), r, nodes.begin());
		return nodes;
	}

	//
	// Erase
	//

	void eraseChildren() { eraseChildren(index()); }

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	void eraseChildren(NodeType const& node)
	{
		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			if (isLeaf(node)) {
				return;
			}

			auto c = children(node);
			for (offset_t i{}; BF > i; ++i) {
				eraseChildren(Index(c, i));
			}

			pruneChildren(node);
		} else if constexpr (std::is_same_v<T, Node>) {
			Index n = index(node);
			if (code(n) != code(node)) {
				// The node does not even exist
				return;
			}

			eraseChildren(n);
		} else if constexpr (std::is_same_v<T, Code>) {
			Index n = index(node);
			if (code(n) != node) {
				// The node does not even exist
				return;
			}

			eraseChildren(n);
		} else {
			eraseChildren(code(node));
		}
	}

	/**************************************************************************************
	|                                                                                     |
	|                                        Leaf                                         |
	|                                                                                     |
	**************************************************************************************/

	//
	// Pure leaf
	//

	/*!
	 * @brief Checks if the block is pure leaf (i.e., can never have children).
	 *
	 * @note Only have to check if the depth of the block is 0.
	 *
	 * @param block the block to check
	 * @return `true` if the block is pure leaf, `false` otherwise.
	 */
	[[nodiscard]] bool isPureLeaf(pos_t block) const { return 0 == depth(block); }

	/*!
	 * @brief Checks if the node is a pure leaf (i.e., can never have children).
	 *
	 * @note Only have to check if the depth of the node is 0.
	 *
	 * @param node the node to check
	 * @return `true` if the node is a pure leaf, `false` otherwise.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] constexpr bool isPureLeaf(NodeType node) const
	{
		return 0 == depth(node);
	}

	//
	// Leaf
	//

	/*!
	 * @brief Checks if the node is a leaf (i.e., has no children).
	 *
	 * @param node the node to check
	 * @return `true` if the node is a leaf, `false` otherwise.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] constexpr bool isLeaf(NodeType node) const
	{
		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			static_assert(TreeIndex::PROCESSING_POS <= TreeIndex::NULL_POS);
			return TreeIndex::PROCESSING_POS <= children(node);
		} else {
			return isLeaf(index(node));
		}
	}

	//
	// Parent
	//

	/*!
	 * @brief Checks if the node is a parent (i.e., has children).
	 *
	 * @param node the node to check
	 * @return `true` if the node is a parent, `false` otherwise.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] constexpr bool isParent(NodeType node) const
	{
		return !isLeaf(node);
	}

	/**************************************************************************************
	|                                                                                     |
	|                                        Root                                         |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] constexpr bool isRoot(pos_t block) const
	{
		return this->block() == block;
	}

	/*!
	 * @brief Checks if the node is the root of the tree.
	 *
	 * @param node the node to check
	 * @return `true` if the node is the root, `false` otherwise.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] constexpr bool isRoot(NodeType node) const
	{
		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			return index() == node;
		} else if constexpr (std::is_same_v<T, Node>) {
			return isRoot(node.code);
		} else if constexpr (std::is_same_v<T, Code>) {
			return code() == node;
		} else if constexpr (std::is_same_v<T, Key>) {
			return key() == node;
		} else {
			return isRoot(key(node));
		}
	}

	/**************************************************************************************
	|                                                                                     |
	|                                        Valid                                        |
	|                                                                                     |
	**************************************************************************************/

	/*!
	 * @brief Checks if a block is valid.
	 *
	 * @param block the block to check
	 * @return `true` if the block is valid, `false` otherwise.
	 */
	[[nodiscard]] bool valid(pos_t block) const { return Data::valid(block); }

	/*!
	 * @brief Checks if an index is valid.
	 *
	 * @param index the index to check
	 * @return `true` if the index is valid, `false` otherwise.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool valid(NodeType node) const
	{
		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			return valid(node.pos) && branchingFactor() > node.offset &&
			       treeBlock(node).valid();
		} else if constexpr (std::is_same_v<T, Node>) {
			return valid(code(node));
		} else if constexpr (std::is_same_v<T, Code>) {
			return node.valid() && numDepthLevels() > depth(node);
		} else if constexpr (std::is_same_v<T, Key>) {
			auto const mv = (2 * half_max_value_) >> depth(node);
			for (std::size_t i{}; node.size() != i; ++i) {
				if (mv < node[i]) {
					return false;
				}
			}

			return node.valid() && numDepthLevels() > depth(node);
		} else {
			return isInside(node) && numDepthLevels() > depth(node);
		}
	}

	/**************************************************************************************
	|                                                                                     |
	|                                        Exist                                        |
	|                                                                                     |
	**************************************************************************************/

	/*!
	 * @brief Checks if a node exists.
	 *
	 * @param node the node to check
	 * @return `true` if the node exists, `false` otherwise.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool exists(NodeType node) const
	{
		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			return valid(node);
		} else if constexpr (std::is_same_v<T, Node>) {
			return code(index(node)) == code(node);
		} else if constexpr (std::is_same_v<T, Code>) {
			return code(index(node)) == node;
		} else {
			return exists(code(node));
		}
	}

	/**************************************************************************************
	|                                                                                     |
	|                                      Traverse                                       |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] std::array<pos_t, BF> children(pos_t block) const
	{
		assert(valid(block));

		auto const& tb = treeBlock(block);

		std::array<pos_t, BF> children;
		for (std::size_t i{}; BF > i; ++i) {
			children[i] = tb.children[i].load(std::memory_order_relaxed);
		}
		return children;
	}

	[[nodiscard]] pos_t children(Index node) const
	{
		assert(valid(node));
		// assert(isParent(node));

		return treeBlock(node).children[node.offset].load(std::memory_order_relaxed);
	}

	/*!
	 * @brief Returns the `i`:th child of `node`.
	 *
	 * @param node the node to return the child of.
	 * @param i the index of the child (in range `[0..2^Dim)`).
	 * @return `i`:th child of `node`.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] constexpr NodeType child(NodeType const& node, offset_t i) const
	{
		assert(0 < depth(node));
		assert(BF > i);

		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			assert(valid(node));
			return Index(children(node), i);
		} else if constexpr (std::is_same_v<T, Node>) {
			return Node(child(node.code, i), (valid(node.index) && isParent(node.index) &&
			                                  code(node.index) == node.code)
			                                     ? child(node.index, i)
			                                     : node.index);
		} else if constexpr (std::is_same_v<T, Code>) {
			return node.child(i);
		} else if constexpr (std::is_same_v<T, Key>) {
			return node.child(i);
		} else {
			return {childCenter(static_cast<Point>(node), halfLength(node), i),
			        node.depth - static_cast<depth_t>(1)};
		}
	}

	/*!
	 * @brief Get a child of a node with bounds checking.
	 *
	 * @param node The node.
	 * @param i The index of the child.
	 * @return The child.
	 */
	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] NodeType childChecked(NodeType node, offset_t i) const
	{
		if (isLeaf(node)) {
			throw std::out_of_range("Node has no children");
		} else if (BF <= i) {
			throw std::out_of_range("i out of range");
		}
		return child(node, i);
	}

	//
	// Sibling
	//

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] NodeType sibling(NodeType node, offset_t i) const
	{
		assert(!isRoot(node));
		assert(BF > i);

		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			return Index(node.pos, i);
		} else if constexpr (std::is_same_v<T, Node>) {
			return Node(sibling(node.code, i),
			            (valid(node.index) && code(node.index) == node.code)
			                ? sibling(node.index, i)
			                : node.index);
		} else if constexpr (std::is_same_v<T, Code>) {
			return node.sibling(i);
		} else if constexpr (std::is_same_v<T, Key>) {
			return node.sibling(i);
		} else {
			return center(sibling(key(node), i));
		}
	}

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] NodeType siblingChecked(NodeType node, offset_t i) const
	{
		if (!isRoot(node)) {
			throw std::out_of_range("Root node has no siblings");
		} else if (BF <= i) {
			throw std::out_of_range("i out of range");
		}
		return sibling(node, i);
	}

	//
	// Parent
	//

	[[nodiscard]] pos_t parentBlock(pos_t block) const
	{
		assert(!isRoot(block));
		return treeBlock(block).parentBlock();
	}

	[[nodiscard]] Index parent(pos_t block) const
	{
		assert(!isRoot(block));
		return treeBlock(block).parent();
	}

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] NodeType parent(NodeType node) const
	{
		assert(!isRoot(node));

		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			return treeBlock(node).parent();
		} else if constexpr (std::is_same_v<T, Node>) {
			return Node(parent(node.code), (valid(node.index) && code(node.index) == node.code)
			                                   ? parent(node.index)
			                                   : node.index);
		} else if constexpr (std::is_same_v<T, Code>) {
			return node.parent();
		} else if constexpr (std::is_same_v<T, Key>) {
			return node.parent();
		} else {
			return center(parent(key(node)));
		}
	}

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] NodeType parentChecked(NodeType node) const
	{
		if (!isRoot(node)) {
			throw std::out_of_range("Root node has no parent");
		}
		return parent(node);
	}

	//
	// Ancestor
	//

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] NodeType ancestor(NodeType node, depth_t depth) const
	{
		assert(!isRoot(node) || this->depth(node) == depth);
		assert(this->depth(node) <= depth);

		if (this->depth(node) == depth) {
			return node;
		}

		using T = remove_cvref_t<NodeType>;
		if constexpr (std::is_same_v<T, Index>) {
			pos_t block = node.pos;
			for (depth_t d = this->depth(node); depth > d + 1; ++d) {
				block = treeBlock(block).parentBlock();
			}
			return treeBlock(block).parent();
		} else if constexpr (std::is_same_v<T, Node>) {
			return Node(ancestor(node.code, depth),
			            (valid(node.index) && code(node.index) == node.code)
			                ? ancestor(node.index, depth)
			                : node.index);
		} else if constexpr (std::is_same_v<T, Code>) {
			return node.toDepth(depth);
		} else if constexpr (std::is_same_v<T, Key>) {
			return node.toDepth(depth);
		} else {
			return center(ancestor(key(node), depth));
		}
	}

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] NodeType ancestorChecked(NodeType node, depth_t depth) const
	{
		if (!isRoot(node)) {
			throw std::out_of_range("Root node has no ancestor");
		} else if (this->depth(node) > depth) {
			throw std::out_of_range(
			    "Ancestors are only upwards (towards heaven, maybe your ancestors went to "
			    "hell?(!))");
		} else if (this->depth() < depth) {
			throw std::out_of_range(
			    "Trying to access ancestors before the big bang (i.e., above the root node)");
		}
		return ancestor(node, depth);
	}

	/*!
	 * @brief Depth first traversal of the tree, starting at the root node. The function
	 * 'f' will be called for each node traverse. If 'f' returns true then the children of
	 * the node will also be traverse, otherwise they will not.
	 *
	 * @param f The callback function to be called for each node traversed.
	 */
	template <class UnaryFun,
	          std::enable_if_t<std::is_invocable_r_v<bool, UnaryFun, Index>, bool> = true>
	void traverse(UnaryFun f) const
	{
		traverse(index(), f);
	}

	/*!
	 * @brief Depth first traversal of the tree, starting at `node`. The function 'f'
	 * will be called for each traversed node. If 'f' returns `true` then the children of
	 * the node will also be traverse, otherwise they will not.
	 *
	 * @param node The node to start the traversal from.
	 * @param f The callback function to be called for each node traversed.
	 */
	template <class NodeType, class UnaryFun,
	          std::enable_if_t<is_node_type_v<NodeType>, bool>                     = true,
	          std::enable_if_t<std::is_invocable_r_v<bool, UnaryFun, Index>, bool> = true>
	void traverse(NodeType node, UnaryFun f) const
	{
		assert(valid(node));

		if (!exists(node)) {
			return;
		}

		Index root = index(node);
		Index cur  = root;

		while (f(cur) && isParent(cur)) {
			cur = child(cur, 0);
		}

		while (root != cur) {
			if (BF - 1 == cur.offset) {
				cur = parent(cur);
				continue;
			}

			++cur.offset;

			while (f(cur) && isParent(cur)) {
				cur = child(cur, 0);
			}
		}
	}

	/*!
	 * @brief Depth first traversal of the tree, starting at the root node. The function
	 * 'f' will be called for each node traverse. If 'f' returns true then the children of
	 * the node will also be traverse, otherwise they will not.
	 *
	 * @param f The callback function to be called for each node traversed.
	 * @param pred Predicates that need to be fulfilled.
	 * @param only_exists Whether only existing nodes should be traversed.
	 */
	template <class UnaryFun, class Predicate = pred::True,
	          std::enable_if_t<std::is_invocable_r_v<bool, UnaryFun, Node>, bool> = true,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool>                  = true>
	void traverse(UnaryFun f, Predicate const& pred = pred::True{},
	              bool only_exists = true) const
	{
		traverse(node(), f, pred, only_exists);
	}

	/*!
	 * @brief Depth first traversal of the tree, starting at `node`. The function 'f'
	 * will be called for each traversed node. If 'f' returns `true` then the children of
	 * the node will also be traverse, otherwise they will not.
	 *
	 * @param node The node to start the traversal from.
	 * @param f The callback function to be called for each node traversed.
	 * @param pred Predicates that need to be fulfilled.
	 * @param only_exists Whether only existing nodes should be traversed.
	 */
	template <class NodeType, class UnaryFun, class Predicate = pred::True,
	          std::enable_if_t<is_node_type_v<NodeType>, bool>                    = true,
	          std::enable_if_t<std::is_invocable_r_v<bool, UnaryFun, Node>, bool> = true,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool>                  = true>
	void traverse(NodeType node, UnaryFun f, Predicate pred = pred::True{},
	              bool only_exists = true) const
	{
		assert(valid(node));

		using Filter = pred::Filter<Predicate>;

		Filter::init(pred, derived());

		if (only_exists) {
			if (!exists(node)) {
				return;
			}

			Index root = index(node);
			Index cur  = root;

			auto fun = [this, f, &pred](Node const& node) {
				return (!Filter::returnable(pred, derived(), node) || f(node)) &&
				       isParent(node.index) && Filter::traversable(pred, derived(), node);
			};

			while (fun(this->node(cur))) {
				cur = child(cur, 0);
			}

			while (root != cur) {
				if (BF - 1 == cur.offset) {
					cur = parent(cur);
					continue;
				}

				++cur.offset;

				while (fun(this->node(cur))) {
					cur = child(cur, 0);
				}
			}
		} else {
			Node root = this->node(node);
			Node cur  = root;

			auto fun = [this, f, &pred](Node& node) {
				bool ret = (!Filter::returnable(pred, derived(), node) || f(node)) &&
				           !isPureLeaf(node.code) && Filter::traversable(pred, derived(), node);

				// Fix index
				auto min_depth = this->depth(node.code);
				auto depth     = this->depth(node.index);
				while (min_depth < depth && isParent(node.index)) {
					node.index = child(node.index, node.code.offset(--depth));
				}

				return ret;
			};

			while (fun(cur)) {
				cur = Node(child(cur.code, 0),
				           isParent(cur.index) ? child(cur.index, 0) : cur.index);
			}

			while (root != cur) {
				auto branch = cur.code.offset();
				if (BF - 1 == branch) {
					cur = Node(parent(cur.code),
					           code(cur.index) == cur.code ? parent(cur.index) : cur.index);
					continue;
				}

				cur = Node(sibling(cur.code, branch + 1), code(cur.index) == cur.code
				                                              ? sibling(cur.index, branch + 1)
				                                              : cur.index);

				while (fun(cur)) {
					cur = Node(child(cur.code, 0),
					           isParent(cur.index) ? child(cur.index, 0) : cur.index);
				}
			}
		}
	}

	/**************************************************************************************
	|                                                                                     |
	|                                      Iterators                                      |
	|                                                                                     |
	**************************************************************************************/

	//
	// Iterator
	//

	[[nodiscard]] const_iterator begin(bool only_leaves = true,
	                                   bool only_exists = true) const
	{
		return begin(node(), only_leaves, only_exists);
	}

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] const_iterator begin(NodeType node, bool only_leaves = true,
	                                   bool only_exists = true) const
	{
		return const_iterator(const_cast<Derived*>(&derived()), this->node(node), only_leaves,
		                      only_exists);
	}

	[[nodiscard]] const_iterator end() const { return const_iterator(); }

	//
	// Query iterator
	//

	template <class Predicate, std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] const_query_iterator_pred<Predicate> beginQuery(
	    Predicate const& pred, bool only_exists = true, bool early_stopping = false) const
	{
		return beginQuery(node(), pred, only_exists, early_stopping);
	}

	template <class NodeType, class Predicate,
	          std::enable_if_t<is_node_type_v<NodeType>, bool>   = true,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] const_query_iterator_pred<Predicate> beginQuery(
	    NodeType node, Predicate const& pred, bool only_exists = true,
	    bool early_stopping = false) const
	{
		return const_query_iterator_pred<Predicate>(const_cast<Derived*>(&derived()),
		                                            this->node(node), pred, only_exists,
		                                            early_stopping);
	}

	[[nodiscard]] const_query_iterator endQuery() const { return const_query_iterator(); }

	//
	// Nearest iterator
	//

	template <class Geometry>
	[[nodiscard]] const_nearest_iterator_geom<Geometry> beginNearest(
	    Geometry const& geometry, double epsilon = 0.0, bool only_leaves = true,
	    bool only_exists = true) const
	{
		return beginNearest(node(), geometry, epsilon, only_leaves, only_exists);
	}

	template <class NodeType, class Geometry,
	          std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] const_nearest_iterator_geom<Geometry> beginNearest(
	    NodeType node, Geometry const& geometry, double epsilon = 0.0,
	    bool only_leaves = true, bool only_exists = true) const
	{
		return const_nearest_iterator_geom<Geometry>(const_cast<Derived*>(&derived()),
		                                             this->node(node), geometry, epsilon,
		                                             only_leaves, only_exists);
	}

	[[nodiscard]] const_nearest_iterator endNearest() const
	{
		return const_nearest_iterator();
	}

	//
	// Query nearest iterator
	//

	template <class Predicate, class Geometry,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] const_query_nearest_iterator_pred_geom<Predicate, Geometry>
	beginQueryNearest(Predicate const& pred, Geometry const& geometry, double epsilon = 0.0,
	                  bool only_exists = true, bool early_stopping = false) const
	{
		return beginQueryNearest(node(), pred, geometry, epsilon, only_exists,
		                         early_stopping);
	}

	template <class NodeType, class Predicate, class Geometry,
	          std::enable_if_t<is_node_type_v<NodeType>, bool>   = true,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] const_query_nearest_iterator_pred_geom<Predicate, Geometry>
	beginQueryNearest(NodeType node, Predicate const& pred, Geometry const& geometry,
	                  double epsilon = 0.0, bool only_exists = true,
	                  bool early_stopping = false) const
	{
		return const_query_nearest_iterator_pred_geom<Predicate, Geometry>(
		    const_cast<Derived*>(&derived()), this->node(node), pred, geometry, epsilon,
		    only_exists, early_stopping);
	}

	[[nodiscard]] const_query_nearest_iterator endQueryNearest() const
	{
		return const_query_nearest_iterator();
	}

	/**************************************************************************************
	|                                                                                     |
	|                                        Query                                        |
	|                                                                                     |
	**************************************************************************************/

	//
	// Query
	//

	template <class Predicate, std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] ConstQuery<Predicate> query(Predicate const& pred,
	                                          bool             only_exists    = true,
	                                          bool             early_stopping = false) const
	{
		return query(node(), pred, only_exists, early_stopping);
	}

	template <class NodeType, class Predicate,
	          std::enable_if_t<is_node_type_v<NodeType>, bool>   = true,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] ConstQuery<Predicate> query(NodeType node, Predicate const& pred,
	                                          bool only_exists    = true,
	                                          bool early_stopping = false) const
	{
		return ConstQuery<Predicate>(beginQuery(node, pred, only_exists, early_stopping),
		                             endQuery());
	}

	//
	// Nearest
	//

	template <class Geometry>
	[[nodiscard]] ConstNearest<Geometry> nearest(Geometry const& geometry,
	                                             double          epsilon     = 0.0,
	                                             bool            only_leaves = true,
	                                             bool            only_exists = true) const
	{
		return nearest(node(), geometry, epsilon, only_leaves, only_exists);
	}

	template <class NodeType, class Geometry,
	          std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] ConstNearest<Geometry> queryNearest(NodeType        node,
	                                                  Geometry const& geometry,
	                                                  double          epsilon     = 0.0,
	                                                  bool            only_leaves = true,
	                                                  bool only_exists = true) const
	{
		return ConstNearest<Geometry>(
		    beginNearest(node, geometry, epsilon, only_leaves, only_exists), endNearest());
	}

	//
	// Query nearest
	//

	template <class Predicate, class Geometry,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] ConstQueryNearest<Predicate, Geometry> queryNearest(
	    Predicate const& pred, Geometry const& geometry, double epsilon = 0.0,
	    bool only_exists = true, bool early_stopping = false) const
	{
		return queryNearest(node(), pred, geometry, epsilon, only_exists, early_stopping);
	}

	template <class NodeType, class Predicate, class Geometry,
	          std::enable_if_t<is_node_type_v<NodeType>, bool>   = true,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] ConstQueryNearest<Predicate, Geometry> queryNearest(
	    NodeType node, Predicate const& pred, Geometry const& geometry,
	    double epsilon = 0.0, bool only_exists = true, bool early_stopping = false) const
	{
		return ConstQueryNearest<Predicate, Geometry>(
		    beginQueryNearest(node, pred, geometry, epsilon, only_exists, early_stopping),
		    endQueryNearest());
	}

	/**************************************************************************************
	|                                                                                     |
	|                                        Trace                                        |
	|                                                                                     |
	**************************************************************************************/

	template <class Predicate, std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] TraceResult<Dim> trace(
	    Ray<Dim, ray_t> const& ray, Predicate const& pred, float min_dist = 0.0f,
	    float max_dist = std::numeric_limits<float>::max()) const
	{
		return trace(index(), ray, pred, min_dist, max_dist);
	}

	template <class NodeType, class Predicate,
	          std::enable_if_t<is_node_type_v<NodeType>, bool>   = true,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] TraceResult<Dim> trace(
	    NodeType node, Ray<Dim, ray_t> const& ray, Predicate pred, float min_dist = 0.0f,
	    float max_dist = std::numeric_limits<float>::max()) const
	{
		using Filter = pred::Filter<Predicate>;

		Filter::init(pred, derived());

		Node n = node(node);
		if (!exists(n)) {
			return TraceResult<Dim>{
			    Index(), Vec<Dim, float>(std::numeric_limits<float>::quiet_NaN()), -1.0f};
		}

		auto params = traceInit(n, ray);
		return trace(n, params, pred, min_dist, max_dist);
	}

	template <class InputIt, class OutputIt, class Predicate,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	OutputIt trace(InputIt first, InputIt last, OutputIt d_first, Predicate const& pred,
	               float min_dist = 0.0f,
	               float max_dist = std::numeric_limits<float>::max()) const
	{
		return trace(index(), first, last, d_first, pred, min_dist, max_dist);
	}

	template <class NodeType, class InputIt, class OutputIt, class Predicate,
	          std::enable_if_t<is_node_type_v<NodeType>, bool>   = true,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	OutputIt trace(NodeType node, InputIt first, InputIt last, OutputIt d_first,
	               Predicate const& pred, float min_dist = 0.0f,
	               float max_dist = std::numeric_limits<float>::max()) const
	{
		// TODO: Implement
	}

	template <class InputIt, class Predicate,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] std::vector<TraceResult<Dim>> trace(
	    InputIt first, InputIt last, Predicate const& pred, float min_dist = 0.0f,
	    float max_dist = std::numeric_limits<float>::max()) const
	{
		return trace(index(), first, last, pred, min_dist, max_dist);
	}

	template <class NodeType, class InputIt, class Predicate,
	          std::enable_if_t<is_node_type_v<NodeType>, bool>   = true,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] std::vector<TraceResult<Dim>> trace(
	    NodeType node, InputIt first, InputIt last, Predicate const& pred,
	    float min_dist = 0.0f, float max_dist = std::numeric_limits<float>::max()) const
	{
		std::vector<TraceResult<Dim>> res;
		trace(node, first, last, std::back_inserter(res), pred, min_dist, max_dist);
		return res;
	}

	template <
	    class ExecutionPolicy, class RandomIt1, class RandomIt2, class Predicate,
	    std::enable_if_t<pred::is_pred_v<Predicate>, bool>                        = true,
	    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
	RandomIt2 trace(ExecutionPolicy&& policy, RandomIt1 first, RandomIt1 last,
	                RandomIt2 d_first, Predicate const& pred, float min_dist = 0.0f,
	                float max_dist = std::numeric_limits<float>::max()) const
	{
		return trace(std::forward<ExecutionPolicy>(policy), index(), first, last, d_first,
		             pred, min_dist, max_dist);
	}

	template <
	    class ExecutionPolicy, class NodeType, class RandomIt1, class RandomIt2,
	    class Predicate, std::enable_if_t<is_node_type_v<NodeType>, bool> = true,
	    std::enable_if_t<pred::is_pred_v<Predicate>, bool>                        = true,
	    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
	RandomIt2 trace(ExecutionPolicy&& policy, NodeType node, RandomIt1 first,
	                RandomIt1 last, RandomIt2 d_first, Predicate pred,
	                float min_dist = 0.0f,
	                float max_dist = std::numeric_limits<float>::max()) const
	{
		using Filter = pred::Filter<Predicate>;

		Filter::init(pred, derived());

		Node n = this->node(node);
		if (!exists(n)) {
			for (; last != first; ++first, ++d_first) {
				*d_first = TraceResult<Dim>{
				    Index(), Vec<Dim, float>(std::numeric_limits<float>::quiet_NaN()), -1.0f};
			}
			return d_first;
		}

		auto center      = this->center(n);
		auto half_length = halfLength(n);

		return transform(std::forward<ExecutionPolicy>(policy), first, last, d_first,
		                 [&](Ray<Dim, ray_t> const& ray) {
			                 auto params = traceInit(ray, center, half_length);
			                 return trace(n, params, pred, min_dist, max_dist);
		                 });
	}

	template <
	    class ExecutionPolicy, class RandomIt, class Predicate,
	    std::enable_if_t<pred::is_pred_v<Predicate>, bool>                        = true,
	    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
	[[nodiscard]] std::vector<TraceResult<Dim>> trace(
	    ExecutionPolicy&& policy, RandomIt first, RandomIt last, Predicate const& pred,
	    float min_dist = 0.0f, float max_dist = std::numeric_limits<float>::max()) const
	{
		return trace(std::forward<ExecutionPolicy>(policy), index(), first, last, pred,
		             min_dist, max_dist);
	}

	template <
	    class ExecutionPolicy, class NodeType, class RandomIt, class Predicate,
	    std::enable_if_t<is_node_type_v<NodeType>, bool>                          = true,
	    std::enable_if_t<pred::is_pred_v<Predicate>, bool>                        = true,
	    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
	[[nodiscard]] std::vector<TraceResult<Dim>> trace(
	    ExecutionPolicy&& policy, NodeType node, RandomIt first, RandomIt last,
	    Predicate const& pred, float min_dist = 0.0f,
	    float max_dist = std::numeric_limits<float>::max()) const
	{
		__block std::vector<TraceResult<Dim>> res(std::distance(first, last));
		trace(std::forward<ExecutionPolicy>(policy), node, first, last, res.begin(), pred,
		      min_dist, max_dist);
		return res;
	}

	/**************************************************************************************
	|                                                                                     |
	|                                         GPU                                         |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] std::size_t gpuTreeNumBuffers() const
	{
		return Data::template gpuNumBuffers<Block>();
	}

	[[nodiscard]] WGPUBuffer gpuTreeBuffer(std::size_t index = 0) const
	{
		return Data::template gpuBuffer<Block>(index);
	}

	[[nodiscard]] std::size_t gpuTreeBufferSize(std::size_t index = 0) const
	{
		return Data::template gpuBufferSize<Block>(index);
	}

	bool gpuTreeUpdate() { return Data::template gpuUpdate<Block>(); }

	/**************************************************************************************
	|                                                                                     |
	|                                     Comparison                                      |
	|                                                                                     |
	**************************************************************************************/

	template <class Derived2, std::size_t Dim2, class Block2, class... Blocks2>
	friend bool operator==(Tree<Derived2, Dim2, Block2, Blocks2...> const& lhs,
	                       Tree<Derived2, Dim2, Block2, Blocks2...> const& rhs);

	template <class Derived2, std::size_t Dim2, class Block2, class... Blocks2>
	friend bool operator!=(Tree<Derived2, Dim2, Block2, Blocks2...> const& lhs,
	                       Tree<Derived2, Dim2, Block2, Blocks2...> const& rhs);

 protected:
	/**************************************************************************************
	|                                                                                     |
	|                                    Constructors                                     |
	|                                                                                     |
	**************************************************************************************/

	Tree(length_t leaf_node_length, depth_t num_depth_levels)
	    : Tree(Length(leaf_node_length), num_depth_levels)
	{
	}

	Tree(Length leaf_node_length, depth_t num_depth_levels)
	{
		init(leaf_node_length, num_depth_levels);

		createRoot();
	}

	Tree(Tree const&) = default;

	Tree(Tree&&) = default;

	template <class Derived2, class... Blocks2>
	Tree(Tree<Derived2, Dim, Block, Blocks2...> const& other)
	{
		// TODO: Implement
	}

	/**************************************************************************************
	|                                                                                     |
	|                                     Destructor                                      |
	|                                                                                     |
	**************************************************************************************/

	~Tree() = default;

	/**************************************************************************************
	|                                                                                     |
	|                                 Assignment operator                                 |
	|                                                                                     |
	**************************************************************************************/

	Tree& operator=(Tree const&) = default;

	Tree& operator=(Tree&&) = default;

	template <class Derived2, class... Blocks2>
	Tree& operator=(Tree<Derived2, Dim, Block, Blocks2...> const& rhs)
	{
		// TODO: Implement

		return *this;
	}

	/**************************************************************************************
	|                                                                                     |
	|                                        Init                                         |
	|                                                                                     |
	**************************************************************************************/

	void init(Length leaf_node_length, depth_t num_depth_levels)
	{
		if (minNumDepthLevels() > num_depth_levels ||
		    maxNumDepthLevels() < num_depth_levels) {
			throw std::invalid_argument("'num_depth_levels' has to be in range [" +
			                            std::to_string(+minNumDepthLevels()) + ".." +
			                            std::to_string(+maxNumDepthLevels()) + "], '" +
			                            std::to_string(+num_depth_levels) + "' was supplied.");
		}

		if (length_t(0) >= min(leaf_node_length) || !isfinite(leaf_node_length)) {
			std::stringstream ss;
			ss << leaf_node_length;
			throw std::invalid_argument(
			    "'leaf_node_length' has to be finite and greater than zero, '" + ss.str() +
			    "' was supplied.");
		}
		if (!isnormal(ldexp(leaf_node_length, num_depth_levels - 1))) {
			std::stringstream ss;
			ss << ldexp(leaf_node_length, num_depth_levels - 1);
			throw std::invalid_argument(
			    "'leaf_node_length * 2^(num_depth_levels - 1)' has to be finite and greater "
			    "than zero, '" +
			    ss.str() + "' was supplied.");
		}
		if (length_t(0) >= min(length_t(1) / ldexp(leaf_node_length, -1))) {
			std::stringstream ss;
			ss << (length_t(1) / ldexp(leaf_node_length, -1));
			throw std::invalid_argument(
			    "The reciprocal of half 'leaf_node_length' (i.e., 1 / (leaf_node_length / "
			    "2)) has to be a greater than zero, '" +
			    ss.str() + "' was supplied.");
		}

		num_depth_levels_ = num_depth_levels;
		half_max_value_   = key_t(1) << (num_depth_levels - 2);

		// For increased precision
		for (int i{}; node_half_length_.size() > i; ++i) {
			node_half_length_[i]            = ldexp(leaf_node_length, i - 1);
			node_half_length_reciprocal_[i] = length_t(1) / node_half_length_[i];
		}
	}

	/**************************************************************************************
	|                                                                                     |
	|                                         Swap                                        |
	|                                                                                     |
	**************************************************************************************/

	void swap(Tree& other)
	{
		using std::swap;
		swap(static_cast<Data&>(*this), static_cast<Data&>(other));
		swap(num_depth_levels_, other.num_depth_levels_);
		swap(half_max_value_, other.half_max_value_);
		swap(node_half_length_, other.node_half_length_);
		swap(node_half_length_reciprocal_, other.node_half_length_reciprocal_);
	}

	/**************************************************************************************
	|                                                                                     |
	|                                       Derived                                       |
	|                                                                                     |
	**************************************************************************************/

	/*!
	 * @brief Returns a reference of the derived class.
	 *
	 * @return A reference of the derived class.
	 */
	[[nodiscard]] constexpr Derived& derived() { return *static_cast<Derived*>(this); }

	/*!
	 * @brief Returns a reference of the derived class.
	 *
	 * @return A reference of the derived class.
	 */
	[[nodiscard]] constexpr Derived const& derived() const
	{
		return *static_cast<Derived const*>(this);
	}

	/**************************************************************************************
	|                                                                                     |
	|                                     Create root                                     |
	|                                                                                     |
	**************************************************************************************/

	void createRoot()
	{
		pos_t p = Data::create();
		treeBlock(p) =
		    Block(Index::NULL_POS, code(), parentCenter(center(), halfLength(), 0), length());
	}

	/**************************************************************************************
	|                                                                                     |
	|                                       Access                                        |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] Block& treeBlock(pos_t block)
	{
		assert(valid(block));

		return Data::template data<Block>(block);
	}

	[[nodiscard]] Block const& treeBlock(pos_t block) const
	{
		assert(valid(block));

		return Data::template data<Block>(block);
	}

	[[nodiscard]] Block& treeBlock(Index node) { return treeBlock(node.pos); }

	[[nodiscard]] Block const& treeBlock(Index node) const { return treeBlock(node.pos); }

	/**************************************************************************************
	|                                                                                     |
	|                                        Recurs                                       |
	|                                                                                     |
	**************************************************************************************/

	template <
	    class UpdateFun,
	    std::enable_if_t<std::is_invocable_r_v<void, UpdateFun, Index, pos_t>, bool> = true>
	void recursUpdate(Index node, UpdateFun update_f)
	{
		auto const root_depth = depth();
		for (auto d = depth(node); root_depth > d; ++d) {
			auto const children = node.pos;
			node                = parent(node);
			update_f(node, children);
		}
	}

	template <
	    class NodeFun, class BlockFun, class UpdateFun,
	    std::enable_if_t<std::is_invocable_r_v<void, NodeFun, Index>, bool>          = true,
	    std::enable_if_t<std::is_invocable_r_v<void, BlockFun, pos_t>, bool>         = true,
	    std::enable_if_t<std::is_invocable_r_v<void, UpdateFun, Index, pos_t>, bool> = true>
	void recursLeaves(Index node, NodeFun node_f, BlockFun block_f, UpdateFun update_f)
	{
		assert(valid(node));

		treeBlock(node.pos).modifiedSet(node.offset);

		if (isLeaf(node)) {
			node_f(node);
			return;
		}

		auto c = children(node);

		if (allLeaf(c)) {
			treeBlock(c).modifiedSet();
			block_f(c);
		} else {
			for (std::size_t i{}; BF > i; ++i) {
				recursLeaves(Index(c, i), node_f, block_f, update_f);
			}
		}

		update_f(node, c);
	}

	template <
	    class NodeType, class NodeFun, class BlockFun, class UpdateFun,
	    std::enable_if_t<is_node_type_v<NodeType>, bool>                             = true,
	    std::enable_if_t<std::is_invocable_r_v<void, NodeFun, Index>, bool>          = true,
	    std::enable_if_t<std::is_invocable_r_v<void, BlockFun, pos_t>, bool>         = true,
	    std::enable_if_t<std::is_invocable_r_v<void, UpdateFun, Index, pos_t>, bool> = true>
	void recursLeaves(NodeType node, NodeFun node_f, BlockFun block_f, UpdateFun update_f,
	                  bool update)
	{
		assert(valid(node));

		if constexpr (std::is_same_v<Index, remove_cvref_t<NodeType>>) {
			recursLeaves(node, node_f, block_f, update_f);
			if (update) {
				recursUpdate(node, [this, update_f](Index node, pos_t children) {
					treeBlock(node.pos).modifiedSet(node.offset);
					update_f(node, children);
				});
			} else {
				create(node);
			}
		} else {
			Index n = create(node);
			recursLeaves(n, node_f, block_f, update_f);
			if (update) {
				recursUpdate(n, update_f);
			}
		}
	}

	template <class BlockFun,
	          std::enable_if_t<std::is_invocable_r_v<void, BlockFun, pos_t>, bool> = true>
	void recursParentFirst(pos_t block, BlockFun block_f)
	{
		assert(valid(block));

		treeBlock(block).modifiedSet();

		block_f(block);

		if (allLeaf(block)) {
			return;
		}

		for (std::size_t i{}; BF > i; ++i) {
			Index node(block, i);
			if (isParent(node)) {
				recursParentFirst(children(node), block_f);
			}
		}
	}

	template <
	    class NodeType, class NodeFun, class BlockFun, class UpdateFun,
	    std::enable_if_t<is_node_type_v<NodeType>, bool>                             = true,
	    std::enable_if_t<std::is_invocable_r_v<void, NodeFun, Index>, bool>          = true,
	    std::enable_if_t<std::is_invocable_r_v<void, BlockFun, pos_t>, bool>         = true,
	    std::enable_if_t<std::is_invocable_r_v<void, UpdateFun, Index, pos_t>, bool> = true>
	void recursParentFirst(NodeType node, NodeFun node_f, BlockFun block_f,
	                       UpdateFun update_f, bool update)
	{
		assert(valid(node));

		if constexpr (std::is_same_v<Index, remove_cvref_t<NodeType>>) {
			treeBlock(node.pos).modifiedSet(node.offset);
			node_f(node);
			if (isParent(node)) {
				recursParentFirst(children(node), block_f);
			}
			if (update) {
				recursUpdate(node, [this, update_f](Index node, pos_t children) {
					treeBlock(node.pos).modifiedSet(node.offset);
					update_f(node, children);
				});
			} else {
				create(node);
			}
		} else {
			Index n = create(node);
			treeBlock(n.pos).modifiedSet(n.offset);
			node_f(n);
			if (isParent(n)) {
				recursParentFirst(children(n), block_f);
			}
			if (update) {
				recursUpdate(n, update_f);
			}
		}
	}

	/**************************************************************************************
	|                                                                                     |
	|                                        Leaf                                         |
	|                                                                                     |
	**************************************************************************************/

	/*!
	 * @brief Checks if all nodes of a block are pure leaves.
	 *
	 * @param block the block to check
	 * @return `true` if all nodes of the block are pure leaves, `false` otherwise.
	 */
	[[nodiscard]] bool allPureLeaf(pos_t block) const
	{
		assert(valid(block));

		return 0 == treeBlock(block).depth();
	}

	/*!
	 * @brief Checks if any node of a block is a pure leaves.
	 *
	 * @param block the block to check
	 * @return `true` if any node of the block is a pure leaf, `false` otherwise.
	 */
	[[nodiscard]] bool anyPureLeaf(pos_t block) const
	{
		assert(valid(block));

		return 0 == treeBlock(block).depth();
	}

	/*!
	 * @brief Checks if no nodes of a block are pure leaves.
	 *
	 * @param block the block to check
	 * @return `true` if no nodes of the block are pure leaves, `false` otherwise.
	 */
	[[nodiscard]] bool nonePureLeaf(pos_t block) const
	{
		assert(valid(block));

		return 0 != treeBlock(block).depth();
	}

	/*!
	 * @brief Checks if some nodes of a block are pure leaves, same as
	 * `anyPureLeaf(block) && !allPureLeaf(block)`.
	 *
	 * @param block the block to check
	 * @return `true` if some nodes of the block are pure leaves, `false` otherwise.
	 */
	[[nodiscard]] bool somePureLeaf(pos_t block) const
	{
		assert(valid(block));

		return false;
	}

	/*!
	 * @brief Checks if all nodes of a block are leaves.
	 *
	 * @param block the block to check
	 * @return `true` if all nodes of the block are leaves, `false` otherwise.
	 */
	[[nodiscard]] bool allLeaf(pos_t block) const
	{
		assert(valid(block));

		return std::all_of(treeBlock(block).children.begin(), treeBlock(block).children.end(),
		                   [](auto const& e) {
			                   return Index::PROCESSING_POS <=
			                          e.load(std::memory_order_relaxed);
		                   });
	}

	/*!
	 * @brief Checks if any node of a block is a leaf.
	 *
	 * @param block the block to check
	 * @return `true` if any node of the block is a leaf, `false` otherwise.
	 */
	[[nodiscard]] bool anyLeaf(pos_t block) const
	{
		assert(valid(block));

		return std::any_of(treeBlock(block).children.begin(), treeBlock(block).children.end(),
		                   [](auto const& e) {
			                   return Index::PROCESSING_POS <=
			                          e.load(std::memory_order_relaxed);
		                   });
	}

	/*!
	 * @brief Checks if no nodes of a block are leaves.
	 *
	 * @param block the block to check
	 * @return `true` if no nodes of the block are leaves, `false` otherwise.
	 */
	[[nodiscard]] bool noneLeaf(pos_t block) const
	{
		assert(valid(block));

		return std::none_of(treeBlock(block).children.begin(),
		                    treeBlock(block).children.end(), [](auto const& e) {
			                    return Index::PROCESSING_POS <=
			                           e.load(std::memory_order_relaxed);
		                    });
	}

	/*!
	 * @brief Checks if some nodes of a block are leaves, same as `anyLeaf(block) &&
	 * !allLeaf(block)`.
	 *
	 * @param block the block to check
	 * @return `true` if no nodes of the block are leaves, `false` otherwise.
	 */
	[[nodiscard]] bool someLeaf(pos_t block) const
	{
		bool leaf   = false;
		bool parent = false;
		for (auto e : children(block)) {
			leaf   = leaf || Index::PROCESSING_POS <= e;
			parent = parent || Index::PROCESSING_POS > e;
		}
		return leaf && parent;
	}

	/*!
	 * @brief Checks if all nodes of a block are parents.
	 *
	 * @param block the block to check
	 * @return `true` if all nodes of the block are parents, `false` otherwise.
	 */
	[[nodiscard]] bool allParent(pos_t block) const { return noneLeaf(block); }

	/*!
	 * @brief Checks if any node of a block is a parent.
	 *
	 * @param block the block to check
	 * @return `true` if any node of the block is a parent, `false` otherwise.
	 */
	[[nodiscard]] bool anyParent(pos_t block) const { return !allLeaf(block); }

	/*!
	 * @brief Checks if no nodes of a block are parents.
	 *
	 * @param block the block to check
	 * @return `true` if no nodes of the block are parents, `false` otherwise.
	 */
	[[nodiscard]] bool noneParent(pos_t block) const { return allLeaf(block); }

	/*!
	 * @brief Checks if some nodes of a block are parents, same as `anyParent(block) &&
	 * !allParent(block)`.
	 *
	 * @param block the block to check
	 * @return `true` if some nodes of the block are parents, `false` otherwise.
	 */
	[[nodiscard]] bool someParent(pos_t block) const { return someLeaf(block); }

	/**************************************************************************************
	|                                                                                     |
	|                                       Center                                        |
	|                                                                                     |
	**************************************************************************************/

	//
	// Child center
	//

	/*!
	 * @brief Returns the center of the `child_index`th child.
	 *
	 * @param center the center of the parent
	 * @param half_length the half length of the parent
	 * @param child_index the index of the child
	 * @return The center of the `child_index`th child.
	 */
	[[nodiscard]] static constexpr Point childCenter(Point center, Length half_length,
	                                                 offset_t child_index)
	{
		assert(BF > child_index);
		half_length /= length_t(2);
		for (std::size_t i{}; Point::size() > i; ++i) {
			center[i] += (child_index & offset_t(1u << i)) ? half_length[i] : -half_length[i];
		}
		return center;
	}

	//
	// Parent center
	//

	/*!
	 * @brief Returns the center of the parent of the node.
	 *
	 * @param center the center of the child
	 * @param half_length the half length of the child
	 * @param index the index of the child
	 * @return The center of the parent.
	 */
	[[nodiscard]] static constexpr Point parentCenter(Point center, Length half_length,
	                                                  offset_t index)
	{
		assert(BF > index);
		for (std::size_t i{}; Point::size() > i; ++i) {
			center[i] += (index & offset_t(1u << i)) ? -half_length[i] : half_length[i];
		}
		return center;
	}

	/**************************************************************************************
	|                                                                                     |
	|                                 Create/erase nodes                                  |
	|                                                                                     |
	**************************************************************************************/

	void initChildren(Index node, Block& block, pos_t children)
	{
		if constexpr (Block::HasCenter) {
			treeBlock(children).fill(node.pos, block, node.offset,
			                         isRoot(node) ? Length(0.0) : halfLength(node));
		} else {
			treeBlock(children).fill(node.pos, block, node.offset, halfLength(node));
		}
		derived().onInitChildren(node, children);

		block.children[node.offset].store(children);
	}

	void pruneChildren(Index node)
	{
		// FIXME: What memory order to use?
		pos_t children = treeBlock(node).children[node.offset].exchange(
		    Index::NULL_POS, std::memory_order_acq_rel);
		derived().onPruneChildren(node, children);
		// NOTE: Important that derived is pruned first in case they use parent code
		treeBlock(children).reset();
		Data::eraseBlock(children);
	}

	//
	// Create
	//

	pos_t createChildren(Index node)
	{
		assert(!isPureLeaf(node));

		Block& block = treeBlock(node);

		pos_t children = block.children[node.offset].load(std::memory_order_relaxed);
		assert(TreeIndex::PROCESSING_POS != children);

		if (TreeIndex::NULL_POS == children) {
			children = Data::create();
			initChildren(node, block, children);
		}

		block.modifiedSet(node.offset);

		return children;
	}

	pos_t createChildrenThreadSafe(Index node)
	{
		assert(!isPureLeaf(node));

		Block& block = treeBlock(node);

		pos_t children = block.children[node.offset].load(std::memory_order_acquire);
		if (Index::NULL_POS == children) {
			if (block.children[node.offset].compare_exchange_strong(children,
			                                                        Index::PROCESSING_POS)) {
				children = Data::createThreadSafe();
				initChildren(node, block, children);
			}
		}

		while (Index::PROCESSING_POS == children) {
			children = block.children[node.offset].load(std::memory_order_acquire);
		}

		block.modifiedSet(node.offset);

		return children;
	}

	Index createChild(Index node, offset_t child_index)
	{
		assert(0 < depth(node));
		assert(branchingFactor() > child_index);

		return Index(createChildren(node), child_index);
	}

	Index createChildThreadSafe(Index node, offset_t child_index)
	{
		assert(0 < depth(node));
		assert(branchingFactor() > child_index);

		return Index(createChildrenThreadSafe(node), child_index);
	}

	/**************************************************************************************
	|                                                                                     |
	|                                       Nearest                                       |
	|                                                                                     |
	**************************************************************************************/

	// LOOKAT: Benchmark against only returning the distance

	// TODO: Implement below correctly

	template <bool OnlyDistance = false, bool FastAsSonic = false, class ValueFun,
	          class InnerFun>
	[[nodiscard]] std::conditional_t<OnlyDistance, float, std::pair<float, Index>> nearest(
	    Index node, NearestSearchAlgorithm search_alg, ValueFun value_f, InnerFun inner_f,
	    float max_dist, float epsilon) const
	{
		// FIXME: Look at
		// assert(std::isfinite(max_dist));
		// assert(std::isfinite(epsilon));

		std::conditional_t<OnlyDistance, float, std::pair<float, Index>> closest{};
		if constexpr (OnlyDistance) {
			closest = max_dist;
		} else {
			closest.first = max_dist;
		}

		if (isParent(node) && max_dist >= inner_f(node)) {
			auto cb = children(node);
			auto cd = depth(cb);

			// if (0.0f < epsilon) {
			// switch (search_alg) {
			// 	case NearestSearchAlgorithm::DEPTH_FIRST:
			closest = nearestDepthFirst<OnlyDistance, FastAsSonic>(cb, cd, max_dist, epsilon,
			                                                       value_f, inner_f);
			// 		case NearestSearchAlgorithm::A_STAR:
			// 			closest = nearestAStar(cb, cd, max_dist, epsilon, value_f, inner_f);
			// 	}
			// } else {
			// 	switch (search_alg) {
			// 		case NearestSearchAlgorithm::DEPTH_FIRST:
			// 			closest = nearestDepthFirst(cb, cd, max_dist, value_f, inner_f);
			// 		case NearestSearchAlgorithm::A_STAR:
			// 			closest = nearestAStar(cb, cd, max_dist, value_f, inner_f);
			// 	}
			// }
		}

		if constexpr (!FastAsSonic) {
			max_dist = value_f(node);
		}
		assert(!std::isnan(max_dist));
		if constexpr (OnlyDistance) {
			return UFO_MIN(closest, max_dist);
		} else {
			return closest.first < max_dist ? closest : std::pair{max_dist, node};
		}
	}

	template <class Predicate, class ValueFun, class InnerFun,
	          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] std::pair<float, Index> nearest(Index node, Predicate pred,
	                                              NearestSearchAlgorithm search_alg,
	                                              ValueFun value_f, InnerFun inner_f,
	                                              float max_dist, float epsilon) const
	{
		using Filter = pred::Filter<Predicate>;

		Filter::init(pred, derived());

		auto wrapped_value_f = [value_f, &pred](Index node) -> float {
			return Filter::returnable(pred) ? value_f(node)
			                                : std::numeric_limits<float>::infinity();
		};

		auto wrapped_inner_f = [inner_f, &pred](Index node) -> float {
			return Filter::traversable(pred) ? inner_f(node)
			                                 : std::numeric_limits<float>::infinity();
		};

		return nearest(node, search_alg, wrapped_value_f, wrapped_inner_f, max_dist, epsilon);
	}

	template <bool OnlyDistance, bool FastAsSonic, class ValueFun, class InnerFun>
	[[nodiscard]] std::conditional_t<OnlyDistance, float, std::pair<float, Index>>
	nearestDepthFirst(pos_t block, depth_t depth, float c_dist, float epsilon,
	                  ValueFun value_f, InnerFun inner_f) const
	{
		struct StackElement {
			using Container = std::array<std::pair<float, pos_t>, BF>;
			using Iterator  = typename Container::iterator;

			Container container;
			Iterator  it;

			[[nodiscard]] constexpr float& distance() { return it->first; }

			[[nodiscard]] constexpr float const& distance() const { return it->first; }

			[[nodiscard]] constexpr pos_t& block() { return it->second; }

			[[nodiscard]] constexpr pos_t const& block() const { return it->second; }

			constexpr void start() { it = container.begin(); }

			[[nodiscard]] constexpr bool empty() { return container.end() == it; }

			[[nodiscard]] constexpr bool empty() const { return container.end() == it; }

			StackElement& operator++()
			{
				++it;
				return *this;
			}

			constexpr void sort()
			{
				if constexpr (2 == BF) {
					UFO_SORT_ASCENDING_PAIR_FIRST_2(container);
				} else if constexpr (4 == BF) {
					UFO_SORT_ASCENDING_PAIR_FIRST_4(container);
				} else if constexpr (8 == BF) {
					UFO_SORT_ASCENDING_PAIR_FIRST_8(container);
				} else if constexpr (16 == BF) {
					UFO_SORT_ASCENDING_PAIR_FIRST_16(container);
				} else {
					std::sort(container.begin(), container.end(),
					          [](auto a, auto b) { return a.first < b.first; });
				}
			}
		};

		using Stack = std::array<StackElement, maxNumDepthLevels() - 1>;

		Stack stack;
		// Since we only have one block in the beginning we set the index to `BF - 1u` (the
		// last index)
		stack[depth].it = std::prev(stack[depth].container.end());
		// The first block to go through
		stack[depth].block() = block;
		// This distance does not matter as long as it is less than `c_dist - epsilon`, since
		// we only have one block in the beginning
		stack[depth].distance() = 0.0f;

		std::conditional_t<OnlyDistance, bool, Index> c_node;

		std::array<std::conditional_t<OnlyDistance, float, std::pair<float, offset_t>>, BF> d;

		for (depth_t max_depth = depth + 1; max_depth > depth;) {
			StackElement& se = stack[depth];

			if (se.empty() || c_dist - epsilon <= se.distance()) {
				++depth;
				continue;
			}

			block = se.block();
			++se;

			StackElement& cur = stack[depth - 1u];

			cur.start();

			for (std::size_t i{}; BF > i; ++i) {
				Index node(block, i);
				cur.container[i].first = inner_f(node);
				assert(!std::isnan(cur.container[i].first));
				cur.container[i].second = children(node);

				if constexpr (!FastAsSonic) {
					if constexpr (OnlyDistance) {
						d[i] = value_f(node);
						assert(!std::isnan(d[i]));
					} else {
						d[i].first = value_f(node);
						assert(!std::isnan(d[i].first));
						d[i].second = i;
					}
				}
			}

			if constexpr (!FastAsSonic) {
				if constexpr (OnlyDistance) {
					if constexpr (2 == BF) {
						UFO_MIN_2(d);
					} else if constexpr (4 == BF) {
						UFO_MIN_4(d);
					} else if constexpr (8 == BF) {
						UFO_MIN_8(d);
					} else if constexpr (16 == BF) {
						UFO_MIN_16(d);
					} else {
						for (std::size_t i = 1; BF > i; ++i) {
							d[0] = UFO_MIN(d[0], d[i]);
						}
					}

					c_dist = c_dist <= d[0] ? c_dist : d[0];
				} else {
					if constexpr (2 == BF) {
						UFO_MIN_PAIR_FIRST_2(d);
					} else if constexpr (4 == BF) {
						UFO_MIN_PAIR_FIRST_4(d);
					} else if constexpr (8 == BF) {
						UFO_MIN_PAIR_FIRST_8(d);
					} else if constexpr (16 == BF) {
						UFO_MIN_PAIR_FIRST_16(d);
					} else {
						for (std::size_t i = 1; BF > i; ++i) {
							d[0] = UFO_MIN_PAIR_FIRST(d[0], d[i]);
						}
					}

					c_node = c_dist <= d[0].first ? c_node : Index{block, d[0].second};
					c_dist = c_dist <= d[0].first ? c_dist : d[0].first;
				}
			}

			if (1u == depth) {
				for (auto [dist, child_block] : cur.container) {
					if (c_dist <= dist + epsilon) {
						continue;
					}

					if constexpr (OnlyDistance) {
						for (offset_t i{}; BF > i; ++i) {
							d[i] = value_f(Index(child_block, i));
							assert(!std::isnan(d[i]));
						}

						if constexpr (2 == BF) {
							UFO_MIN_2(d);
						} else if constexpr (4 == BF) {
							UFO_MIN_4(d);
						} else if constexpr (8 == BF) {
							UFO_MIN_8(d);
						} else if constexpr (16 == BF) {
							UFO_MIN_16(d);
						} else {
							for (std::size_t i = 1; BF > i; ++i) {
								d[0] = UFO_MIN(d[0], d[i]);
							}
						}

						c_dist = c_dist <= d[0] ? c_dist : d[0];
					} else {
						for (offset_t i{}; BF > i; ++i) {
							d[i].first = value_f(Index(child_block, i));
							assert(!std::isnan(d[i].first));
							d[i].second = i;
						}

						if constexpr (2 == BF) {
							UFO_MIN_PAIR_FIRST_2(d);
						} else if constexpr (4 == BF) {
							UFO_MIN_PAIR_FIRST_4(d);
						} else if constexpr (8 == BF) {
							UFO_MIN_PAIR_FIRST_8(d);
						} else if constexpr (16 == BF) {
							UFO_MIN_PAIR_FIRST_16(d);
						} else {
							for (std::size_t i = 1; BF > i; ++i) {
								d[0] = UFO_MIN_PAIR_FIRST(d[0], d[i]);
							}
						}

						c_node = c_dist <= d[0].first ? c_node : Index{child_block, d[0].second};
						c_dist = c_dist <= d[0].first ? c_dist : d[0].first;
					}
				}
			} else {
				cur.sort();
				--depth;
			}
		}

		if constexpr (OnlyDistance) {
			return c_dist;
		} else {
			return {c_dist, c_node};
		}
	}

	template <class ValueFun, class InnerFun>
	[[nodiscard]] std::pair<float, Index> nearestDepthFirst(pos_t block, depth_t depth,
	                                                        float c_dist, ValueFun value_f,
	                                                        InnerFun inner_f) const
	{
		using Stack =
		    std::array<std::pair<std::size_t, std::array<std::pair<float, pos_t>, BF>>,
		               maxNumDepthLevels() - 1>;

		Stack stack;
		stack[depth].first                 = BF - 1u;
		stack[depth].second[BF - 1].first  = 0.0f;
		stack[depth].second[BF - 1].second = block;

		Index c_node;

		for (depth_t max_depth = depth + 1; max_depth > depth;) {
			auto& [idx, c] = stack[depth];

			if (BF <= idx || c_dist <= c[idx].first) {
				++depth;
				continue;
			}

			block = c[idx].second;
			++idx;

			stack[depth - 1].first = 0;
			auto& candidates       = stack[depth - 1].second;

			for (std::size_t i{}; BF > i; ++i) {
				Index node(block, i);
				candidates[i].first = inner_f(node);
				assert(!std::isnan(candidates[i].first));
				candidates[i].second = children(node);
			}

			if (1u == depth) {
				std::array<std::pair<float, offset_t>, BF> d;
				for (auto [dist, child_block] : candidates) {
					if (c_dist <= dist) {
						continue;
					}

					for (offset_t i{}; BF > i; ++i) {
						d[i].first = value_f(Index(child_block, i));
						assert(!std::isnan(d[i].first));
						d[i].second = i;
					}

					if constexpr (2 == BF) {
						UFO_MIN_PAIR_FIRST_2(d);
					} else if constexpr (4 == BF) {
						UFO_MIN_PAIR_FIRST_4(d);
					} else if constexpr (8 == BF) {
						UFO_MIN_PAIR_FIRST_8(d);
					} else if constexpr (16 == BF) {
						UFO_MIN_PAIR_FIRST_16(d);
					} else {
						for (std::size_t i = 1; BF > i; ++i) {
							d[0] = UFO_MIN_PAIR_FIRST(d[0], d[i]);
						}
					}

					c_node = c_dist <= d[0].first ? c_node : Index{child_block, d[0].second};
					c_dist = c_dist <= d[0].first ? c_dist : d[0].first;
				}
			} else {
				if constexpr (2 == BF) {
					UFO_SORT_ASCENDING_PAIR_FIRST_2(candidates);
				} else if constexpr (4 == BF) {
					UFO_SORT_ASCENDING_PAIR_FIRST_4(candidates);
				} else if constexpr (8 == BF) {
					UFO_SORT_ASCENDING_PAIR_FIRST_8(candidates);
				} else if constexpr (16 == BF) {
					UFO_SORT_ASCENDING_PAIR_FIRST_16(candidates);
				} else {
					std::sort(candidates.begin(), candidates.end(),
					          [](auto a, auto b) { return a.first < b.first; });
				}
				--depth;
			}
		}

		return {c_dist, c_node};
	}

	// template <class ValueFun, class InnerFun>
	// [[nodiscard]] std::pair<float, Index> nearestAStar(pos_t block, depth_t depth,
	//                                                    float c_dist, float epsilon,
	//                                                    ValueFun value_f,
	//                                                    InnerFun inner_f) const
	// {
	// 	struct S {
	// 		float   dist;
	// 		pos_t   block;
	// 		depth_t depth;

	// 		S(float dist, pos_t block, depth_t depth) noexcept
	// 		    : dist(dist), block(block), depth(depth)
	// 		{
	// 		}

	// 		bool operator>(S rhs) const noexcept
	// 		{
	// 			// return dist > rhs.dist;
	// 			return dist + (depth << 2) > rhs.dist + (rhs.depth << 2);
	// 		}
	// 	};

	// 	using Queue = std::priority_queue<S, std::vector<S>, std::greater<S>>;

	// 	std::vector<S> container;
	// 	container.reserve(1024);
	// 	Queue queue(std::greater<S>{}, std::move(container));
	// 	queue.emplace(0.0f, block, depth);

	// 	auto max_size = depth << 2;

	// 	Index c_node;

	// 	while (!queue.empty()) {
	// 		auto cur = queue.top();

	// 		if (c_dist + max_size - (cur.depth << 2) <= cur.dist + epsilon) {
	// 			return {c_dist, c_node};
	// 		}

	// 		if (c_dist <= cur.dist + epsilon) {
	// 			queue.pop();
	// 			continue;
	// 		}

	// 		queue.pop();

	// 		block = cur.block;
	// 		depth = cur.depth;

	// 		std::array<std::pair<float, pos_t>, BF> candidates;
	// 		for (std::size_t i{}; BF > i; ++i) {
	// 			Index node(block, i);
	// 			candidates[i].first = inner_f(node);
	// 			assert(!std::isnan(candidates[i].first));
	// 			candidates[i].second = children(node);
	// 		}

	// 		if (1u == depth) {
	// 			std::array<std::pair<float, offset_t>, BF> d;
	// 			for (auto [dist, child_block] : candidates) {
	// 				if (c_dist <= dist + epsilon) {
	// 					continue;
	// 				}

	// 				for (offset_t i{}; BF > i; ++i) {
	// 					d[i].first = value_f(Index(child_block, i));
	// 					assert(!std::isnan(d[i].first));
	// 					d[i].second = i;
	// 				}

	// 				if constexpr (2 == BF) {
	// 					UFO_MIN_PAIR_FIRST_2(d);
	// 				} else if constexpr (4 == BF) {
	// 					UFO_MIN_PAIR_FIRST_4(d);
	// 				} else if constexpr (8 == BF) {
	// 					UFO_MIN_PAIR_FIRST_8(d);
	// 				} else if constexpr (16 == BF) {
	// 					UFO_MIN_PAIR_FIRST_16(d);
	// 				} else {
	// 					for (std::size_t i = 1; BF > i; ++i) {
	// 						d[0] = UFO_MIN_PAIR_FIRST(d[0], d[i]);
	// 					}
	// 				}

	// 				c_node = c_dist <= d[0].first ? c_node : Index{child_block, d[0].second};
	// 				c_dist = c_dist <= d[0].first ? c_dist : d[0].first;
	// 			}
	// 		} else {
	// 			for (auto [dist, child_block] : candidates) {
	// 				if (c_dist <= dist + epsilon) {
	// 					continue;
	// 				}
	// 				queue.emplace(dist, child_block, depth - 1);
	// 			}
	// 		}
	// 	}

	// 	return {c_dist, c_node};
	// }

	// template <class ValueFun, class InnerFun>
	// [[nodiscard]] std::pair<float, Index> nearestAStar(pos_t block, depth_t depth,
	//                                                    float c_dist, ValueFun value_f,
	//                                                    InnerFun inner_f) const
	// {
	// 	struct S {
	// 		float   dist;
	// 		pos_t   block;
	// 		depth_t depth;

	// 		S(float dist, pos_t block, depth_t depth) noexcept
	// 		    : dist(dist), block(block), depth(depth)
	// 		{
	// 		}

	// 		bool operator>(S rhs) const noexcept
	// 		{
	// 			// return dist > rhs.dist;
	// 			return dist + (depth << 2) > rhs.dist + (rhs.depth << 2);
	// 		}
	// 	};

	// 	using Queue = std::priority_queue<S, std::vector<S>, std::greater<S>>;

	// 	std::vector<S> container;
	// 	container.reserve(1024);
	// 	Queue queue(std::greater<S>{}, std::move(container));
	// 	queue.emplace(0.0f, block, depth);

	// 	auto max_size = depth << 2;

	// 	Index c_node;

	// 	while (!queue.empty()) {
	// 		auto cur = queue.top();

	// 		if (c_dist + max_size - (cur.depth << 2) <= cur.dist) {
	// 			return {c_dist, c_node};
	// 		}

	// 		if (c_dist <= cur.dist) {
	// 			queue.pop();
	// 			continue;
	// 		}

	// 		queue.pop();

	// 		block = cur.block;
	// 		depth = cur.depth;

	// 		std::array<std::pair<float, pos_t>, BF> candidates;
	// 		for (std::size_t i{}; BF > i; ++i) {
	// 			Index node(block, i);
	// 			candidates[i].first = inner_f(node);
	// 			assert(!std::isnan(candidates[i].first));
	// 			candidates[i].second = children(node);
	// 		}

	// 		if (1u == depth) {
	// 			std::array<std::pair<float, offset_t>, BF> d;
	// 			for (auto [dist, child_block] : candidates) {
	// 				if (c_dist <= dist) {
	// 					continue;
	// 				}

	// 				for (offset_t i{}; BF > i; ++i) {
	// 					d[i].first = value_f(Index(child_block, i));
	// 					assert(!std::isnan(d[i].first));
	// 					d[i].second = i;
	// 				}

	// 				if constexpr (2 == BF) {
	// 					UFO_MIN_PAIR_FIRST_2(d);
	// 				} else if constexpr (4 == BF) {
	// 					UFO_MIN_PAIR_FIRST_4(d);
	// 				} else if constexpr (8 == BF) {
	// 					UFO_MIN_PAIR_FIRST_8(d);
	// 				} else if constexpr (16 == BF) {
	// 					UFO_MIN_PAIR_FIRST_16(d);
	// 				} else {
	// 					for (std::size_t i = 1; BF > i; ++i) {
	// 						d[0] = UFO_MIN_PAIR_FIRST(d[0], d[i]);
	// 					}
	// 				}

	// 				c_node = c_dist <= d[0].first ? c_node : Index{child_block, d[0].second};
	// 				c_dist = c_dist <= d[0].first ? c_dist : d[0].first;
	// 			}
	// 		} else {
	// 			for (auto [dist, child_block] : candidates) {
	// 				if (c_dist <= dist) {
	// 					continue;
	// 				}
	// 				queue.emplace(dist, child_block, depth - 1);
	// 			}
	// 		}
	// 	}

	// 	return {c_dist, c_node};
	// }

	/**************************************************************************************
	|                                                                                     |
	|                                        Trace                                        |
	|                                                                                     |
	**************************************************************************************/

	struct TraceParams {
		Ray<Dim, ray_t> ray;
		Point           t0;
		Point           t1;
		unsigned        a{};
	};

	struct TraceStackElement {
		Point    t0;
		Point    t1;
		Point    tm;
		unsigned cur_node;
		Node     node;

		TraceStackElement() = default;

		constexpr TraceStackElement(Node node, unsigned cur_node, Point const& t0,
		                            Point const& t1, Point const& tm)
		    : node(node), cur_node(cur_node), t0(t0), t1(t1), tm(tm)
		{
		}
	};

	template <class NodeType, std::enable_if_t<is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] TraceParams traceInit(NodeType node, Ray<Dim, ray_t> const& ray) const
	{
		return traceInit(ray, center(node), halfLength(node));
	}

	[[nodiscard]] static constexpr TraceParams traceInit(Ray<Dim, ray_t> const& ray,
	                                                     Point const&           center,
	                                                     Length half_length) noexcept
	{
		TraceParams params;
		params.ray = ray;

		for (std::size_t i{}; Dim > i; ++i) {
			float origin = 0 > ray.direction[i] ? center[i] * 2 - ray.origin[i] : ray.origin[i];

			auto a = center[i] - half_length[i] - origin;
			auto b = center[i] + half_length[i] - origin;

			// FIXME: Look at
			params.t0[i] = 0 == ray.direction[i] ? 1e+25 * a : a / std::abs(ray.direction[i]);
			params.t1[i] = 0 == ray.direction[i] ? 1e+25 * b : b / std::abs(ray.direction[i]);

			params.a |= unsigned(0 > ray.direction[i]) << i;
		}

		return params;
	}

	[[nodiscard]] static constexpr inline unsigned firstNode(Point const& tm,
	                                                         float const  t) noexcept
	{
		unsigned node = static_cast<unsigned>(tm[0] < t);
		for (unsigned i = 1; Dim > i; ++i) {
			node |= static_cast<unsigned>(tm[i] < t) << i;
		}
		return node;
	}

	[[nodiscard]] static constexpr inline unsigned newNode(unsigned cur,
	                                                       unsigned dim) noexcept
	{
		// You are at cur, you want to move along dim in positive direction
		unsigned x = 1u << dim;
		return ((cur & x) << Dim) | cur | x;
	}

	template <class Predicate, std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
	[[nodiscard]] constexpr TraceResult<Dim> trace(Node node, TraceParams const& params,
	                                               Predicate const& pred,
	                                               float const      near_clip,
	                                               float const      far_clip) const
	{
		using Filter = pred::Filter<Predicate>;

		auto returnable = [this, near_clip, far_clip, &pred](Node const& node, float min_dist,
		                                                     float max_dist) {
			return near_clip <= max_dist && far_clip >= min_dist &&
			       Filter::returnable(pred, derived(), node);
		};

		auto traversable = [this, near_clip, far_clip, &pred](
		                       Node const& node, float min_dist, float max_dist) {
			return near_clip <= max_dist && far_clip >= min_dist && isParent(node.index) &&
			       Filter::traversable(pred, derived(), node);
		};

		constexpr auto const new_node_lut = []() {
			std::array<std::array<unsigned, Dim>, BF> lut{};
			for (unsigned cur{}; BF != cur; ++cur) {
				for (unsigned dim{}; Dim != dim; ++dim) {
					lut[cur][dim] = newNode(cur, dim);
				}
			}
			return lut;
		}();

		auto t0 = params.t0;
		auto t1 = params.t1;
		auto tm = (t0 + t1) * 0.5f;
		auto a  = params.a;

		auto min_dist = max(t0);
		auto max_dist = min(t1);

		if (min_dist >= max_dist || near_clip > max_dist || far_clip < min_dist) {
			return TraceResult<Dim>{Index(),
			                        Vec<Dim, float>(std::numeric_limits<float>::quiet_NaN()),
			                        std::numeric_limits<float>::infinity()};
		} else if (returnable(node, min_dist, max_dist)) {
			float distance = std::max(near_clip, min_dist);
			return TraceResult<Dim>{
			    node.index, params.ray.origin + params.ray.direction * distance, distance};
		} else if (!traversable(node, min_dist, max_dist)) {
			return TraceResult<Dim>{Index(),
			                        Vec<Dim, float>(std::numeric_limits<float>::quiet_NaN()),
			                        std::numeric_limits<float>::infinity()};
		}

		unsigned cur_node = firstNode(tm, min_dist);

		std::array<TraceStackElement, maxNumDepthLevels()> stack;
		stack[0] = TraceStackElement{node, cur_node, t0, t1, tm};

		for (int idx{}; 0 <= idx;) {
			node     = stack[idx].node;
			cur_node = stack[idx].cur_node;
			t0       = stack[idx].t0;
			t1       = stack[idx].t1;
			tm       = stack[idx].tm;

			// We have a need for speed and we don´t need safety here since we know all the
			// nodes exist and are valid
			node.code  = child(node.code, cur_node ^ a);
			node.index = child(node.index, cur_node ^ a);

			for (unsigned i{}; Dim > i; ++i) {
				t0[i] = (cur_node & (1u << i)) ? tm[i] : t0[i];
				t1[i] = (cur_node & (1u << i)) ? t1[i] : tm[i];
			}

			stack[idx].cur_node = new_node_lut[cur_node][minIndex(t1)];
			idx -= BF <= stack[idx].cur_node;

			min_dist = max(t0);
			max_dist = min(t1);

			if (returnable(node, min_dist, max_dist)) {
				float distance = std::max(near_clip, min_dist);
				return TraceResult<Dim>{
				    node.index, params.ray.origin + params.ray.direction * distance, distance};
			} else if (!traversable(node, min_dist, max_dist)) {
				continue;
			}

			tm = (t0 + t1) * 0.5f;

			unsigned cur_node = firstNode(tm, min_dist);

			stack[++idx] = TraceStackElement{node, cur_node, t0, t1, tm};
		}

		return TraceResult<Dim>{Index(),
		                        Vec<Dim, float>(std::numeric_limits<float>::quiet_NaN()),
		                        std::numeric_limits<float>::infinity()};
	}

 private:
	// The number of depth levels
	depth_t num_depth_levels_;
	// Half the maximum key value the tree can store
	key_t half_max_value_;

	// Stores the node half length at a given depth, where the index is the depth
	std::array<Length, maxNumDepthLevels() + 1> node_half_length_;
	// Reciprocal of the node half length at a given depth, where the index is the depth
	std::array<Length, maxNumDepthLevels() + 1> node_half_length_reciprocal_;
};

template <class Derived, std::size_t Dim, class Block, class... Blocks>
bool operator==(Tree<Derived, Dim, Block, Blocks...> const& lhs,
                Tree<Derived, Dim, Block, Blocks...> const& rhs)
{
	return lhs.num_depth_levels_ == rhs.num_depth_levels_ &&
	       lhs.node_half_length_ == rhs.node_half_length_ &&
	       std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
	                  [&lhs, &rhs](TreeNode<Dim> const& a, TreeNode<Dim> const& b) {
		                  return 0 != a.index.offset ||
		                         (a.code == b.code &&
		                          lhs.treeBlock(a.index) == rhs.treeBlock(b.index) &&
		                          ((lhs.template data<Blocks>(a.index.pos) ==
		                            rhs.template data<Blocks>(b.index.pos)) &&
		                           ...));
	                  });
}

template <class Derived, std::size_t Dim, class Block, class... Blocks>
bool operator!=(Tree<Derived, Dim, Block, Blocks...> const& lhs,
                Tree<Derived, Dim, Block, Blocks...> const& rhs)
{
	return !(lhs == rhs);
}

template <class Derived, std::size_t Dim, class Block, class... Blocks>
void swap(Tree<Derived, Dim, Block, Blocks...>& lhs,
          Tree<Derived, Dim, Block, Blocks...>& rhs)
{
	lhs.swap(rhs);
}
}  // namespace ufo

#endif  // UFO_CONTAINER_TREE_HPP