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

#ifndef UFO_GEOMETRY_FUN_HPP
#define UFO_GEOMETRY_FUN_HPP

// UFO
#include <ufo/geometry/aabb.hpp>
#include <ufo/geometry/capsule.hpp>
#include <ufo/geometry/frustum.hpp>
#include <ufo/geometry/line_segment.hpp>
#include <ufo/geometry/obb.hpp>
#include <ufo/geometry/plane.hpp>
#include <ufo/geometry/ray.hpp>
#include <ufo/geometry/sphere.hpp>
#include <ufo/geometry/triangle.hpp>
#include <ufo/math/math.hpp>
#include <ufo/math/vec.hpp>

// STL
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>

namespace ufo
{
/**************************************************************************************
|                                                                                     |
|                                         Min                                         |
|                                                                                     |
**************************************************************************************/

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> min(AABB<Dim, T> const& a)
{
	return a.min;
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> min(Sphere<Dim, T> const& a)
{
	return a.center - a.radius;
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> min(Capsule<Dim, T> const& a)
{
	return min(a.start, a.end) - a.radius;
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> min(Frustum<Dim, T> const& a)
{
	auto        c = corners(a);
	Vec<Dim, T> v_min(std::numeric_limits<T>::max());

	for (auto const& v : c) {
		v_min = min(v_min, v);
	}

	return v_min;
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> min(LineSegment<Dim, T> const& a)
{
	return min(a.start, a.end);
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> min(OBB<Dim, T> const& a)
{
	// TODO: Implement correct
	return a.center - abs(a.rotatedHalfLength());
}

template <class T>
[[nodiscard]] constexpr Vec<3, T> min(Plane<T> const& a)
{
	// TODO: Implement
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> min(Ray<Dim, T> const& a)
{
	Vec<Dim, T> res;
	for (std::size_t i{}; Dim > i; ++i) {
		if constexpr (std::numeric_limits<T>::has_infinity) {
			res[i] = 0 <= a.direction[i] ? a.origin[i] : -std::numeric_limits<T>::infinity();
		} else {
			res[i] = 0 <= a.direction[i] ? a.origin[i] : std::numeric_limits<T>::lowest();
		}
	}
	return res;
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> min(Triangle<Dim, T> const& a)
{
	return min(a[0], min(a[1], a[2]));
}

/**************************************************************************************
|                                                                                     |
|                                         Max                                         |
|                                                                                     |
**************************************************************************************/

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> max(AABB<Dim, T> const& a)
{
	return a.max;
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> max(Sphere<Dim, T> const& a)
{
	return a.center + a.radius;
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> max(Capsule<Dim, T> const& a)
{
	return max(a.start, a.end) + a.radius;
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> max(Frustum<Dim, T> const& a)
{
	auto        c = corners(a);
	Vec<Dim, T> v_max(std::numeric_limits<T>::lowest());

	for (auto const& v : c) {
		v_max = max(v_max, v);
	}

	return v_max;
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> max(LineSegment<Dim, T> const& a)
{
	return max(a.start, a.end);
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> max(OBB<Dim, T> const& a)
{
	// TODO: Implement correct
	return a.center + abs(a.rotatedHalfLength());
}

template <class T>
[[nodiscard]] constexpr Vec<3, T> max(Plane<T> const& a)
{
	// TODO: Implement
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> max(Ray<Dim, T> const& a)
{
	Vec<Dim, T> res;
	for (std::size_t i{}; Dim > i; ++i) {
		if constexpr (std::numeric_limits<T>::has_infinity) {
			res[i] = 0 >= a.direction[i] ? a.origin[i] : std::numeric_limits<T>::infinity();
		} else {
			res[i] = 0 >= a.direction[i] ? a.origin[i] : std::numeric_limits<T>::max();
		}
	}
	return res;
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> max(Triangle<Dim, T> const& a)
{
	return max(a[0], max(a[1], a[2]));
}

/**************************************************************************************
|                                                                                     |
|                                       Corners                                       |
|                                                                                     |
**************************************************************************************/

template <std::size_t Dim, class T>
[[nodiscard]] constexpr std::array<Vec<Dim, T>, ipow(2, Dim)> corners(
    AABB<Dim, T> const& a)
{
	auto min = ufo::min(a);
	auto max = ufo::max(a);
	if constexpr (1 == Dim) {
		return {min, max};
	} else if constexpr (2 == Dim) {
		// clang-format off
		return {Vec<Dim, T>(min[0], min[1]), 
		        Vec<Dim, T>(max[0], min[1]), 
						Vec<Dim, T>(min[0], max[1]), 
						Vec<Dim, T>(max[0], max[1])};
		// clang-format on
	} else if constexpr (3 == Dim) {
		// clang-format off
		return {Vec<Dim, T>(min[0], min[1], min[2]), 
		        Vec<Dim, T>(max[0], min[1], min[2]), 
						Vec<Dim, T>(min[0], max[1], min[2]),
		        Vec<Dim, T>(max[0], max[1], min[2]), 
		        Vec<Dim, T>(min[0], min[1], max[2]), 
		        Vec<Dim, T>(max[0], min[1], max[2]), 
		        Vec<Dim, T>(min[0], max[1], max[2]),
		        Vec<Dim, T>(max[0], max[1], max[2])};
		// clang-format on
	} else if constexpr (4 == Dim) {
		// clang-format off
		return {Vec<Dim, T>(min[0], min[1], min[2], min[3]), 
		        Vec<Dim, T>(max[0], min[1], min[2], min[3]), 
						Vec<Dim, T>(min[0], max[1], min[2], min[3]),
		        Vec<Dim, T>(max[0], max[1], min[2], min[3]), 
		        Vec<Dim, T>(min[0], min[1], max[2], min[3]), 
		        Vec<Dim, T>(max[0], min[1], max[2], min[3]), 
		        Vec<Dim, T>(min[0], max[1], max[2], min[3]),
		        Vec<Dim, T>(max[0], max[1], max[2], min[3]),
						Vec<Dim, T>(min[0], min[1], min[2], max[3]), 
		        Vec<Dim, T>(max[0], min[1], min[2], max[3]), 
						Vec<Dim, T>(min[0], max[1], min[2], max[3]),
		        Vec<Dim, T>(max[0], max[1], min[2], max[3]), 
		        Vec<Dim, T>(min[0], min[1], max[2], max[3]), 
		        Vec<Dim, T>(max[0], min[1], max[2], max[3]), 
		        Vec<Dim, T>(min[0], max[1], max[2], max[3]),
		        Vec<Dim, T>(max[0], max[1], max[2], max[3])};
		// clang-format on
	} else {
		// Error
	}
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr std::array<Vec<Dim, T>, ipow(2, Dim)> corners(
    Frustum<Dim, T> const& a)
{
	if constexpr (2 == Dim) {
		return {intersectionPoint(a.far, a.right), intersectionPoint(a.far, a.left),
		        intersectionPoint(a.near, a.left), intersectionPoint(a.near, a.right)};
	} else if constexpr (3 == Dim) {
		// TODO: Implement
	} else if constexpr (4 == Dim) {
		// TODO: Implement
	} else {
		// Error
	}
}

template <std::size_t Dim, class T>
[[nodiscard]] constexpr std::array<Vec<Dim, T>, ipow(2, Dim)> corners(
    OBB<Dim, T> const& a)
{
	// TODO: Implement
}
}  // namespace ufo

#endif  // UFO_GEOMETRY_FUN_HPP