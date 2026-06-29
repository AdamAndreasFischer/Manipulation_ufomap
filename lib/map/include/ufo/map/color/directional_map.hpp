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

#ifndef UFO_MAP_COLOR_DIRECTIONAL_MAP_HPP
#define UFO_MAP_COLOR_DIRECTIONAL_MAP_HPP

// UFO
#include <ufo/compute/compute.hpp>
#include <ufo/map/block.hpp>
#include <ufo/map/color/block.hpp>
#include <ufo/map/color/map.hpp>
#include <ufo/map/type.hpp>
#include <ufo/math/vec.hpp>
#include <ufo/utility/bit_set.hpp>
#include <ufo/utility/enum.hpp>
#include <ufo/utility/io/buffer.hpp>
#include <ufo/vision/color.hpp>

// STL
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>

namespace ufo
{
template <class Derived, class Tree, ColorType CT>
class ColorMap<Derived, Tree, CT, true>
{
	template <class Derived2, class Tree2, ColorType CT2, bool Directional2>
	friend class ColorMap;

	static constexpr auto const BF  = Tree::branchingFactor();
	static constexpr auto const Dim = Tree::dimensions();

	using Block = ColorBlock<Dim, BF, CT, true>;

	using value_type = typename Block::value_type;

 public:
	/**************************************************************************************
	|                                                                                     |
	|                                        Tags                                         |
	|                                                                                     |
	**************************************************************************************/

	static constexpr MapType const Type = MapType::COLOR;

	// Container
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

	using color_type = typename Block::color_type;
	using Direction  = Vec<Dim, float>;
	using Normal     = Direction;

 public:
	/**************************************************************************************
	|                                                                                     |
	|                                        Info                                         |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] static constexpr bool colorDirectional() { return true; }

	[[nodiscard]] static constexpr bool colorHasAlpha() { return color::has_alpha_v<CT>; }

	[[nodiscard]] static constexpr bool colorHasWeight() { return color::has_weight_v<CT>; }

	[[nodiscard]] static constexpr ColorType colorNative() { return CT; }

	[[nodiscard]] static constexpr ColorType colorDefaultSRGB()
	{
		return colorHasAlpha() ? ColorType::RGBA8U : ColorType::RGB8U;
	}

	/**************************************************************************************
	|                                                                                     |
	|                                       Access                                        |
	|                                                                                     |
	**************************************************************************************/

	// TODO: Add

	// template <ColorType To = color::remove_weight_v<colorNative()>, class NodeType,
	//           std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	// [[nodiscard]] Color<To> color(NodeType const& node, Direction const& view_dir) const
	// {
	// 	Index       n  = derived().index(node);
	// 	auto const& cb = colorBlock(n.pos)[n.offset];

	// 	auto const indices = dirIndices(view_dir);
	// 	auto const weights = dirWeights(view_dir);

	// 	std::array<Color<color::remove_weight_v<CT>>, Dim> colors;
	// 	for (unsigned i{}; Dim > i; ++i) {
	// 		colors[i] = ufo::removeWeight(cb[indices[i]]);
	// 	}

	// 	return ufo::convert<To>(blend(colors.begin(), colors.end(), weights.begin()));
	// }

	// template <ColorType To = colorDefaultSRGB(), class NodeType,
	//           std::enable_if_t<color::is_rgb_v<To>, bool>                     = true,
	//           std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	// [[nodiscard]] Color<To> colorSRGB(NodeType const& node, Direction const& view_dir)
	// const
	// {
	// 	return fromLinearRGB(color<To>(node, view_dir));
	// }

	template <ColorType To = colorNative(), class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] Color<To> color(NodeType const& node) const
	{
		Index       n  = derived().index(node);
		auto const& cb = colorBlock(n.pos)[n.offset];
		return ufo::convert<To>(average(cb));
	}

