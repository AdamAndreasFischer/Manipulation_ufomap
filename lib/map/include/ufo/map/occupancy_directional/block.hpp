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
#ifndef UFO_MAP_OCCUPANCY_DIRECTIONAL_BLOCK_HPP
#define UFO_MAP_OCCUPANCY_DIRECTIONAL_BLOCK_HPP

// UFO
#include <ufo/math/math.hpp>
#include <ufo/utility/create_array.hpp>

// STL
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>

namespace ufo
{
template <std::size_t Dim>
struct OccupancyDirElement {
	using logit_t = float;

	std::array<logit_t, 2u * Dim> logit{};
	bool                          contains_unknown{};
	bool                          contains_free{};
	bool                          contains_occupied{};

	constexpr OccupancyDirElement() noexcept = default;

	constexpr OccupancyDirElement(std::array<logit_t, 2u * Dim> const& logit,
	                              bool contains_unknown, bool contains_free,
	                              bool contains_occupied) noexcept
	    : logit(logit)
	    , contains_unknown(contains_unknown)
	    , contains_free(contains_free)
	    , contains_occupied(contains_occupied)
	{
	}

	constexpr OccupancyDirElement(logit_t logit, bool contains_unknown, bool contains_free,
	                              bool contains_occupied) noexcept
	    : logit(createArray<2u * Dim>(logit))
	    , contains_unknown(contains_unknown)
	    , contains_free(contains_free)
	    , contains_occupied(contains_occupied)
	{
	}

	[[nodiscard]] logit_t logitMax() const
	{
		return *std::max_element(logit.begin(), logit.end());
	}

	friend constexpr bool operator==(OccupancyDirElement const& lhs,
	                                 OccupancyDirElement const& rhs)
	{
		return lhs.logit == rhs.logit && lhs.contains_unknown == rhs.contains_unknown &&
		       lhs.contains_free == rhs.contains_free &&
		       lhs.contains_occupied == rhs.contains_occupied;
	}

	friend constexpr bool operator!=(OccupancyDirElement const& lhs,
	                                 OccupancyDirElement const& rhs)
	{
		return !(lhs == rhs);
	};
};

template <std::size_t Dim, std::size_t BF>
struct OccupancyDirBlock {
	using logit_t = typename OccupancyDirElement<Dim>::logit_t;

	std::array<OccupancyDirElement<Dim>, BF> data;

	constexpr OccupancyDirBlock() = default;

	constexpr OccupancyDirBlock(OccupancyDirElement<Dim> const& parent)
	    : data(createArray<BF>(parent))
	{
	}

	[[nodiscard]] constexpr OccupancyDirElement<Dim>& operator[](std::size_t pos)
	{
		assert(BF > pos);
		return data[pos];
	}

	[[nodiscard]] constexpr OccupancyDirElement<Dim> const& operator[](
	    std::size_t pos) const
	{
		assert(BF > pos);
		return data[pos];
	}

	auto begin() { return data.begin(); }

	auto begin() const { return data.begin(); }

	auto cbegin() const { return begin(); }

	auto end() { return data.end(); }

	auto end() const { return data.end(); }

	auto cend() const { return end(); }

	friend constexpr bool operator==(OccupancyDirBlock const& lhs,
	                                 OccupancyDirBlock const& rhs)
	{
		return lhs.data == rhs.data;
	}

	friend constexpr bool operator!=(OccupancyDirBlock const& lhs,
	                                 OccupancyDirBlock const& rhs)
	{
		return !(lhs == rhs);
	};
};
}  // namespace ufo
#endif  // UFO_MAP_OCCUPANCY_DIRECTIONAL_BLOCK_HPP