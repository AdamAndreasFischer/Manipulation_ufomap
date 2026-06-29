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

#ifndef UFO_CONTAINER_TREE_COORD_HPP
#define UFO_CONTAINER_TREE_COORD_HPP

// UFO
#include <ufo/math/detail/vec.hpp>
#include <ufo/utility/type_traits.hpp>

// STL
#include <cstddef>
#include <ostream>
#include <type_traits>

namespace ufo
{
template <std::size_t Dim, class T = float>
struct TreeCoord : public Vec<Dim, T> {
	using Point   = Vec<Dim, T>;
	using coord_t = typename Point::value_type;
	using depth_t = unsigned;

	depth_t depth{};

	constexpr TreeCoord() noexcept                       = default;
	constexpr TreeCoord(TreeCoord const& ohter) noexcept = default;
	constexpr TreeCoord(TreeCoord&&) noexcept            = default;

	template <std::size_t D, class U>
	constexpr TreeCoord(TreeCoord<D, U> const& other) noexcept
	    : Vec<Dim, T>(static_cast<Vec<D, U> const&>(other)), depth(other.depth)
	{
	}

	constexpr TreeCoord& operator=(TreeCoord const&) noexcept = default;
	constexpr TreeCoord& operator=(TreeCoord&&) noexcept      = default;

	template <std::size_t D, class U>
	constexpr TreeCoord& operator=(TreeCoord<D, U> const& rhs) noexcept
	{
		static_cast<Vec<Dim, T>&>(*this) = static_cast<Vec<D, U> const&>(rhs);
		depth                            = rhs.depth;
		return *this;
	}

	constexpr explicit TreeCoord(T value) noexcept : Point(value) {}

	template <std::size_t D, class U>
	constexpr explicit TreeCoord(Vec<D, U> const& coord) noexcept : Point(coord)
	{
	}

	template <std::size_t D, class U>
	constexpr TreeCoord(Vec<D, U> const& coord, depth_t depth) noexcept
	    : Point(coord), depth(depth)
	{
	}

	template <class... Args,
	          std::enable_if_t<Dim == sizeof...(Args) && (std::is_scalar_v<Args> && ...),
	                           bool> = true>
	constexpr TreeCoord(Args&&... args) noexcept : Point(std::forward<Args>(args)...)
	{
	}

	template <class... Args, std::enable_if_t<Dim + 1 == sizeof...(Args) &&
	                                              (std::is_scalar_v<Args> && ...),
	                                          bool> = true>
	constexpr TreeCoord(Args&&... args) noexcept
	    : TreeCoord(std::integral_constant<std::size_t, Dim>{}, std::forward<Args>(args)...)
	{
	}

 private:
	template <std::size_t NumTimes, class First, class... Rest>
	constexpr TreeCoord(std::integral_constant<std::size_t, NumTimes>, First&& first,
	                    Rest&&... rest) noexcept
	    : TreeCoord(std::integral_constant<std::size_t, NumTimes - 1>{},
	                std::forward<Rest>(rest)..., std::forward<First>(first))
	{
	}

	template <class Depth, class... Args>
	constexpr TreeCoord(std::integral_constant<std::size_t, 0>, Depth&& depth,
	                    Args&&... args) noexcept
	    : Point(std::forward<Args>(args)...), depth(std::forward<Depth>(depth))
	{
	}
};

//
// Deduction guide
//

template <std::size_t Dim, class T>
TreeCoord(Vec<Dim, T>) -> TreeCoord<Dim, T>;

template <std::size_t Dim, class T>
TreeCoord(Vec<Dim, T>, unsigned) -> TreeCoord<Dim, T>;

template <std::size_t Dim, class T>
constexpr bool operator==(TreeCoord<Dim, T> const& lhs, TreeCoord<Dim, T> const& rhs)
{
	return lhs.depth == rhs.depth &&
	       static_cast<Vec<Dim, T> const&>(lhs) == static_cast<Vec<Dim, T> const&>(rhs);
}

template <std::size_t Dim, class T>
constexpr bool operator!=(TreeCoord<Dim, T> const& lhs, TreeCoord<Dim, T> const& rhs)
{
	return !(lhs == rhs);
}

template <std::size_t Dim, class T>
std::ostream& operator<<(std::ostream& out, TreeCoord<Dim, T> const& tc)
{
	return out << static_cast<Vec<Dim, T> const&>(tc) << " d: " << tc.depth;
}

using BinaryCoord = TreeCoord<1, float>;
using QuadCoord   = TreeCoord<2, float>;
using OctCoord    = TreeCoord<3, float>;
using HexCoord    = TreeCoord<4, float>;

template <class T = float>
using Coord1 = TreeCoord<1, T>;
template <class T = float>
using Coord2 = TreeCoord<2, T>;
template <class T = float>
using Coord3 = TreeCoord<3, T>;
template <class T = float>
using Coord4 = TreeCoord<4, T>;

using Coord1f = TreeCoord<1, float>;
using Coord2f = TreeCoord<2, float>;
using Coord3f = TreeCoord<3, float>;
using Coord4f = TreeCoord<4, float>;

using Coord1d = TreeCoord<1, double>;
using Coord2d = TreeCoord<2, double>;
using Coord3d = TreeCoord<3, double>;
using Coord4d = TreeCoord<4, double>;
}  // namespace ufo

#endif  // UFO_CONTAINER_TREE_COORD_HPP