	template <ColorType To = colorDefaultSRGB(), class NodeType,
	          std::enable_if_t<color::is_rgb_v<To>, bool>                     = true,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] Color<To> colorSRGB(NodeType const& node) const
	{
		return fromLinearRGB(color<To>(node));
	}

	/**************************************************************************************
	|                                                                                     |
	|                                      Modifiers                                      |
	|                                                                                     |
	**************************************************************************************/

	template <class NodeType, ColorType CT2,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void colorSet(NodeType const& node, Direction const& view_dir,
	              Normal const& surface_normal, Color<CT2> const& value,
	              bool propagate = true)
	{
		auto const c = ufo::convert<CT>(value);
		auto const w = dirWeights(surface_normal);
		// auto const indices = dirIndices(-surface_normal);
		// auto const weights = dirWeights(view_dir, surface_normal);

		// value_type v{};
		// for (unsigned i{}; Dim > i; ++i) {
		// 	v[indices[i]] = c * weights[i];
		// }

		value_type v;
		for (unsigned i{}; v.size() > i; ++i) {
			v[i] = c * w[i];
		}

		auto node_f = [this, v](Index node) { colorBlock(node.pos)[node.offset] = v; };

		auto block_f = [this, v](pos_t block) { colorBlock(block).fill(v); };

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	template <class NodeType, ColorType CT2,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void colorSet(NodeType const& node, Direction const& view_dir, Color<CT2> const& value,
	              bool propagate = true)
	{
		return colorSet(node, view_dir, -view_dir, value, propagate);
	}

	template <class NodeType, ColorType CT2,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void colorSet(NodeType const& node, Color<CT2> const& value, bool propagate = true)
	{
		auto const v = ufo::convert<CT>(value);

		auto node_f = [this, v](Index node) { colorBlock(node.pos)[node.offset].fill(v); };

		auto block_f = [this, v](pos_t block) {
			for (auto& c : colorBlock(block)) {
				c.fill(v);
			}
		};

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	//
	// Add
	//

	template <class NodeType, ColorType CT2,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void colorAdd(NodeType const& node, Direction const& view_dir,
	              Normal const& surface_normal, Color<CT2> const& value,
	              bool propagate = true)
	{
		auto const c = ufo::convert<CT>(value);
		// TODO: auto const w_depth = ...;
		auto const w_angle = std::clamp(-dot(view_dir, surface_normal), 0.0f, 1.0f);
		auto const w_dir   = dirWeights(surface_normal);
		value_type v;
		for (unsigned i{}; v.size() > i; ++i) {
			v[i] = c * w_angle * w_dir[i];
		}

		// NOTE: Got bus error when capturing by reference in the two below, at the same
		// time.

		auto node_f = [this, v](Index node) {
			auto& c = colorBlock(node.pos)[node.offset];
			for (unsigned i{}; v.size() > i; ++i) {
				c[i] += v[i];
			}
		};

		auto block_f = [this, v](pos_t block) {
			for (auto& c : colorBlock(block)) {
				for (unsigned i{}; v.size() > i; ++i) {
					c[i] += v[i];
				}
			}
		};

		// auto const                 c       = ufo::convert<CT>(value);
		// auto const                 indices = dirIndices(-surface_normal);
		// auto const                 weights = dirWeights(view_dir, surface_normal);
		// std::array<Color<CT>, Dim> v;
		// for (unsigned i{}; Dim > i; ++i) {
		// 	v[i] = c * weights[i];
		// }

		// // NOTE: Got bus error when capturing by reference in the two below, at the same
		// // time.

		// auto node_f = [this, indices, v](Index node) {
		// 	auto& c = colorBlock(node.pos)[node.offset];
		// 	for (unsigned i{}; Dim > i; ++i) {
		// 		c[indices[i]] += v[i];
		// 	}
		// };

		// auto block_f = [this, indices, v](pos_t block) {
		// 	for (auto& c : colorBlock(block)) {
		// 		for (unsigned i{}; Dim > i; ++i) {
		// 			c[indices[i]] += v[i];
		// 		}
		// 	}
		// };

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	template <class NodeType, ColorType CT2,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void colorAdd(NodeType const& node, Direction const& view_dir, Color<CT2> const& value,
	              bool propagate = true)
	{
		colorAdd(node, view_dir, -view_dir, value, propagate);
	}

	template <class NodeType, ColorType CT2,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void colorAdd(NodeType const& node, Color<CT2> const& value, bool propagate = true)
	{
		auto const v = ufo::convert<CT>(value);

		auto node_f = [this, v](Index node) {
			for (auto& c : colorBlock(node.pos)[node.offset]) {
				c += v;
			}
		};

		auto block_f = [this, v](pos_t block) {
			for (auto& cs : colorBlock(block)) {
				for (auto& c : cs) {
					c += v;
				}
			}
		};

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	//
	// Remove
	//

	template <class NodeType, ColorType CT2,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void colorRemove(NodeType const& node, Direction const& view_dir,
	                 Normal const& surface_normal, Color<CT2> const& value,
	                 bool propagate = true)
	{
		// TODO: Implement

		// auto const                 c       = ufo::convert<CT>(value);
		// auto const                 indices = dirIndices(-surface_normal);
		// auto const                 weights = dirWeights(view_dir, surface_normal);
		// std::array<Color<CT>, Dim> v;
		// for (unsigned i{}; Dim > i; ++i) {
		// 	v[i] = c * weights[i];
		// }

		// auto node_f = [this, indices, v](Index node) {
		// 	auto& c = colorBlock(node.pos)[node.offset];
		// 	for (unsigned i{}; Dim > i; ++i) {
		// 		c[indices[i]] -= v[i];
		// 	}
		// };

		// auto block_f = [this, indices, v](pos_t block) {
		// 	for (auto& c : colorBlock(block)) {
		// 		for (unsigned i{}; Dim > i; ++i) {
		// 			c[indices[i]] -= v[i];
		// 		}
		// 	}
		// };

		// auto update_f = [this](Index node, pos_t children) {
		// 	onPropagateChildren(node, children);
		// };

		// derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	template <class NodeType, ColorType CT2,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void colorRemove(NodeType const& node, Direction const& view_dir,
	                 Color<CT2> const& value, bool propagate = true)
	{
		colorRemove(node, view_dir, -view_dir, value, propagate);
	}

	template <class NodeType, ColorType CT2,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void colorRemove(NodeType const& node, Color<CT2> const& value, bool propagate = true)
	{
		auto const v = ufo::convert<CT>(value);

		auto node_f = [this, v](Index node) {
			for (auto& c : colorBlock(node.pos)[node.offset]) {
				c -= v;
			}
		};

		auto block_f = [this, v](pos_t block) {
			for (auto& cs : colorBlock(block)) {
				for (auto& c : cs) {
					c -= v;
				}
			}
		};

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	//
	// Update
	//

	template <class NodeType, class UnaryOp,
	          std::enable_if_t<std::is_invocable_v<UnaryOp, Index>, bool> = true>
	void colorUpdate(NodeType const& node, Direction const& view_dir,
	                 Normal const& surface_normal, UnaryOp unary_op, bool propagate = true)
	{
		// TODO: Implement

		// // TODO: What should this function do?

		// auto const indices = dirIndices(-surface_normal);
		// auto const weights = dirWeights(view_dir, surface_normal);

		// auto node_f = [this, indices, weights, unary_op](Index node) {
		// 	auto const c = ufo::convert<CT>(unary_op(node));

		// 	auto& cb = colorBlock(node.pos)[node.offset];

		// 	cb = {};
		// 	for (unsigned i{}; Dim > i; ++i) {
		// 		cb[indices[i]] = c * weights[i];
		// 	}
		// };

		// auto block_f = [this, indices, weights, unary_op](pos_t block) {
		// 	for (unsigned i{}; BF > i; ++i) {
		// 		auto const c = ufo::convert<CT>(unary_op(Index(block, i)));

		// 		auto& cb = colorBlock(block)[i];

		// 		cb = {};
		// 		for (unsigned j{}; Dim > j; ++j) {
		// 			cb[indices[j]] = c * weights[j];
		// 		}
		// 	}
		// };

		// auto update_f = [this](Index node, pos_t children) {
		// 	onPropagateChildren(node, children);
		// };

		// derived().recursLeaves(node, node_f, block_f, update_f, propagate);
	}

	template <class NodeType, class UnaryOp,
	          std::enable_if_t<std::is_invocable_v<UnaryOp, Index>, bool> = true>
	void colorUpdate(NodeType const& node, Direction const& view_dir, UnaryOp unary_op,
	                 bool propagate = true)
	{
		colorUpdate(node, view_dir, -view_dir, unary_op, propagate);
	}

	template <class NodeType, class UnaryOp,
	          std::enable_if_t<std::is_invocable_v<UnaryOp, Index>, bool> = true>
	void colorUpdate(NodeType const& node, UnaryOp unary_op, bool propagate = true)
	{
		// TODO: What should this function do?

		auto node_f = [this, unary_op](Index node) {
			colorBlock(node.pos)[node.offset].fill(ufo::convert<CT>(unary_op(node)));
		};

		auto block_f = [this, unary_op](pos_t block) {
			for (unsigned i{}; BF > i; ++i) {
				colorBlock(block)[i].fill(ufo::convert<CT>(unary_op(Index(block, i))));
			}
		};

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursLeaves(node, node_f, block_f, update_f, propagate);
	}

	//
	// Clear
	//

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void colorClear(NodeType const& node, bool propagate = true)
	{
		colorSet(node, color_type{}, propagate);
	}

	/**************************************************************************************
	|                                                                                     |
	|                                       Lookup                                        |
	|                                                                                     |
	**************************************************************************************/

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] auto colorAlpha(NodeType const& node, Direction const& view_dir) const
	{
		return alpha(color(node, view_dir));
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] auto colorAlpha(NodeType const& node) const
	{
		return alpha(color(node));
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] float colorWeight(NodeType const& node, Direction const& view_dir) const
	{
		return weight(color(node, view_dir));
	}

	template <class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] float colorWeight(NodeType const& node) const
	{
		return weight(color(node));
	}

	/**************************************************************************************
	|                                                                                     |
	|                                        Prune                                        |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] constexpr bool colorPruneNormalize() const { return prune_normalize_; }

	void colorPruneNormalize(bool value) { prune_normalize_ = value; }

	[[nodiscard]] constexpr float colorPruneMaxDeltaE() const
	{
		return std::sqrt(prune_max_delta_e_sq_);
	}

	void colorPruneMaxDeltaE(float value) { prune_max_delta_e_sq_ = value * value; }

	[[nodiscard]] constexpr ColorDeltaE colorPruneDeltaE() const { return prune_delta_e_; }

	void colorPruneDeltaE(ColorDeltaE value) { prune_delta_e_ = value; }

	[[nodiscard]] constexpr ColorSpace colorPruneSpace() const { return prune_space_; }

	void colorPruneSpace(ColorSpace value) { prune_space_ = value; }

	/**************************************************************************************
	|                                                                                     |
	|                                         GPU                                         |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] std::size_t gpuColorNumBuffers() const
	{
		return derived().template gpuNumBuffers<Block>();
	}

	[[nodiscard]] WGPUBuffer gpuColorBuffer(std::size_t index = 0) const
	{
		return derived().template gpuBuffer<Block>(index);
	}

	[[nodiscard]] std::size_t gpuColorBufferSize(std::size_t index = 0) const
	{
		return derived().template gpuBufferSize<Block>(index);
	}

	bool gpuColorUpdate() { return derived().template gpuUpdate<Block>(); }

 protected:
	/**************************************************************************************
	|                                                                                     |
	|                                    Constructors                                     |
	|                                                                                     |
	**************************************************************************************/

	ColorMap() { onInitRoot(); }

	ColorMap(ColorMap const&) = default;

	ColorMap(ColorMap&&) = default;

	template <class Derived2, class Tree2, ColorType CT2, bool Directional2>
	ColorMap(ColorMap<Derived2, Tree2, CT2, Directional2> const& other)
	// TODO: Implement
	{
	}

	template <class Derived2, class Tree2, ColorType CT2, bool Directional2>
	ColorMap(ColorMap<Derived2, Tree2, CT2, Directional2>&& other)
	// TODO: Implement
	{
	}

	/**************************************************************************************
	|                                                                                     |
	|                                     Destructor                                      |
	|                                                                                     |
	**************************************************************************************/

	~ColorMap() = default;

	/**************************************************************************************
	|                                                                                     |
	|                                 Assignment operator                                 |
	|                                                                                     |
	**************************************************************************************/

	ColorMap& operator=(ColorMap const&) = default;

	ColorMap& operator=(ColorMap&&) = default;

	template <class Derived2, class Tree2, ColorType CT2, bool Directional2>
	ColorMap& operator=(ColorMap<Derived2, Tree2, CT2, Directional2> const& rhs)
	{
		// TODO: Implement
		return *this;
	}

	template <class Derived2, class Tree2, ColorType CT2, bool Directional2>
	ColorMap& operator=(ColorMap<Derived2, Tree2, CT2, Directional2>&& rhs)
	{
		// TODO: Implement
		return *this;
	}

	//
	// Swap
	//

	friend void swap(ColorMap& lhs, ColorMap& rhs) noexcept
	{
		// TODO: Implement
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

	[[nodiscard]] Block& colorBlock(pos_t pos)
	{
		return derived().template data<Block>(pos);
	}

	[[nodiscard]] Block const& colorBlock(pos_t pos) const
	{
		return derived().template data<Block>(pos);
	}

	/**************************************************************************************
	|                                                                                     |
	|                                     Directional                                     |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] float dirWeight(Normal const&    surface_normal,
	                              Direction const& view_dir) const
	{
		assert(isNormalized(surface_normal));

		float const theta = numbers::pi_v<float> / 2.0f;
		float const v     = std::acos(-dot(surface_normal, view_dir));
		float const x     = (1.0f - v) / (2.0f * theta - (numbers::pi_v<float> / 4.0f));
		return std::clamp(x, 0.0f, 1.0f);
		// return std::clamp(dot(surface_normal, view_dir), 0.0f, 1.0f);
	}

	[[nodiscard]] std::array<float, 2 * Dim> dirWeights(Normal const& surface_normal) const
	{
		// clang-format off
		return std::array{
		    dirWeight(surface_normal, Direction(-1.0f,  0.0f,  0.0f)),
		    dirWeight(surface_normal, Direction( 1.0f,  0.0f,  0.0f)),
		    dirWeight(surface_normal, Direction( 0.0f, -1.0f,  0.0f)),
		    dirWeight(surface_normal, Direction( 0.0f,  1.0f,  0.0f)),
		    dirWeight(surface_normal, Direction( 0.0f,  0.0f, -1.0f)),
		    dirWeight(surface_normal, Direction( 0.0f,  0.0f,  1.0f)),
		};
		// clang-format on
	}

	// [[nodiscard]] constexpr unsigned dirIndex(Direction const& view_dir, unsigned i)
	// const
	// {
	// 	assert(isNormalized(view_dir));

	// 	return 2u * i + (view_dir[i] < 0.0f ? 0u : 1u);
	// }

	// [[nodiscard]] std::array<unsigned, Dim> dirIndices(Direction const& view_dir) const
	// {
	// 	std::array<unsigned, Dim> indices;
	// 	for (unsigned i{}; Dim > i; ++i) {
	// 		indices[i] = dirIndex(view_dir, i);
	// 	}
	// 	return indices;
	// }

	// [[nodiscard]] Vec<Dim, float> dirWeights(Direction const& view_dir) const
	// {
	// 	assert(isNormalized(view_dir));

	// 	auto const a = abs(view_dir);
	// 	return a / sum(a);
	// }

	// [[nodiscard]] Vec<Dim, float> dirWeights(Direction const& view_dir,
	//                                          Normal const&    surface_normal) const
	// {
	// 	assert(isNormalized(view_dir));
	// 	assert(isNormalized(surface_normal));

	// 	// Inner product

	// 	float const w = std::clamp(-dot(view_dir, surface_normal), 0.0f, 1.0f);

	// 	// return {w, w, w};
	// 	return dirWeights(surface_normal) * w;
	// }

	template <ColorType To, ColorType From, std::size_t N,
	          std::enable_if_t<To != From, bool> = true>
	[[nodiscard]] std::array<Color<To>, N> convert(
	    std::array<Color<From>, N> const& from) const
	{
		std::array<Color<To>, N> to;
		for (unsigned i{}; N > i; ++i) {
			to[i] = ufo::convert<To>(from[i]);
		}
		return to;
	}

	template <ColorType T, std::size_t N>
	[[nodiscard]] std::array<Color<T>, N> convert(std::array<Color<T>, N> from) const
	{
		return from;
	}

	template <ColorType T, std::size_t N,
	          std::enable_if_t<color::has_weight_v<T>, bool> = true>
	[[nodiscard]] std::array<Color<color::remove_weight_v<T>>, N> removeWeight(
	    std::array<Color<T>, N> const& from) const
	{
		std::array<Color<color::remove_weight_v<T>>, N> to;
		for (unsigned i{}; N > i; ++i) {
			to[i] = ufo::removeWeight(from[i]);
		}
		return to;
	}

	template <ColorType T, std::size_t N,
	          std::enable_if_t<!color::has_weight_v<T>, bool> = true>
	[[nodiscard]] std::array<Color<T>, N> removeWeight(std::array<Color<T>, N> from) const
	{
		return from;
	}

	/**************************************************************************************
	|                                                                                     |
	|                              Functions Derived expects                              |
	|                                                                                     |
	**************************************************************************************/

	void onInitRoot() { colorBlock(0)[0] = {}; }

	void onInitChildren(Index node, pos_t children)
	{
		colorBlock(children).fill(colorBlock(node.pos)[node.offset]);
	}

	void onPropagateChildren(Index node, pos_t children)
	{
		auto&       c  = colorBlock(node.pos)[node.offset];
		auto const& cb = colorBlock(children);

		unsigned const size = c.size();

		std::array<Color<CT>, BF> v;
		for (unsigned i{}; size > i; ++i) {
			for (unsigned j{}; BF > j; ++j) {
				v[j] = cb[j][i];
			}

			c[i] = average(v);
		}
	}

	[[nodiscard]] bool onIsPrunable(pos_t block) const
	{
		using std::begin;
		using std::end;
		auto const& cb = colorBlock(block);
		if (prune_normalize_) {
			return std::all_of(
			    begin(cb) + 1, end(cb), [this, v = removeWeight(cb[0])](auto const& e) {
				    auto const x = removeWeight(e);
				    return std::equal(begin(v), end(v), begin(x),
				                      [mdes = prune_max_delta_e_sq_, de = prune_delta_e_,
				                       s = prune_space_](auto const& a, auto const& b) {
					                      return mdes >= deltaESquared(a, b, de, s);
				                      });
			    });
		} else {
			// FIXME: How to do this?
			return std::all_of(begin(cb) + 1, end(cb),
			                   [v = cb[0]](auto const& e) { return v == e; });
		}
	}

	void onPruneChildren(Index /* node */, pos_t /* children */) {}

	[[nodiscard]] std::size_t onSerializedSize(
	    std::vector<std::pair<pos_t, BitSet<BF>>> const& /* nodes */,
	    std::size_t num_nodes) const
	{
		// ColorType + is_directional + num_nodes * color
		return sizeof(std::uint64_t) + sizeof(std::uint8_t) + num_nodes * sizeof(value_type);
	}

	void onRead(ReadBuffer& in, std::vector<std::pair<pos_t, BitSet<BF>>> const& nodes)
	{
		std::uint64_t tmp;
		in.read(tmp);
		ColorType const ct = static_cast<ColorType>(tmp);

		switch (ct) {
			case ColorType::RGB8U: return onRead<ColorType::RGB8U>(in, nodes);
			case ColorType::RGBA8U: return onRead<ColorType::RGBA8U>(in, nodes);
			case ColorType::RGB32F: return onRead<ColorType::RGB32F>(in, nodes);
			case ColorType::RGBA32F: return onRead<ColorType::RGBA32F>(in, nodes);
			case ColorType::RGBW32F: return onRead<ColorType::RGBW32F>(in, nodes);
			case ColorType::RGBAW32F: return onRead<ColorType::RGBAW32F>(in, nodes);
			case ColorType::LAB32F: return onRead<ColorType::LAB32F>(in, nodes);
			case ColorType::LABA32F: return onRead<ColorType::LABA32F>(in, nodes);
			case ColorType::LABW32F: return onRead<ColorType::LABW32F>(in, nodes);
			case ColorType::LABAW32F: return onRead<ColorType::LABAW32F>(in, nodes);
			case ColorType::LCH32F: return onRead<ColorType::LCH32F>(in, nodes);
			case ColorType::LCHA32F: return onRead<ColorType::LCHA32F>(in, nodes);
			case ColorType::LCHW32F: return onRead<ColorType::LCHW32F>(in, nodes);
			case ColorType::LCHAW32F: return onRead<ColorType::LCHAW32F>(in, nodes);
		}
	}

	template <ColorType From>
	void onRead(ReadBuffer& in, std::vector<std::pair<pos_t, BitSet<BF>>> const& nodes)
	{
		std::uint8_t tmp;
		in.read(tmp);
		bool const directional = static_cast<bool>(tmp);

		if (directional) {
			if constexpr (CT == From) {
				for (auto [block, offset] : nodes) {
					auto& cb = colorBlock(block);

					if (offset.all()) {
						in.read(cb);
					} else {
						for (std::size_t i{}; BF > i; ++i) {
							if (offset[i]) {
								in.read(cb[i]);
							}
						}
					}
				}
			} else {
				for (auto [block, offset] : nodes) {
					auto& cb = colorBlock(block);

					std::array<Color<From>, 2 * Dim> tmp;
					for (std::size_t i{}; BF > i; ++i) {
						if (offset[i]) {
							in.read(tmp);
							cb[i] = convert<CT>(tmp);
						}
					}
				}
			}
		} else {
			for (auto [block, offset] : nodes) {
				auto& cb = colorBlock(block);

				if (offset.all()) {
					std::array<Color<From>, BF> tmp;
					in.read(tmp);
					for (unsigned i{}; BF > i; ++i) {
						cb[i].fill(ufo::convert<CT>(tmp[i]));
					}
				} else {
					Color<From> tmp;
					for (std::size_t i{}; BF > i; ++i) {
						if (offset[i]) {
							in.read(tmp);
							cb[i].fill(ufo::convert<CT>(tmp));
						}
					}
				}
			}
		}
	}

	void onWrite(WriteBuffer&                                     out,
	             std::vector<std::pair<pos_t, BitSet<BF>>> const& nodes) const
	{
		std::uint64_t const ct = static_cast<std::uint64_t>(to_underlying(CT));
		out.write(ct);

		std::uint8_t const directional = true;
		out.write(directional);

		for (auto [block, offset] : nodes) {
			auto const& cb = colorBlock(block);

			if (offset.all()) {
				out.write(cb);
			} else {
				for (std::size_t i{}; BF > i; ++i) {
					if (offset[i]) {
						out.write(cb[i]);
					}
				}
			}
		}
	}

	void onDotFile(std::ostream& out, Index node) const { out << "Color: " << color(node); }

 private:
	bool        prune_normalize_      = false;
	float       prune_max_delta_e_sq_ = 2.3f * 2.3f;
	ColorDeltaE prune_delta_e_        = ColorDeltaE::EUCLIDEAN;
	ColorSpace  prune_space_          = ColorSpace::NATIVE;
};

template <class Derived, class Tree>
using ColorDirectionalMapRGBA8u = ColorMap<Derived, Tree, ColorType::RGBA8U, true>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorDirectionalMapRGBA8u, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::RGBA8U, true>;
};

template <class Derived, class Tree>
using ColorDirectionalMapRGBW32f = ColorMap<Derived, Tree, ColorType::RGBW32F, true>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorDirectionalMapRGBW32f, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::RGBW32F, true>;
};

template <class Derived, class Tree>
using ColorDirectionalMapRGBAW32f = ColorMap<Derived, Tree, ColorType::RGBAW32F, true>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorDirectionalMapRGBAW32f, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::RGBAW32F, true>;
};

