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

#ifndef UFO_GEOMETRY_SHAPE_FRUSTUM_HPP
#define UFO_GEOMETRY_SHAPE_FRUSTUM_HPP

// UFO
#include <ufo/geometry/plane.hpp>
#include <ufo/math/vec.hpp>

// STL
#include <cmath>
#include <cstddef>

namespace ufo
{
template <class T>
struct Frustum {
	Plane<T> top;
	Plane<T> bottom;
	Plane<T> left;
	Plane<T> right;
	Plane<T> far;
	Plane<T> near;

	constexpr Frustum() noexcept               = default;
	constexpr Frustum(Frustum const&) noexcept = default;

	constexpr Frustum(Vec<3, T> pos, Vec<3, T> target, Vec<3, T> up, T vertical_angle,
	                  T horizontal_angle, T near_distance, T far_distance) noexcept
	// : position(pos)
	// , target(target)
	// , up(up)
	// , vertical_angle(vertical_angle)
	// , horizontal_angle(horizontal_angle)
	// , near_distance(near_distance)
	// , far_distance(far_distance)
	{
		T ratio = horizontal_angle / vertical_angle;

		// FIXME: Check if correct
		T tang        = std::tan(vertical_angle * static_cast<T>(0.5));
		T near_height = near_distance * tang;
		T near_width  = near_height * ratio;
		T far_height  = far_distance * tang;
		T far_width   = far_height * ratio;

		auto Z = normalize(pos - target);

		auto X = normalize(cross(up, Z));

		auto Y = cross(Z, X);

		auto nc = pos - Z * near_distance;
		auto fc = pos - Z * far_distance;

		auto near_top_left     = nc + Y * near_height - X * near_width;
		auto near_top_right    = nc + Y * near_height + X * near_width;
		auto near_bottom_left  = nc - Y * near_height - X * near_width;
		auto near_bottom_right = nc - Y * near_height + X * near_width;

		auto far_top_left     = fc + Y * far_height - X * far_width;
		auto far_top_right    = fc + Y * far_height + X * far_width;
		auto far_bottom_left  = fc - Y * far_height - X * far_width;
		auto far_bottom_right = fc - Y * far_height + X * far_width;

		top    = Plane(near_top_right, near_top_left, far_top_left);
		bottom = Plane(near_bottom_left, near_bottom_right, far_bottom_right);
		left   = Plane(near_top_left, near_bottom_left, far_bottom_left);
		right  = Plane(near_bottom_right, near_top_right, far_bottom_right);
		near   = Plane(near_top_left, near_top_right, near_bottom_right);
		far    = Plane(far_top_right, far_top_left, far_bottom_left);
	}

	template <class U>
	constexpr Frustum(Frustum<U> const& other) noexcept
	    : top(other.top)
	    , bottom(other.bottom)
	    , left(other.left)
	    , right(other.right)
	    , far(other.far)
	    , near(other.near)
	{
	}

	[[nodiscard]] constexpr Plane<T>& operator[](std::size_t pos) noexcept
	{
		return (&top)[pos];
	}

	[[nodiscard]] constexpr Plane<T> const& operator[](std::size_t pos) const noexcept
	{
		return (&top)[pos];
	}

	//  private:
	// 	Point position;
	// 	Point target;
	// 	Point up;
	// 	float vertical_angle{};
	// 	float horizontal_angle{};
	// 	float near_distance{};
	// 	float far_distance{};
};

/*!
 * @brief Compare two Frustums.
 *
 * @param lhs,rhs The Frustums to compare
 * @return `true` if they compare equal, `false` otherwise.
 */
template <class T>
bool operator==(Frustum<T> const& lhs, Frustum<T> const& rhs)
{
	return lhs.top == rhs.top && lhs.bottom == rhs.bottom && lhs.left == rhs.left &&
	       lhs.right == rhs.right && lhs.far == rhs.far && lhs.near == rhs.near;
}

/*!
 * @brief Compare two Frustums.
 *
 * @param lhs,rhs The Frustums to compare
 * @return `true` if they do not compare equal, `false` otherwise.
 */
template <class T>
bool operator!=(Frustum<T> const& lhs, Frustum<T> const& rhs)
{
	return !(lhs == rhs);
}
}  // namespace ufo

#endif  // UFO_GEOMETRY_SHAPE_FRUSTUM_HPP