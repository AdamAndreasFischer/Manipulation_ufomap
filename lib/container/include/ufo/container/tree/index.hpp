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

#ifndef UFO_CONTAINER_TREE_INDEX_HPP
#define UFO_CONTAINER_TREE_INDEX_HPP

// STL
#include <algorithm>
#include <cstdint>
#include <functional>
#include <limits>
#include <ostream>

namespace ufo
{
struct TreeIndex {
	// 64 bits instead, either
	// 	32 bits X high levels
	// 	32 bits Y low levels
	// or
	//  32-Dim bits X high levels
	//  32 bits Y low levels
	//  Dim bits for offset

	using pos_t    = std::uint32_t;
	using offset_t = std::uint32_t;

	static constexpr pos_t const NULL_POS       = std::numeric_limits<pos_t>::max();
	static constexpr pos_t const PROCESSING_POS = NULL_POS - 1;

	pos_t    pos{NULL_POS};
	offset_t offset{0};

	constexpr TreeIndex() noexcept = default;

	constexpr TreeIndex(pos_t pos, offset_t offset) noexcept : pos(pos), offset(offset) {}

	friend void swap(TreeIndex& lhs, TreeIndex& rhs) noexcept
	{
		std::swap(lhs.pos, rhs.pos);
		std::swap(lhs.offset, rhs.offset);
	}

	constexpr bool operator==(TreeIndex rhs) const
	{
		return pos == rhs.pos && offset == rhs.offset;
	}

	constexpr bool operator!=(TreeIndex rhs) const { return !(operator==(rhs)); }

	[[nodiscard]] constexpr TreeIndex sibling(offset_t offset) const
	{
		return {pos, offset};
	}

	[[nodiscard]] constexpr bool valid() const { return PROCESSING_POS > pos; }
};

inline std::ostream& operator<<(std::ostream& out, TreeIndex index)
{
	return out << "pos: " << +index.pos << " offset: " << +index.offset;
}
}  // namespace ufo

namespace std
{
template <>
struct hash<ufo::TreeIndex> {
	std::size_t operator()(ufo::TreeIndex index) const
	{
		return (static_cast<std::uint64_t>(index.pos) << 3) | index.offset;
	}
};
}  // namespace std

#endif  // UFO_CONTAINER_TREE_INDEX_HPP