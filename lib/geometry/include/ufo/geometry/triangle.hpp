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

#ifndef UFO_GEOMETRY_TRIANGLE_HPP
#define UFO_GEOMETRY_TRIANGLE_HPP

// UFO
#include <ufo/math/vec.hpp>

// STL
#include <array>
#include <cstddef>

namespace ufo
{
template <std::size_t Dim = 3, class T = float>
struct Triangle {
	using value_type = T;

	std::array<Vec<Dim, T>, 3> points;

	constexpr Triangle() noexcept                = default;
	constexpr Triangle(Triangle const&) noexcept = default;

	constexpr Triangle(Vec<Dim, T> point_1, Vec<Dim, T> point_2,
	                   Vec<Dim, T> point_3) noexcept
	    : points{point_1, point_2, point_3}
	{
	}
	template <class U>
	constexpr explicit Triangle(Triangle<Dim, U> const& other) noexcept
	    : points{Vec<Dim, T>(other[0]), Vec<Dim, T>(other[1]), Vec<Dim, T>(other[2])}
	{
	}

	[[nodiscard]] constexpr Vec<Dim, T>& operator[](std::size_t pos) noexcept
	{
		return points[pos];
	}

	[[nodiscard]] constexpr Vec<Dim, T> const& operator[](std::size_t pos) const noexcept
	{
		return points[pos];
	}
};

using Triangle2 = Triangle<2, float>;
using Triangle3 = Triangle<3, float>;
using Triangle4 = Triangle<4, float>;

using Triangle2d = Triangle<2, double>;
using Triangle3d = Triangle<3, double>;
using Triangle4d = Triangle<4, double>;

/*!
 * @brief Compare two Triangles.
 *
 * @param lhs,rhs The Triangles to compare
 * @return `true` if they compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator==(Triangle<Dim, T> const& lhs, Triangle<Dim, T> const& rhs)
{
	return lhs.points == rhs.points;
}

/*!
 * @brief Compare two Triangles.
 *
 * @param lhs,rhs The Triangles to compare
 * @return `true` if they do not compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator!=(Triangle<Dim, T> const& lhs, Triangle<Dim, T> const& rhs)
{
	return !(lhs == rhs);
}
}  // namespace ufo

#endif  // UFO_GEOMETRY_TRIANGLE_HPP