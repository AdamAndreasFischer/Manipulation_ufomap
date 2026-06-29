/*!
 * UFOMap: An Efficient Probabilistic 3D Mapping Framework That Embraces the Unknown
 *
 * @author Daniel Duberg (dduberg@kth.se), Ramona Häuselmann (ramonaha@kth.se)
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

#ifndef UFO_GEOMETRY_LINE_HPP
#define UFO_GEOMETRY_LINE_HPP

// UFO
#include <limits>
#include <ufo/math/vec.hpp>

namespace ufo
{

template <std::size_t Dim = 3, class T = float>
struct Line;

template <class T>
struct Line<2, T> {
	Vec<2, T> normal;
	T         distance{};

	constexpr Line() noexcept            = default;
	constexpr Line(Line const&) noexcept = default;

	constexpr explicit Line(Vec<2, T> normal) noexcept : normal(normal) {}

	constexpr Line(Vec<2, T> normal, T distance) noexcept
	    : normal(normal), distance(distance)
	{
	}

	constexpr Line(Vec<2, T> v_1, Vec<2, T> v_2) noexcept
	{
		auto aux_1 = v_2 - v_1;
		normal     = normalize(Vec<2, T>(-aux_1.y, aux_1.x));
		distance   = dot(normal, v_1);
	}

	template <class U>
	constexpr explicit Line(Line<2, U> const& other) noexcept
	    : normal(other.normal), distance(static_cast<T>(other.distance))
	{
	}
};

// TODO: Where to put this?
template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> intersectionPoint(Line<Dim, T> const& a,
                                                 Line<Dim, T> const& b)
{
	if constexpr (2 == Dim) {
		// LOOKAT: What if they are parallel, then det is also 0
		auto det = a.normal.x * b.normal.y - a.normal.y * b.normal.x;
		return det != 0 ? 1 / det *
		                      Vec<2, T>((a.distance * b.normal.y - b.distance * a.normal.y),
		                                (b.distance * a.normal.x - a.distance * b.normal.x))
		                : Vec<2, T>(std::numeric_limits<T>::infinity());
	} else if constexpr (3 == Dim) {
		// TODO: Implement
	} else if constexpr (4 == Dim) {
		// TODO: Implement
	} else {
		// Error
	}
}

/*!
 * @brief Compare two Lines.
 *
 * @param lhs,rhs The Lines to compare
 * @return `true` if they compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator==(Line<Dim, T> const& lhs, Line<Dim, T> const& rhs)
{
	return (lhs.normal == rhs.normal && lhs.distance == rhs.distance) ||
	       (lhs.normal == -rhs.normal && lhs.distance == -rhs.distance);
}

/*!
 * @brief Compare two Lines.
 *
 * @param lhs,rhs The Lines to compare
 * @return `true` if they do not compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator!=(Line<Dim, T> const& lhs, Line<Dim, T> const& rhs)
{
	return !(lhs == rhs);
}

template <class T>
using Line1 = Line<1, T>;
template <class T>
using Line2 = Line<2, T>;
template <class T>
using Line3 = Line<3, T>;
template <class T>
using Line4 = Line<4, T>;

using Line1f = Line<1, float>;
using Line2f = Line<2, float>;
using Line3f = Line<3, float>;
using Line4f = Line<4, float>;

using Line1d = Line<1, double>;
using Line2d = Line<2, double>;
using Line3d = Line<3, double>;
using Line4d = Line<4, double>;
}  // namespace ufo

#endif  // UFO_GEOMETRY_LINE_HPP