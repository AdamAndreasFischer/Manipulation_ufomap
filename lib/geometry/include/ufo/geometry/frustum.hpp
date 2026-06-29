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

#ifndef UFO_GEOMETRY_FRUSTUM_HPP
#define UFO_GEOMETRY_FRUSTUM_HPP

// UFO
#include <ufo/geometry/line.hpp>
#include <ufo/geometry/plane.hpp>
#include <ufo/math/vec.hpp>

// STL
#include <cmath>
#include <cstddef>
#include <ostream>
#include <type_traits>

namespace ufo
{
template <std::size_t Dim = 3, class T = float>
struct Frustum;

template <class T>
struct Frustum<2, T> {
	static_assert(std::is_floating_point_v<T>, "T is required to be floating point.");

	using value_type = T;

	Line<2, T> left;
	Line<2, T> right;
	Line<2, T> far;
	Line<2, T> near;

	constexpr Frustum() noexcept = default;

	constexpr Frustum(Vec<2, T> const& far_right, Vec<2, T> const& far_left,
	                  Vec<2, T> const& near_left, Vec<2, T> const& near_right)
	    : left(near_left, far_left)
	    , right(far_right, near_right)
	    , far(far_left, far_right)
	    , near(near_right, near_left)
	{
	}

	constexpr Frustum(Vec<2, T> const& pos, Vec<2, T> const& target, T fov, T near_dist,
	                  T far_dist)
	{
		auto      dir = normalize(target - pos);
		Vec<2, T> right_dir(dir.y, -dir.x);
		auto      half_fov        = fov * T(0.5);
		auto      half_width_near = near_dist * std::tan(half_fov);
		auto      half_width_far  = far_dist * std::tan(half_fov);

		Vec<2, T> far_right  = pos + dir * far_dist + half_width_far * right_dir;
		Vec<2, T> far_left   = pos + dir * far_dist - half_width_far * right_dir;
		Vec<2, T> near_left  = pos + dir * near_dist - half_width_near * right_dir;
		Vec<2, T> near_right = pos + dir * near_dist + half_width_near * right_dir;

		left  = Line<2, T>(near_left, far_left);
		right = Line<2, T>(far_right, near_right);
		far   = Line<2, T>(far_left, far_right);
		near  = Line<2, T>(near_right, near_left);
	}

	constexpr Frustum(Frustum const&) noexcept = default;

	template <class U>
	constexpr explicit Frustum(Frustum<2, U> const& other) noexcept
	    : left(other.left), right(other.right), far(other.far), near(other.near)
	{
	}

	[[nodiscard]] constexpr static std::size_t size() noexcept { return 2; }

	[[nodiscard]] constexpr Line<2, T>& operator[](std::size_t pos) noexcept
	{
		return (&left)[pos];
	}

	[[nodiscard]] constexpr Line<2, T> const& operator[](std::size_t pos) const noexcept
	{
		return (&left)[pos];
	}
};

//
// Deduction guide
//

template <class T>
struct Frustum<3, T> {
	static_assert(std::is_floating_point_v<T>, "T is required to be floating point.");

	using value_type = T;

	Plane<T> top;
	Plane<T> bottom;
	Plane<T> left;
	Plane<T> right;
	Plane<T> far;
	Plane<T> near;

	constexpr Frustum() noexcept = default;

	constexpr Frustum(Vec<3, T> const& far_top_right, Vec<3, T> const& far_top_left,
	                  Vec<3, T> const& far_bottom_left, Vec<3, T> const& far_bottom_right,
	                  Vec<3, T> const& near_top_right, Vec<3, T> const& near_top_left,
	                  Vec<3, T> const& near_bottom_left, Vec<3, T> const& near_bottom_right)
	{
		top    = Plane<T>(near_top_right, near_top_left, far_top_left);
		bottom = Plane<T>(near_bottom_left, near_bottom_right, far_bottom_right);
		left   = Plane<T>(near_top_left, near_bottom_left, far_bottom_left);
		right  = Plane<T>(near_bottom_right, near_top_right, far_bottom_right);
		near   = Plane<T>(near_top_left, near_top_right, near_bottom_right);
		far    = Plane<T>(far_top_right, far_top_left, far_bottom_left);
	}

