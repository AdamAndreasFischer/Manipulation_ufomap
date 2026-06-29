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

#ifndef UFO_MAP_OCCUPANCY_DIRECTIONAL_MAP_HPP
#define UFO_MAP_OCCUPANCY_DIRECTIONAL_MAP_HPP

// UFO
#include <ufo/compute/compute.hpp>
#include <ufo/container/tree/container.hpp>
#include <ufo/container/tree/tree.hpp>
#include <ufo/map/block.hpp>
#include <ufo/map/occupancy/block.hpp>
#include <ufo/map/occupancy/map.hpp>
#include <ufo/map/occupancy/predicate.hpp>
#include <ufo/map/occupancy/propagation_criteria.hpp>
#include <ufo/map/occupancy/state.hpp>
#include <ufo/map/type.hpp>
#include <ufo/math/math.hpp>
#include <ufo/math/transform3.hpp>
#include <ufo/utility/bit_set.hpp>
#include <ufo/utility/io/buffer.hpp>
#include <ufo/utility/macros.hpp>
#include <ufo/utility/type_traits.hpp>

// STL
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string_view>
#include <type_traits>

namespace ufo
{
// TODO: Store only index in block and store the values separately with index 0 being
// logit 0 node (so all can point to that).

template <class Derived, class Tree>
class OccupancyMapFull<Derived, Tree, true>
{
 private:
	template <class Derived2, class Tree2, bool Directional2>
	friend class OccupancyMapFull;

	static constexpr auto const BF  = Tree::branchingFactor();
	static constexpr auto const Dim = Tree::dimensions();

	using Block = OccupancyBlock<Dim, BF, true>;

	using value_type = typename Block::value_type;

 public:
	/**************************************************************************************
	|                                                                                     |
	|                                        Tags                                         |
	|                                                                                     |
	**************************************************************************************/

	static constexpr MapType const Type = MapType::OCCUPANCY;

	using Index    = typename Tree::Index;
	using Node     = typename Tree::Node;
	using Code     = typename Tree::Code;
	using Key      = typename Tree::Key;
	using Point    = typename Tree::Point;
	using Coord    = typename Tree::Coord;
	using coord_t  = typename Tree::coord_t;
	using depth_t  = typename Tree::depth_t;
	using offset_t = typename Tree::offset_t;
	using length_t = typename Tree::length_t;
	using pos_t    = typename Tree::pos_t;

	using logit_type     = typename Block::logit_type;
	using occupancy_type = float;
	using Direction      = Vec<Dim, float>;
	using Normal         = Direction;

 public:
	/**************************************************************************************
	|                                                                                     |
	|                                        Info                                         |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] static constexpr bool occupancyDirectional() { return true; }

	/**************************************************************************************
	|                                                                                     |
	|                                       Access                                        |
	|                                                                                     |
	**************************************************************************************/

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] logit_type occupancyLogit(NodeType const&  node,
	                                        Direction const& view_dir) const
	{
		Index       n = derived().index(node);
		auto const& v = occupancyBlock(n.pos)[n.offset].logit;

		auto const indices = dirIndices(view_dir);
		auto const weights = dirWeights(view_dir);

		logit_type res{};
		for (unsigned i{}; Dim > i; ++i) {
			res += v[indices[i]] * weights[i];
		}

		return res;
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] occupancy_type occupancy(NodeType const&  node,
	                                       Direction const& view_dir) const
	{
		return probabilityToLogit(occupancyLogit(node, view_dir));
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] logit_type occupancyLogit(NodeType const& node) const
	{
		Index       n = derived().index(node);
		auto const& v = occupancyBlock(n.pos)[n.offset].logit;

		// TODO: What should this return? Max? Sum?
		return *std::max_element(v.begin(), v.end());
		// return std::reduce(v.begin(), v.end());
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] occupancy_type occupancy(NodeType const& node) const
	{
		return probabilityToLogit(occupancyLogit(node));
	}

