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

#ifndef UFO_GEOMETRY_SHAPE_CONE_HPP
#define UFO_GEOMETRY_SHAPE_CONE_HPP

// UFO
#include <ufo/math/vec.hpp>

// STL
#include <cstddef>

namespace ufo
{
template <std::size_t Dim, class T = float>
struct Cone {
	Vec<Dim, T> base_center;
	Vec<Dim, T> tip;
	T           radius;
	constexpr Cone() noexcept            = default;
	constexpr Cone(Cone const&) noexcept = default;

	constexpr Cone(Vec<Dim, T> base_center, Vec<Dim, T> tip, T radius) noexcept
	    : base_center(base_center), tip(tip), radius(radius)
	{
	}

	template <class U>
	constexpr explicit Cone(Cone<Dim, U> const& other) noexcept
	    : base_center(other.base_center), tip(other.tip), radius(other.radius)
	{
	}
};

using Cone1 = Cone<1, float>;
using Cone2 = Cone<2, float>;
using Cone3 = Cone<3, float>;
using Cone4 = Cone<4, float>;

using Cone1d = Cone<1, double>;
using Cone2d = Cone<2, double>;
using Cone3d = Cone<3, double>;
using Cone4d = Cone<4, double>;

/*!
 * @brief Compare two BSs.
 *
 * @param lhs,rhs The BSs to compare
 * @return `true` if they compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator==(Cone<Dim, T> const& lhs, Cone<Dim, T> const& rhs)
{
	return lhs.base_center == rhs.base_center && lhs.tip == rhs.tip &&
	       lhs.radius == rhs.radius;
}

/*!
 * @brief Compare two BSs.
 *
 * @param lhs,rhs The BSs to compare
 * @return `true` if they do not compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator!=(Cone<Dim, T> const& lhs, Cone<Dim, T> const& rhs)
{
	return !(lhs == rhs);
}
}  // namespace ufo

#endif  // UFO_GEOMETRY_SHAPE_CONE_HPP
