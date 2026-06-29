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

#ifndef UFO_CONTAINER_TREE_MAP_BLOCK_HPP
#define UFO_CONTAINER_TREE_MAP_BLOCK_HPP

// UFO
#include <ufo/container/tree/block.hpp>
#include <ufo/container/tree/code.hpp>
#include <ufo/container/tree/index.hpp>
#include <ufo/geometry/aabb.hpp>

// STL
#include <array>
#include <cstddef>
#include <limits>
#include <list>
#include <utility>

namespace ufo
{
template <std::size_t Dim, std::size_t BF, class T>
struct TreeMapBlock : public TreeBlock<Dim, BF> {
	using Base = TreeBlock<Dim, BF>;

	using Code           = TreeCode<Dim>;
	using length_t       = typename Base::length_t;
	using Length         = typename Base::Length;
	using Point          = typename Base::Point;
	using Bounds         = AABB<Dim, typename Point::value_type>;
	using scalar_type    = typename Point::value_type;
	using value_type     = std::pair<Point const, T>;
	using container_type = std::list<value_type>;

	static constexpr auto const MIN =
	    Point(std::numeric_limits<typename Point::value_type>::lowest());
	static constexpr auto const MAX =
	    Point(std::numeric_limits<typename Point::value_type>::max());

	std::array<Bounds, BF>         bounds = createArray<BF>(Bounds(MAX, MIN));
	std::array<container_type, BF> values;

	constexpr TreeMapBlock() = default;

	constexpr TreeMapBlock(TreeIndex::pos_t parent_block, Code code, Point center,
	                       Length half_length)
	    : Base(parent_block, code, center, half_length)
	{
	}

	constexpr TreeMapBlock(TreeIndex::pos_t parent_block, TreeMapBlock const& parent,
	                       std::size_t offset, Length half_length)
	    : Base(parent_block, parent, offset, half_length)
	{
	}
};
}  // namespace ufo

#endif  // UFO_CONTAINER_TREE_MAP_BLOCK_HPP