	/**************************************************************************************
	|                                                                                     |
	|                                      Modifiers                                      |
	|                                                                                     |
	**************************************************************************************/

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancySetLogit(NodeType const& node, Direction const& view_dir,
	                       Normal const& surface_normal, logit_type value,
	                       bool propagate = true)
	{
		auto const indices = dirIndices(-surface_normal);
		auto const weights = dirWeights(view_dir, surface_normal);

		value_type v{};
		for (unsigned i{}; Dim > i; ++i) {
			v.logit[indices[i]] = value * weights[i];
		}

		for (auto const& l : v.logit) {
			v.contains_unknown  = v.contains_unknown || occupancyUnknownLogit(l);
			v.contains_free     = v.contains_free || occupancyFreeLogit(l);
			v.contains_occupied = v.contains_occupied || occupancyOccupiedLogit(l);
		}

		auto node_f = [this, v](Index node) { occupancyBlock(node.pos)[node.offset] = v; };

		auto block_f = [this, v](pos_t pos) { occupancyBlock(pos).fill(v); };

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancySetLogit(NodeType const& node, Direction const& view_dir,
	                       logit_type value, bool propagate = true)
	{
		occupancySetLogit(node, view_dir, -view_dir, value, propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancySet(NodeType const& node, Direction const& view_dir,
	                  Normal const& surface_normal, occupancy_type value,
	                  bool propagate = true)
	{
		occupancySetLogit(node, view_dir, surface_normal, probabilityToLogit(value),
		                  propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancySet(NodeType const& node, Direction const& view_dir, occupancy_type value,
	                  bool propagate = true)
	{
		occupancySet(node, view_dir, -view_dir, value, propagate);
	}

	/**
	 * @brief
	 *
	 * @note If `NodeType` is `Index`, only propagates up to `node` and it does not set
	 * modified if propagation is `false`.
	 *
	 * @tparam NodeType Should be of type `Index`, `Node`, `Code`, `Key`, `Coord`, or
	 * `Point`
	 * @param node
	 * @param value
	 * @param propagate
	 */
	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancySetLogit(NodeType const& node, logit_type value, bool propagate = true)
	{
		value_type v{createArray<2 * Dim>(value), occupancyUnknownLogit(value),
		             occupancyFreeLogit(value), occupancyOccupiedLogit(value)};

		auto node_f = [this, v](Index node) { occupancyBlock(node.pos)[node.offset] = v; };

		auto block_f = [this, v](pos_t pos) { occupancyBlock(pos).fill(v); };

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancySet(NodeType const& node, occupancy_type value, bool propagate = true)
	{
		occupancySetLogit(node, probabilityToLogit(value), propagate);
	}

	//
	// Update logit
	//

	template <
	    class NodeType, class UnaryOp,
	    std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool>           = true,
	    std::enable_if_t<std::is_invocable_r_v<logit_type, UnaryOp, Index>, bool> = true>
	void occupancyUpdateLogit(NodeType const& node, Direction const& view_dir,
	                          Normal const& surface_normal, UnaryOp unary_op,
	                          bool propagate = true)
	{
		// TODO: What should this function do?

		auto const indices = dirIndices(view_dir);
		auto const weights = dirWeights(view_dir, surface_normal);

		auto node_f = [this, unary_op](Index node) {
			logit_type const v                    = unary_op(node);
			occupancyBlock(node.pos)[node.offset] = value_type{
			    v, occupancyUnknownLogit(v), occupancyFreeLogit(v), occupancyOccupiedLogit(v)};
		};

		auto block_f = [this, node_f](pos_t pos) {
			for (unsigned i{}; BF > i; ++i) {
				node_f(Index(pos, i));
			}
		};

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	template <
	    class NodeType, class UnaryOp,
	    std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool>           = true,
	    std::enable_if_t<std::is_invocable_r_v<logit_type, UnaryOp, Index>, bool> = true>
	void occupancyUpdateLogit(NodeType const& node, Direction const& view_dir,
	                          UnaryOp unary_op, bool propagate = true)
	{
		occupancyUpdateLogit(node, view_dir, -view_dir, unary_op, propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancyUpdateLogit(NodeType const& node, Direction const& view_dir,
	                          Normal const& surface_normal, logit_type change,
	                          bool propagate = true)
	{
		// TODO: Implement
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancyUpdateLogit(NodeType const& node, Direction const& view_dir,
	                          logit_type change, bool propagate = true)
	{
		occupancyUpdateLogit(node, view_dir, -view_dir, change, propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancyUpdateLogit(NodeType const& node, Direction const& view_dir,
	                          Normal const& surface_normal, logit_type change,
	                          logit_type min_clamp_thres, logit_type max_clamp_thres,
	                          bool propagate = true)
	{
		// TODO: Implement
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancyUpdateLogit(NodeType const& node, Direction const& view_dir,
	                          logit_type change, logit_type min_clamp_thres,
	                          logit_type max_clamp_thres, bool propagate = true)
	{
		occupancyUpdateLogit(node, view_dir, -view_dir, change, min_clamp_thres,
		                     max_clamp_thres, propagate);
	}

	template <
	    class NodeType, class UnaryOp,
	    std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool>           = true,
	    std::enable_if_t<std::is_invocable_r_v<logit_type, UnaryOp, Index>, bool> = true>
	void occupancyUpdateLogit(NodeType const& node, UnaryOp unary_op, bool propagate = true)
	{
		// TODO: Implement

		auto node_f = [this, unary_op](Index node) {
			logit_type const v                    = unary_op(node);
			occupancyBlock(node.pos)[node.offset] = value_type{
			    v, occupancyUnknownLogit(v), occupancyFreeLogit(v), occupancyOccupiedLogit(v)};
		};

		auto block_f = [this, node_f](pos_t pos) {
			for (unsigned i{}; BF > i; ++i) {
				node_f(Index(pos, i));
			}
		};

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancyUpdateLogit(NodeType const& node, logit_type change,
	                          bool propagate = true)
	{
		// TODO: Implement

		occupancyUpdateLogit(
		    node, [this, change](Index node) { return occupancyLogit(node) + change; },
		    propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancyUpdateLogit(NodeType const& node, logit_type change,
	                          logit_type min_clamp_thres, logit_type max_clamp_thres,
	                          bool propagate = true)
	{
		// TODO: Implement

		occupancyUpdateLogit(
		    node,
		    [this, change, min_clamp_thres, max_clamp_thres](Index node) {
			    return std::clamp(occupancyLogit(node) + change, min_clamp_thres,
			                      max_clamp_thres);
		    },
		    propagate);
	}

	//
	// Update probability
	//

	template <class NodeType, class UnaryOp,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true,
	          std::enable_if_t<std::is_invocable_r_v<occupancy_type, UnaryOp, Index>,
	                           bool>                                          = true>
	void occupancyUpdate(NodeType const& node, Direction const& view_dir,
	                     Normal const& surface_normal, UnaryOp unary_op,
	                     bool propagate = true)
	{
		occupancyUpdateLogit(
		    node, view_dir, surface_normal,
		    [this, unary_op](Index node) { return probabilityToLogit(unary_op(node)); },
		    propagate);
	}

	template <class NodeType, class UnaryOp,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true,
	          std::enable_if_t<std::is_invocable_r_v<occupancy_type, UnaryOp, Index>,
	                           bool>                                          = true>
	void occupancyUpdate(NodeType const& node, Direction const& view_dir, UnaryOp unary_op,
	                     bool propagate = true)
	{
		occupancyUpdate(node, view_dir, -view_dir, unary_op, propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancyUpdate(NodeType const& node, Direction const& view_dir,
	                     Normal const& surface_normal, occupancy_type change,
	                     bool propagate = true)
	{
		occupancyUpdateLogit(node, view_dir, surface_normal, probabilityToLogit(change),
		                     propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancyUpdate(NodeType const& node, Direction const& view_dir,
	                     occupancy_type change, bool propagate = true)
	{
		occupancyUpdate(node, view_dir, -view_dir, change, propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancyUpdate(NodeType const& node, Direction const& view_dir,
	                     Normal const& surface_normal, occupancy_type change,
	                     occupancy_type min_clamp_thres, occupancy_type max_clamp_thres,
	                     bool propagate = true)
	{
		occupancyUpdateLogit(node, view_dir, surface_normal, probabilityToLogit(change),
		                     probabilityToLogit(min_clamp_thres),
		                     probabilityToLogit(max_clamp_thres), propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancyUpdate(NodeType const& node, Direction const& view_dir,
	                     occupancy_type change, occupancy_type min_clamp_thres,
	                     occupancy_type max_clamp_thres, bool propagate = true)
	{
		occupancyUpdate(node, view_dir, -view_dir, change, min_clamp_thres, max_clamp_thres,
		                propagate);
	}

	template <class NodeType, class UnaryOp,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true,
	          std::enable_if_t<std::is_invocable_r_v<occupancy_type, UnaryOp, Index>,
	                           bool>                                          = true>
	void occupancyUpdate(NodeType const& node, UnaryOp unary_op, bool propagate = true)
	{
		// TODO: Implement

		occupancyUpdateLogit(
		    node, [this, unary_op](Index node) { return probabilityToLogit(unary_op(node)); },
		    propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancyUpdate(NodeType const& node, occupancy_type change, bool propagate = true)
	{
		// TODO: Implement

		occupancyUpdateLogit(node, probabilityToLogit(change), propagate);
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void occupancyUpdate(NodeType const& node, occupancy_type change,
	                     occupancy_type min_clamp_thres, occupancy_type max_clamp_thres,
	                     bool propagate = true)
	{
		// TODO: Implement

		occupancyUpdateLogit(node, probabilityToLogit(change),
		                     probabilityToLogit(min_clamp_thres),
		                     probabilityToLogit(max_clamp_thres), propagate);
	}

	/**************************************************************************************
	|                                                                                     |
	|                                       Lookup                                        |
	|                                                                                     |
	**************************************************************************************/

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] OccupancyState occupancyState(NodeType const&  node,
	                                            Direction const& dir) const
	{
		// Avoid using `occupancyUnknown` here since it calls both `occupancyFree` and
		// `occupancyOccupied`
		auto const occ = occupancyLogit(node, dir);
		if (occupancyOccupiedLogit(occ)) {
			return OccupancyState::OCCUPIED;
		} else if (occupancyFreeLogit(occ)) {
			return OccupancyState::FREE;
		} else {
			return OccupancyState::UNKNOWN;
		}
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] OccupancyState occupancyState(NodeType const& node) const
	{
		// Avoid using `occupancyUnknown` here since it calls both `occupancyFree` and
		// `occupancyOccupied`
		auto const occ = occupancyLogit(node);
		if (occupancyOccupiedLogit(occ)) {
			return OccupancyState::OCCUPIED;
		} else if (occupancyFreeLogit(occ)) {
			return OccupancyState::FREE;
		} else {
			return OccupancyState::UNKNOWN;
		}
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool occupancyContainsState(NodeType const& node,
	                                          OccupancyState  state) const
	{
		Index n = derived().index(node);
		switch (state) {
			case OccupancyState::UNKNOWN: return containsUnknown(n);
			case OccupancyState::FREE: return containsFree(n);
			case OccupancyState::OCCUPIED: return containsOccupied(n);
		}
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool occupancyUnknown(NodeType const& node, Direction const& dir) const
	{
		return occupancyUnknownLogit(occupancyLogit(node, dir));
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool occupancyUnknown(NodeType const& node) const
	{
		return occupancyUnknownLogit(occupancyLogit(node));
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool occupancyFree(NodeType const& node, Direction const& dir) const
	{
		return occupancyFreeLogit(occupancyLogit(node, dir));
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool occupancyFree(NodeType const& node) const
	{
		return occupancyFreeLogit(occupancyLogit(node));
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool occupancyOccupied(NodeType const& node, Direction const& dir) const
	{
		return occupancyOccupiedLogit(occupancyLogit(node, dir));
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool occupancyOccupied(NodeType const& node) const
	{
		return occupancyOccupiedLogit(occupancyLogit(node));
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool containsUnknown(NodeType const& node) const
	{
		Index n = derived().index(node);
		return occupancyBlock(n.pos)[n.offset].contains_unknown;
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool containsFree(NodeType const& node) const
	{
		Index n = derived().index(node);
		return occupancyBlock(n.pos)[n.offset].contains_free;
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] bool containsOccupied(NodeType const& node) const
	{
		Index n = derived().index(node);
		return occupancyBlock(n.pos)[n.offset].contains_occupied;
	}

	//
	// Sensor model
	//

	[[nodiscard]] constexpr logit_type occupiedThresLogit() const noexcept
	{
		return occupied_thres_logit_;
	}

	[[nodiscard]] constexpr occupancy_type occupiedThres() const noexcept
	{
		return logitToProbability(occupiedThresLogit());
	}

	[[nodiscard]] constexpr logit_type freeThresLogit() const noexcept
	{
		return free_thres_logit_;
	}

	[[nodiscard]] constexpr occupancy_type freeThres() const noexcept
	{
		return logitToProbability(freeThresLogit());
	}

	//
	// Set sensor model
	//

	// FIXME: Look at
	void occupancySetThresLogit(logit_type occupied_thres, logit_type free_thres,
	                            bool propagate = true)
	{
		// FIXME: Should add a warning that these are very computational expensive to
		// call since the whole tree has to be updated

		occupied_thres_logit_ = occupied_thres;
		free_thres_logit_     = free_thres;

		// TODO: Implement

		// derived().setModified();

		// if (propagate) {
		// 	derived().propagateModified();
		// }
	}

	void occupancySetThres(occupancy_type occupied_thres, occupancy_type free_thres,
	                       bool propagate = true)
	{
		// FIXME: Should add a warning that these are very computational expensive to
		// call since the whole tree has to be updated

		occupancySetThresLogit(probabilityToLogit(occupied_thres),
		                       probabilityToLogit(free_thres), propagate);
	}

	//
	// Propagation criteria
	//

	[[nodiscard]] constexpr OccupancyPropagationCriteria occupancyPropagationCriteria()
	    const noexcept
	{
		return prop_criteria_;
	}

	void occupancySetPropagationCriteria(OccupancyPropagationCriteria prop_criteria,
	                                     bool                         propagate = true)
	{
		if (prop_criteria_ == prop_criteria) {
			return;
		}

		prop_criteria_ = prop_criteria;

		// Set all inner nodes to modified
		// FIXME: Possible to optimize this to only set the ones with children
		derived().setModified();

		if (propagate) {
			derived().propagateModified();
		}
	}

	/**************************************************************************************
	|                                                                                     |
	|                                         GPU                                         |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] std::size_t gpuOccupancyNumBuffers() const
	{
		return derived().template gpuNumBuffers<Block>();
	}

	[[nodiscard]] WGPUBuffer gpuOccupancyBuffer(std::size_t index = 0) const
	{
		return derived().template gpuBuffer<Block>(index);
	}

	[[nodiscard]] std::size_t gpuOccupancyBufferSize(std::size_t index = 0) const
	{
		return derived().template gpuBufferSize<Block>(index);
	}

	bool gpuOccupancyUpdate() { return derived().template gpuUpdate<Block>(); }

 protected:
	/**************************************************************************************
	|                                                                                     |
	|                                    Constructors                                     |
	|                                                                                     |
	**************************************************************************************/

	OccupancyMapFull() { onInitRoot(); }

	OccupancyMapFull(OccupancyMapFull const& other) = default;

	OccupancyMapFull(OccupancyMapFull&& other) = default;

	template <class Derived2, class Tree2, bool Directional2>
	OccupancyMapFull(OccupancyMapFull<Derived2, Tree2, Directional2> const& other)
	    : occupied_thres_logit_(other.occupied_thres_logit_)
	    , free_thres_logit_(other.free_thres_logit_)
	    , prop_criteria_(other.prop_criteria_)
	{
	}

	/**************************************************************************************
	|                                                                                     |
	|                                     Destructor                                      |
	|                                                                                     |
	**************************************************************************************/

	~OccupancyMapFull() = default;

	/**************************************************************************************
	|                                                                                     |
	|                                 Assignment operator                                 |
	|                                                                                     |
	**************************************************************************************/

	OccupancyMapFull& operator=(OccupancyMapFull const& rhs) = default;

	OccupancyMapFull& operator=(OccupancyMapFull&& rhs) = default;

	template <class Derived2, class Tree2, bool Directional2>
	OccupancyMapFull& operator=(OccupancyMapFull<Derived2, Tree2, Directional2> const& rhs)
	{
		occupied_thres_logit_ = rhs.occupied_thres_logit_;
		free_thres_logit_     = rhs.free_thres_logit_;
		prop_criteria_        = rhs.prop_criteria_;
		return *this;
	}

	/**************************************************************************************
	|                                                                                     |
	|                                        Swap                                         |
	|                                                                                     |
	**************************************************************************************/

	void swap(OccupancyMapFull& other) noexcept
	{
		std::swap(occupied_thres_logit_, other.occupied_thres_logit_);
		std::swap(free_thres_logit_, other.free_thres_logit_);
		std::swap(prop_criteria_, other.prop_criteria_);
	}

	/**************************************************************************************
	|                                                                                     |
	|                                       Derived                                       |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] constexpr Derived& derived() { return *static_cast<Derived*>(this); }

	[[nodiscard]] constexpr Derived const& derived() const
	{
		return *static_cast<Derived const*>(this);
	}

	/**************************************************************************************
	|                                                                                     |
	|                                        Block                                        |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] Block& occupancyBlock(pos_t pos)
	{
		return derived().template data<Block>(pos);
	}

	[[nodiscard]] Block const& occupancyBlock(pos_t pos) const
	{
		return derived().template data<Block>(pos);
	}

	/**************************************************************************************
	|                                                                                     |
	|                                     Directional                                     |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] constexpr unsigned dirIndex(Direction const& view_dir, unsigned i) const
	{
		assert(isNormalized(view_dir));

		return 2u * i + (view_dir[i] < 0.0f ? 0u : 1u);
	}

	[[nodiscard]] std::array<unsigned, Dim> dirIndices(Direction const& view_dir) const
	{
		std::array<unsigned, Dim> indices;
		for (unsigned i{}; Dim > i; ++i) {
			indices[i] = dirIndex(view_dir, i);
		}
		return indices;
	}

	[[nodiscard]] Vec<Dim, float> dirWeights(Direction const& view_dir) const
	{
		assert(isNormalized(view_dir));

		auto const a = abs(view_dir);
		return a / sum(a);
	}

	[[nodiscard]] Vec<Dim, float> dirWeights(Direction const& view_dir,
	                                         Normal const&    surface_normal) const
	{
		assert(isNormalized(view_dir));
		assert(isNormalized(surface_normal));

		float const w = std::clamp(-dot(view_dir, surface_normal), 0.0f, 1.0f);

		// return {w, w, w};
		return dirWeights(surface_normal) * w;
	}

	/**************************************************************************************
	|                                                                                     |
	|                              Functions Derived expects                              |
	|                                                                                     |
	**************************************************************************************/

	void onInitRoot()
	{
		logit_type v{};
		occupancyBlock(0)[0] = value_type{createArray<2 * Dim>(v), occupancyUnknownLogit(v),
		                                  occupancyFreeLogit(v), occupancyOccupiedLogit(v)};
	}

	void onInitChildren(Index node, pos_t children)
	{
		occupancyBlock(children).fill(occupancyBlock(node.pos)[node.offset]);
	}

	void onPropagateChildren(Index node, pos_t children)
	{
		// TODO: Implement

		auto&       o   = occupancyBlock(node.pos)[node.offset];
		auto const& ocb = occupancyBlock(children);

		o.contains_unknown  = false;
		o.contains_free     = false;
		o.contains_occupied = false;
		switch (prop_criteria_) {
			case OccupancyPropagationCriteria::MAX:
				o.logit = std::numeric_limits<logit_type>::lowest();
				for (auto child : ocb.data) {
					o.logit             = UFO_MAX(o.logit, child.logit);
					o.contains_unknown  = o.contains_unknown || child.contains_unknown;
					o.contains_free     = o.contains_free || child.contains_free;
					o.contains_occupied = o.contains_occupied || child.contains_occupied;
				}
				break;
			case OccupancyPropagationCriteria::MIN:
				o.logit = std::numeric_limits<logit_type>::max();
				for (auto child : ocb.data) {
					o.logit             = UFO_MIN(o.logit, child.logit);
					o.contains_unknown  = o.contains_unknown || child.contains_unknown;
					o.contains_free     = o.contains_free || child.contains_free;
					o.contains_occupied = o.contains_occupied || child.contains_occupied;
				}
				break;
			case OccupancyPropagationCriteria::MEAN: {
				o.logit = {};
				for (auto child : ocb.data) {
					o.logit += child.logit;
					o.contains_unknown  = o.contains_unknown || child.contains_unknown;
					o.contains_free     = o.contains_free || child.contains_free;
					o.contains_occupied = o.contains_occupied || child.contains_occupied;
				}
				o.logit /= BF;
				break;
			}
			case OccupancyPropagationCriteria::ONLY_INDICATORS: {
				for (auto child : ocb.data) {
					o.contains_unknown  = o.contains_unknown || child.contains_unknown;
					o.contains_free     = o.contains_free || child.contains_free;
					o.contains_occupied = o.contains_occupied || child.contains_occupied;
				}
				break;
			}
			case OccupancyPropagationCriteria::NONE: return;
		}
	}

	[[nodiscard]] bool onIsPrunable(pos_t block) const
	{
		// TODO: Implement

		using std::begin;
		using std::end;
		return std::all_of(begin(occupancyBlock(block)) + 1, end(occupancyBlock(block)),
		                   [v = occupancyBlock(block)[0]](auto const& e) { return v == e; });
	}

	void onPruneChildren(Index node, pos_t /* children */)
	{
		// TODO: Implement

		// Ensure that the indicators of `node` are correct
		auto&      o = occupancyBlock(node.pos)[node.offset];
		logit_type v = o.logit;
		o            = value_type{v, occupancyUnknownLogit(v), occupancyFreeLogit(v),
                   occupancyOccupiedLogit(v)};
	}

	[[nodiscard]] std::size_t onSerializedSize(
	    std::vector<std::pair<pos_t, BitSet<BF>>> const& /* nodes */,
	    std::size_t num_nodes) const
	{
		// Direction + num_nodes * logit_type
		return sizeof(std::uint8_t) + num_nodes * sizeof(std::array<logit_type, 2 * Dim>);
	}

	void onRead(ReadBuffer& in, std::vector<std::pair<pos_t, BitSet<BF>>> const& nodes)
	{
		// TODO: Implement

		std::uint8_t directional;
		in.read(directional);

		if (directional) {
			// TODO: Implement
		} else {
			for (auto [block, offset] : nodes) {
				auto& ob = occupancyBlock(block);

				if (offset.all()) {
					for (std::size_t i{}; BF > i; ++i) {
						logit_type v;
						in.read(v);
						ob[i] = value_type{v, occupancyUnknownLogit(v), occupancyFreeLogit(v),
						                   occupancyOccupiedLogit(v)};
					}
				} else {
					for (std::size_t i{}; BF > i; ++i) {
						if (offset[i]) {
							logit_type v;
							in.read(v);
							ob[i] = value_type{v, occupancyUnknownLogit(v), occupancyFreeLogit(v),
							                   occupancyOccupiedLogit(v)};
						}
					}
				}
			}
		}
	}

	void onWrite(WriteBuffer&                                     out,
	             std::vector<std::pair<pos_t, BitSet<BF>>> const& nodes) const
	{
		// TODO: Implement

		std::uint8_t directional = true;
		out.write(directional);

		for (auto [block, offset] : nodes) {
			auto const& ob = occupancyBlock(block);

			if (offset.all()) {
				for (auto const& x : ob) {
					out.write(x.logit);
				}
			} else {
				for (std::size_t i{}; BF > i; ++i) {
					if (offset[i]) {
						out.write(ob[i].logit);
					}
				}
			}
		}
	}

	void onDotFile(std::ostream& out, Index node) const
	{
		out << "Occupancy: " << occupancy(node);
	}

	//
	// Is
	//

	[[nodiscard]] constexpr bool occupancyUnknownLogit(logit_type logit) const noexcept
	{
		return !occupancyFreeLogit(logit) && !occupancyOccupiedLogit(logit);
	}

	[[nodiscard]] constexpr bool occupancyFreeLogit(logit_type logit) const noexcept
	{
		return freeThresLogit() > logit;
	}

	[[nodiscard]] constexpr bool occupancyOccupiedLogit(logit_type logit) const noexcept
	{
		return occupiedThresLogit() < logit;
	}

 private:
	logit_type occupied_thres_logit_{};  // Threshold for occupied
	logit_type free_thres_logit_{};      // Threshold for free

	// Propagation criteria
	OccupancyPropagationCriteria prop_criteria_ = OccupancyPropagationCriteria::MAX;
};

template <class Derived, class Tree>
using OccupancyDirectionalMap = OccupancyMapFull<Derived, Tree, true>;

template <std::size_t Dim, std::size_t BF>
struct map_block<OccupancyDirectionalMap, Dim, BF> {
	using type = OccupancyBlock<Dim, BF, true>;
};
}  // namespace ufo

#endif  // UFO_MAP_OCCUPANCY_MAP_HPP