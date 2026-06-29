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

#ifndef UFO_GEOMETRY_PLANE_HPP
#define UFO_GEOMETRY_PLANE_HPP

// UFO
#include <ufo/math/vec.hpp>

namespace ufo
{
template <class T = float>
struct Plane {
	Vec<3, T> normal;
	T         distance{};

	constexpr Plane() noexcept             = default;
	constexpr Plane(Plane const&) noexcept = default;

	constexpr explicit Plane(Vec<3, T> const& normal) noexcept : normal(normal) {}

	constexpr Plane(Vec<3, T> const& normal, T distance) noexcept
	    : normal(normal), distance(distance)
	{
	}

	constexpr Plane(Vec<3, T> const& v_1, Vec<3, T> const& v_2,
	                Vec<3, T> const& v_3) noexcept
	{
		auto aux_1 = v_1 - v_2;
		auto aux_2 = v_3 - v_2;
		normal     = normalize(cross(aux_2, aux_1));
		distance   = -dot(normal, v_2);
	}

	constexpr Plane& operator=(Plane const&) noexcept = default;
};

/*!
 * @brief Compare two Planes.
 *
 * @param lhs,rhs The Planes to compare
 * @return `true` if they compare equal, `false` otherwise.
 */
template <class T>
bool operator==(Plane<T> const& lhs, Plane<T> const& rhs)
{
	return (lhs.normal == rhs.normal && lhs.distance == rhs.distance) ||
	       (lhs.normal == -rhs.normal && lhs.distance == -rhs.distance);
}

/*!
 * @brief Compare two Planes.
 *
 * @param lhs,rhs The Planes to compare
 * @return `true` if they do not compare equal, `false` otherwise.
 */
template <class T>
bool operator!=(Plane<T> const& lhs, Plane<T> const& rhs)
{
	return !(lhs == rhs);
}
}  // namespace ufo

#endif  // UFO_GEOMETRY_PLANE_HPP