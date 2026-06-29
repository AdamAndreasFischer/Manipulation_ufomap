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

#ifndef UFO_MAP_COLOR_MAP_HPP
#define UFO_MAP_COLOR_MAP_HPP

// UFO
#include <ufo/compute/compute.hpp>
#include <ufo/map/block.hpp>
#include <ufo/map/color/block.hpp>
#include <ufo/map/type.hpp>
#include <ufo/utility/bit_set.hpp>
#include <ufo/utility/enum.hpp>
#include <ufo/utility/io/buffer.hpp>
#include <ufo/vision/color.hpp>

// STL
#include <algorithm>
#include <array>
#include <iostream>
#include <type_traits>
#include <vector>

namespace ufo
{
template <class Derived, class Tree, ColorType CT, bool Directional>
class ColorMap
{
	template <class Derived2, class Tree2, ColorType CT2, bool Directional2>
	friend class ColorMap;

	static constexpr auto const BF  = Tree::branchingFactor();
	static constexpr auto const Dim = Tree::dimensions();

	using Block = ColorBlock<Dim, BF, CT, false>;

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

 public:
	/**************************************************************************************
	|                                                                                     |
	|                                        Info                                         |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] static constexpr bool colorDirectional() { return false; }

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

	template <ColorType To = colorNative(), class NodeType,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	[[nodiscard]] Color<To> color(NodeType const& node) const
	{
		Index n = derived().index(node);
		return convert<To>(colorBlock(n.pos)[n.offset]);
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
	void colorSet(NodeType const& node, Color<CT2> const& value, bool propagate = true)
	{
		auto const v = convert<CT>(value);

		auto node_f = [this, v](Index node) { colorBlock(node.pos)[node.offset] = v; };

		auto block_f = [this, v](pos_t block) { colorBlock(block).fill(v); };

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	//
	// Add color
	//

	template <class NodeType, ColorType CT2,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void colorAdd(NodeType const& node, Color<CT2> const& value, bool propagate = true)
	{
		auto const v = convert<CT>(value);

		auto node_f = [this, v](Index node) { colorBlock(node.pos)[node.offset] += v; };

		auto block_f = [this, v](pos_t block) {
			for (auto& c : colorBlock(block)) {
				c += v;
			}
		};

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	//
	// Remove color
	//

	template <class NodeType, ColorType CT2,
	          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true>
	void colorRemove(NodeType const& node, Color<CT2> const& value, bool propagate = true)
	{
		auto const v = convert<CT>(value);

		auto node_f = [this, v](Index node) { colorBlock(node.pos)[node.offset] -= v; };

		auto block_f = [this, v](pos_t block) {
			for (auto& c : colorBlock(block)) {
				c -= v;
			}
		};

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursParentFirst(node, node_f, block_f, update_f, propagate);
	}

	//
	// Update color
	//

	template <class NodeType, class UnaryOp,
	          std::enable_if_t<std::is_invocable_v<UnaryOp, Index>, bool> = true>
	void colorUpdate(NodeType const& node, UnaryOp unary_op, bool propagate = true)
	{
		auto node_f = [this, unary_op](Index node) {
			colorBlock(node.pos)[node.offset] = convert<CT>(unary_op(node));
		};

		auto block_f = [this, unary_op](pos_t block) {
			auto& cb = colorBlock(block);
			for (unsigned i{}; BF > i; ++i) {
				cb[i] = convert<CT>(unary_op(Index(block, i)));
			}
		};

		auto update_f = [this](Index node, pos_t children) {
			onPropagateChildren(node, children);
		};

		derived().recursLeaves(node, node_f, block_f, update_f, propagate);
	}

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
	[[nodiscard]] auto colorAlpha(NodeType const& node) const
	{
		return alpha(color(node));
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
		colorBlock(node.pos)[node.offset] = average(colorBlock(children));
	}

	[[nodiscard]] bool onIsPrunable(pos_t block) const
	{
		using std::begin;
		using std::end;
		if (prune_normalize_) {
			return std::all_of(begin(colorBlock(block)) + 1, end(colorBlock(block)),
			                   [this, v = removeWeight(colorBlock(block)[0])](auto const& e) {
				                   return prune_max_delta_e_sq_ >=
				                          deltaESquared(v, removeWeight(e), prune_delta_e_,
				                                        prune_space_);
			                   });
		} else {
			// FIXME: How to do this?
			return std::all_of(begin(colorBlock(block)) + 1, end(colorBlock(block)),
			                   [&v = colorBlock(block)[0]](auto const& e) { return v == e; });
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
			for (auto [block, offset] : nodes) {
				auto& cb = colorBlock(block);

				std::array<Color<From>, 2 * Dim> tmp;
				for (std::size_t i{}; BF > i; ++i) {
					if (offset[i]) {
						in.read(tmp);
						cb[i] = convert<CT>(average(tmp));
					}
				}
			}
		} else {
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

					Color<From> tmp;
					for (std::size_t i{}; BF > i; ++i) {
						if (offset[i]) {
							in.read(tmp);
							cb[i] = convert<CT>(tmp);
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

		std::uint8_t const directional = false;
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

	void onDotFile(std::ostream& out, Index node) const
	{
		out << "Color: " << color(node);

		// Color    c     = color(node);
		// unsigned red   = c.red;
		// unsigned green = c.green;
		// unsigned blue  = c.blue;
		// unsigned alpha = c.alpha;

		// out << "RGBA: <font color='#" << std::hex << std::setfill('0') << std::setw(2) <<
		// red
		//     << std::setw(2) << green << std::setw(2) << blue << "'> #" << std::uppercase
		//     << std::setw(2) << red << std::setw(2) << green << std::setw(2) << blue
		//     << std::setw(2) << alpha << "</font>" << std::nouppercase << std::dec;

		// out << color_[node.pos][node.offset];

		// auto color_box = "▀";  // "█"; // ▄, ▌, ▐

		// out << std::hex << std::setfill('0') << "RGBA: #" << std::setw(2) << red
		//     << std::setw(2) << green << std::setw(2) << blue << std::setw(2) << alpha
		//     << "<font color='#" << std::setw(2) << red << std::setw(2) << green
		//     << std::setw(2) << blue << "'> " << color_box << "</font>" << std::dec;
	}

 private:
	bool        prune_normalize_      = false;
	float       prune_max_delta_e_sq_ = 2.3f * 2.3f;
	ColorDeltaE prune_delta_e_        = ColorDeltaE::EUCLIDEAN;
	ColorSpace  prune_space_          = ColorSpace::NATIVE;
};

template <class Derived, class Tree>
using ColorMapRGBA8u = ColorMap<Derived, Tree, ColorType::RGBA8U, false>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorMapRGBA8u, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::RGBA8U, false>;
};

template <class Derived, class Tree>
using ColorMapRGBW32f = ColorMap<Derived, Tree, ColorType::RGBW32F, false>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorMapRGBW32f, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::RGBW32F, false>;
};

template <class Derived, class Tree>
using ColorMapRGBAW32f = ColorMap<Derived, Tree, ColorType::RGBAW32F, false>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorMapRGBAW32f, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::RGBAW32F, false>;
};

template <class Derived, class Tree>
using ColorMapLabW32f = ColorMap<Derived, Tree, ColorType::LABW32F, false>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorMapLabW32f, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::LABW32F, false>;
};

template <class Derived, class Tree>
using ColorMapLabAW32f = ColorMap<Derived, Tree, ColorType::LABAW32F, false>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorMapLabAW32f, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::LABAW32F, false>;
};

template <class Derived, class Tree>
using ColorMapLChW32f = ColorMap<Derived, Tree, ColorType::LCHW32F, false>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorMapLChW32f, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::LCHW32F, false>;
};

template <class Derived, class Tree>
using ColorMapLChAW32f = ColorMap<Derived, Tree, ColorType::LCHAW32F, false>;

template <std::size_t Dim, std::size_t BF>
struct map_block<ColorMapLChAW32f, Dim, BF> {
	using type = ColorBlock<Dim, BF, ColorType::LCHAW32F, false>;
};

template <class Derived, class Tree>
using ColorMapSmall = ColorMapRGBA8u<Derived, Tree>;

template <class Derived, class Tree>
using ColorMapFine = ColorMapLabW32f<Derived, Tree>;

template <class Derived, class Tree>
using ColorMapFineWithAlpha = ColorMapLabAW32f<Derived, Tree>;
}  // namespace ufo

#endif  // UFO_MAP_COLOR_MAP_HPP