	constexpr Frustum(Vec<3, T> const& pos, Vec<3, T> const& target, Vec<3, T> const& up,
	                  T vertical_fov, T horizontal_fov, T near_distance, T far_distance)
	{
		T ratio = horizontal_fov / vertical_fov;

		// FIXME: Check if correct
		T tang        = std::tan(vertical_fov * static_cast<T>(0.5));
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

		top    = Plane<T>(near_top_right, near_top_left, far_top_left);
		bottom = Plane<T>(near_bottom_left, near_bottom_right, far_bottom_right);
		left   = Plane<T>(near_top_left, near_bottom_left, far_bottom_left);
		right  = Plane<T>(near_bottom_right, near_top_right, far_bottom_right);
		near   = Plane<T>(near_top_left, near_top_right, near_bottom_right);
		far    = Plane<T>(far_top_right, far_top_left, far_bottom_left);
	}

	constexpr Frustum(Frustum const&) noexcept = default;

	template <class U>
	constexpr Frustum(Frustum<3, U> const& other) noexcept
	    : top(other.top)
	    , bottom(other.bottom)
	    , left(other.left)
	    , right(other.right)
	    , far(other.far)
	    , near(other.near)
	{
	}

	[[nodiscard]] constexpr static std::size_t size() noexcept { return 3; }
};

//
// Deduction guide
//

template <class T>
struct Frustum<4, T> {
	static_assert(std::is_floating_point_v<T>, "T is required to be floating point.");

	using value_type = T;

	// TODO: Good luck have fun!
};

//
// Deduction guide
//

/*!
 * @brief Compare two Frustums.
 *
 * @param lhs,rhs The Frustums to compare
 * @return `true` if they compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator==(Frustum<Dim, T> const& lhs, Frustum<Dim, T> const& rhs)
{
	if constexpr (2 == Dim) {
		return lhs.left == rhs.left && lhs.right == rhs.right && lhs.far == rhs.far &&
		       lhs.near == rhs.near;
	} else if constexpr (3 == Dim) {
		return lhs.top == rhs.top && lhs.bottom == rhs.bottom && lhs.left == rhs.left &&
		       lhs.right == rhs.right && lhs.far == rhs.far && lhs.near == rhs.near;
	} else if constexpr (4 == Dim) {
		// TODO: Implement
	}
	return false;
}

/*!
 * @brief Compare two Frustums.
 *
 * @param lhs,rhs The Frustums to compare
 * @return `true` if they do not compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator!=(Frustum<Dim, T> const& lhs, Frustum<Dim, T> const& rhs)
{
	return !(lhs == rhs);
}

template <std::size_t Dim, class T>
std::ostream& operator<<(std::ostream& out, Frustum<Dim, T> const& frustum)
{
	if constexpr (2 == Dim) {
		return out << "Left: " << frustum.left << ", Right: " << frustum.right
		           << ", Far: " << frustum.far << ", Near: " << frustum.near;
	} else if constexpr (3 == Dim) {
		return out << "Left: " << frustum.left << ", Right: " << frustum.right
		           << ", Far: " << frustum.far << ", Near: " << frustum.near
		           << ", Top: " << frustum.top << ", Bottom: " << frustum.bottom;
	} else if constexpr (4 == Dim) {
		// TODO: Implement
	}
	return out;
}

template <class T>
using Frustum2 = Frustum<2, T>;
template <class T>
using Frustum3 = Frustum<3, T>;
template <class T>
using Frustum4 = Frustum<4, T>;

using Frustum2f = Frustum<2, float>;
using Frustum3f = Frustum<3, float>;
using Frustum4f = Frustum<4, float>;

using Frustum2d = Frustum<2, double>;
using Frustum3d = Frustum<3, double>;
using Frustum4d = Frustum<4, double>;
}  // namespace ufo

#endif  // UFO_GEOMETRY_FRUSTUM_HPP