template <class Derived, class Tree>
using ColorDirectionalMapLabW32f = ColorMap<Derived, Tree, ColorType::LABW32F, true>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorDirectionalMapLabW32f, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::LABW32F, true>;
};

template <class Derived, class Tree>
using ColorDirectionalMapLabAW32f = ColorMap<Derived, Tree, ColorType::LABAW32F, true>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorDirectionalMapLabAW32f, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::LABAW32F, true>;
};

template <class Derived, class Tree>
using ColorDirectionalMapLChW32f = ColorMap<Derived, Tree, ColorType::LCHW32F, true>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorDirectionalMapLChW32f, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::LCHW32F, true>;
};

template <class Derived, class Tree>
using ColorDirectionalMapLChAW32f = ColorMap<Derived, Tree, ColorType::LCHAW32F, true>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorDirectionalMapLChAW32f, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::LCHAW32F, true>;
};

template <class Derived, class Tree>
using ColorDirectionalMapSmall = ColorDirectionalMapRGBA8u<Derived, Tree>;

template <class Derived, class Tree>
using ColorDirectionalMapFine = ColorDirectionalMapLabW32f<Derived, Tree>;

template <class Derived, class Tree>
using ColorDirectionalMapFineWithAlpha = ColorDirectionalMapLabAW32f<Derived, Tree>;
}  // namespace ufo

#endif  // UFO_MAP_COLOR_DIRECTIONAL_MAP_HPP