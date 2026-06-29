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

#ifndef UFO_GEOMETRY_SPHERE_HPP
#define UFO_GEOMETRY_SPHERE_HPP

// UFO
#include <ufo/math/vec.hpp>

// STL
#include <cstddef>
#include <iostream>
#include <type_traits>

namespace ufo
{
/*!
 * @brief Something something
 * @author Daniel Duberg
 *
 */
template <std::size_t Dim = 3, class T = float>
struct Sphere {
	static_assert(std::is_floating_point_v<T>, "T is required to be floating point.");

	using value_type = T;

	Vec<Dim, T> center;
	T           radius;

	constexpr Sphere() noexcept = default;

	constexpr Sphere(Vec<Dim, T> const& center, T radius) noexcept
	    : center(center), radius(radius)
	{
	}

	constexpr Sphere(Sphere const&) noexcept = default;

	template <class U>
	constexpr explicit Sphere(Sphere<Dim, U> const& other) noexcept
	    : center(other.center), radius(other.radius)
	{
	}
};

//
// Deduction guide
//

template <std::size_t Dim, class T>
Sphere(Vec<Dim, T>, T) -> Sphere<Dim, T>;

/*!
 * @brief Compare two Spheres.
 *
 * @param lhs,rhs The Spheres to compare
 * @return `true` if they compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator==(Sphere<Dim, T> const& lhs, Sphere<Dim, T> const& rhs)
{
	return lhs.center == rhs.center && lhs.radius == rhs.radius;
}

/*!
 * @brief Compare two Spheres.
 *
 * @param lhs,rhs The Spheres to compare
 * @return `true` if they do not compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator!=(Sphere<Dim, T> const& lhs, Sphere<Dim, T> const& rhs)
{
	return !(lhs == rhs);
}

template <std::size_t Dim, class T>
std::ostream& operator<<(std::ostream& out, Sphere<Dim, T> const& sphere)
{
	return out << "Center: " << sphere.center << ", Radius: " << sphere.radius;
}

template <class T>
using Sphere1 = Sphere<1, T>;
template <class T>
using Sphere2 = Sphere<2, T>;
template <class T>
using Sphere3 = Sphere<3, T>;
template <class T>
using Sphere4 = Sphere<4, T>;

using Sphere1f = Sphere<1, float>;
using Sphere2f = Sphere<2, float>;
using Sphere3f = Sphere<3, float>;
using Sphere4f = Sphere<4, float>;

using Sphere1d = Sphere<1, double>;
using Sphere2d = Sphere<2, double>;
using Sphere3d = Sphere<3, double>;
using Sphere4d = Sphere<4, double>;
}  // namespace ufo

#endif  // UFO_GEOMETRY_SPHERE_HPP