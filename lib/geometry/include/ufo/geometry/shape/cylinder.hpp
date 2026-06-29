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

#ifndef UFO_GEOMETRY_SHAPE_CYLINDER_HPP
#define UFO_GEOMETRY_SHAPE_CYLINDER_HPP

// UFO
#include <ufo/math/vec.hpp>

// STL
#include <cstddef>

namespace ufo
{
/*!
 * @brief Something something
 * @author Daniel Duberg
 *
 */
template <std::size_t Dim = 3, class T = float>
struct Cylinder {
	using value_type = T;

	Vec<Dim, T> center_1;
	Vec<Dim, T> center_2;
	T           radius;

	constexpr Cylinder() noexcept                = default;
	constexpr Cylinder(Cylinder const&) noexcept = default;

	constexpr Cylinder(Vec<Dim, T> center_1, Vec<Dim, T> center_2, T radius) noexcept
	    : center_1(center_1), center_2(center_2), radius(radius)
	{
	}

	template <class U>
	constexpr explicit Cylinder(Cylinder<Dim, U> const& other) noexcept
	    : center_1(other.center_1), center_2(other.center_2), radius(other.radius)
	{
	}
};

using Cylinder1 = Cylinder<1, float>;
using Cylinder2 = Cylinder<2, float>;
using Cylinder3 = Cylinder<3, float>;
using Cylinder4 = Cylinder<4, float>;

using Cylinder1d = Cylinder<1, double>;
using Cylinder2d = Cylinder<2, double>;
using Cylinder3d = Cylinder<3, double>;
using Cylinder4d = Cylinder<4, double>;

/*!
 * @brief Compare two BSs.
 *
 * @param lhs,rhs The BSs to compare
 * @return `true` if they compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator==(Cylinder<Dim, T> const& lhs, Cylinder<Dim, T> const& rhs)
{
	return lhs.center_1 == rhs.center_1 && lhs.center_2 == rhs.center_2 &&
	       lhs.radius == rhs.radius;
}

/*!
 * @brief Compare two BSs.
 *
 * @param lhs,rhs The BSs to compare
 * @return `true` if they do not compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator!=(Cylinder<Dim, T> const& lhs, Cylinder<Dim, T> const& rhs)
{
	return !(lhs == rhs);
}
}  // namespace ufo

#endif  // UFO_GEOMETRY_SHAPE_BOUNDING_SPHERE_HPP