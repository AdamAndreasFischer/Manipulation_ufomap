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

#ifndef UFO_GEOMETRY_CLOSEST_POINT_HPP
#define UFO_GEOMETRY_CLOSEST_POINT_HPP

// UFO
#include <ufo/geometry/aabb.hpp>
#include <ufo/geometry/capsule.hpp>
#include <ufo/geometry/line_segment.hpp>
#include <ufo/geometry/obb.hpp>
#include <ufo/geometry/plane.hpp>
#include <ufo/geometry/ray.hpp>
#include <ufo/geometry/sphere.hpp>
#include <ufo/geometry/triangle.hpp>
#include <ufo/math/vec.hpp>

// STL
#include <cmath>

namespace ufo
{
/**************************************************************************************
|                                                                                     |
|                                        AABB                                         |
|                                                                                     |
**************************************************************************************/

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(AABB<Dim, T> const& a,
//                                                  AABB<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(AABB<Dim, T> const& a,
//                                                  Sphere<Dim, T> const&   b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(AABB<Dim, T> const&    a,
//                                                  Capsule<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(AABB<3, T> const& a, Frustum<T> const&
// b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(AABB<Dim, T> const&        a,
//                                                  LineSegment<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(AABB<Dim, T> const& a,
//                                                  OBB<Dim, T> const&  b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(AABB<3, T> const& a, Plane<T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(AABB<Dim, T> const& a,
//                                                  Ray<Dim, T> const&  b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(AABB<Dim, T> const&     a,
//                                                  Triangle<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> closestPoint(AABB<Dim, T> const& a,
                                                 Vec<Dim, T> const&  b)
{
	return clamp(b, min(a), max(a));
}

/**************************************************************************************
|                                                                                     |
|                                       Sphere                                        |
|                                                                                     |
**************************************************************************************/

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Sphere<Dim, T> const&   a,
//                                                  AABB<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Sphere<Dim, T> const& a, Sphere<Dim, T> const&
// b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Sphere<Dim, T> const&      a,
//                                                  Capsule<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(AABB<3, T> const& a, Frustum<T> const&
// b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Sphere<Dim, T> const&          a,
//                                                  LineSegment<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Sphere<Dim, T> const&  a,
//                                                  OBB<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(AABB<3, T> const& a, Plane<T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Sphere<Dim, T> const&  a,
//                                                  Ray<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(AABB<Dim, T> const&     a,
//                                                  Triangle<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> closestPoint(Sphere<Dim, T> const&  a,
                                                 Vec<Dim, T> const& b)
{
	return a.center + normalize(b - a.center) * a.radius;
}

/**************************************************************************************
|                                                                                     |
|                                       Capsule                                       |
|                                                                                     |
**************************************************************************************/

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Capsule<Dim, T> const& a,
//                                                  AABB<Dim, T> const&    b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Capsule<Dim, T> const& a,
//                                                  Sphere<Dim, T> const&      b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Capsule<Dim, T> const& a,
//                                                  Capsule<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(AABB<3, T> const& a, Frustum<T> const&
// b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Capsule<Dim, T> const&     a,
//                                                  LineSegment<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Capsule<Dim, T> const& a,
//                                                  OBB<Dim, T> const&     b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Capsule<3, T> const& a, Plane<T> const&
// b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Capsule<Dim, T> const& a,
//                                                  Ray<Dim, T> const&     b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Capsule<Dim, T> const&  a,
//                                                  Triangle<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Capsule<Dim, T> const& a,
//                                                  Vec<Dim, T> const&     b)
// {
// 	// TODO: Implement
// }

/**************************************************************************************
|                                                                                     |
|                                       Frustum                                       |
|                                                                                     |
**************************************************************************************/

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Frustum<T> const& a, AABB<3, T> const&
// b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Frustum<T> const& a, Sphere<3, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Frustum<T> const&    a,
//                                                Capsule<3, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Frustum<T> const& a, Frustum<T> const&
// b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Frustum<T> const&        a,
//                                                LineSegment<3, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Frustum<T> const& a, OBB<3, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Frustum<T> const& a, Plane<T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Frustum<T> const& a, Ray<3, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Frustum<T> const&     a,
//                                                Triangle<3, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Frustum<T> const& a, Vec<3, T> const& b)
// {
// 	// TODO: Implement
// }

/**************************************************************************************
|                                                                                     |
|                                    Line segment                                     |
|                                                                                     |
**************************************************************************************/

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(LineSegment<Dim, T> const& a,
//                                                  AABB<Dim, T> const&        b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(LineSegment<Dim, T> const& a,
//                                                  Sphere<Dim, T> const&          b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(LineSegment<Dim, T> const& a,
//                                                  Capsule<Dim, T> const&     b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(LineSegment<3, T> const& a,
//                                                Frustum<T> const&        b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(LineSegment<Dim, T> const& a,
//                                                  LineSegment<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(LineSegment<Dim, T> const& a,
//                                                  OBB<Dim, T> const&         b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(LineSegment<3, T> const& a,
//                                                Plane<T> const&          b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(LineSegment<Dim, T> const& a,
//                                                  Ray<Dim, T> const&         b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(LineSegment<Dim, T> const& a,
//                                                  Triangle<Dim, T> const&    b)
// {
// 	// TODO: Implement
// }

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> closestPoint(LineSegment<Dim, T> const& a,
                                                 Vec<Dim, T> const&         b)
{
	auto dir = a.end - a.start;
	auto t   = dot(b - a.start, dir) / dot(dir, dir);
	return a.start + dir * std::clamp(t, T(0), T(1));
}

/**************************************************************************************
|                                                                                     |
|                                         OBB                                         |
|                                                                                     |
**************************************************************************************/

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(OBB<Dim, T> const&  a,
//                                                  AABB<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(OBB<Dim, T> const& a,
//                                                  Sphere<Dim, T> const&  b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(OBB<Dim, T> const&     a,
//                                                  Capsule<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(OBB<3, T> const& a, Frustum<T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(OBB<Dim, T> const&         a,
//                                                  LineSegment<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(OBB<Dim, T> const& a,
//                                                  OBB<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(OBB<3, T> const& a, Plane<T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(OBB<Dim, T> const& a,
//                                                  Ray<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(OBB<Dim, T> const&      a,
//                                                  Triangle<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(OBB<Dim, T> const& a,
//                                                  Vec<Dim, T> const& b)
// {
// 	// TODO: Implement

// 	// Point result = obb.center;
// 	// Point dir    = point - obb.center;

// 	// std::array<float, 9> obb_rot_matrix = obb.rotation.rotMatrix();

// 	// for (std::size_t i{}; 3 > i; ++i) {
// 	// 	Point axis(obb_rot_matrix[i * 3], obb_rot_matrix[(i * 3) + 1],
// 	// 	           obb_rot_matrix[(i * 3) + 2]);
// 	// 	float distance = Point::dot(dir, axis);
// 	// 	if (distance > obb.half_size[i]) {
// 	// 		distance = obb.half_size[i];
// 	// 	}
// 	// 	if (distance < -obb.half_size[i])  // FIXME: Should this be else if?
// 	// 	{
// 	// 		distance = -obb.half_size[i];
// 	// 	}
// 	// 	result = result + (axis * distance);
// 	// }
// 	// return result;
// }

/**************************************************************************************
|                                                                                     |
|                                        Plane                                        |
|                                                                                     |
**************************************************************************************/

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Plane<T> const& a, AABB<3, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Plane<T> const& a, Sphere<3, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Plane<T> const& a, Capsule<3, T> const&
// b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Plane<T> const& a, Frustum<T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Plane<T> const&          a,
//                                                LineSegment<3, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Plane<T> const& a, OBB<3, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Plane<T> const& a, Plane<T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Plane<T> const& a, Ray<3, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Plane<T> const& a, Triangle<3, T> const&
// b)
// {
// 	// TODO: Implement
// }

template <class T>
[[nodiscard]] constexpr Vec<3, T> closestPoint(Plane<T> const& a, Vec<3, T> const& b)
{
	auto distance = dot(a.normal, b) - a.distance;
	return b - a.normal * distance;
}

/**************************************************************************************
|                                                                                     |
|                                         Ray                                         |
|                                                                                     |
**************************************************************************************/

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Ray<Dim, T> const&  a,
//                                                  AABB<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Ray<Dim, T> const& a,
//                                                  Sphere<Dim, T> const&  b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Ray<Dim, T> const&     a,
//                                                  Capsule<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Ray<3, T> const& a, Frustum<T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Ray<Dim, T> const&         a,
//                                                  LineSegment<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Ray<Dim, T> const& a,
//                                                  OBB<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Ray<3, T> const& a, Plane<T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Ray<Dim, T> const& a,
//                                                  Ray<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Ray<Dim, T> const&      a,
//                                                  Triangle<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

template <std::size_t Dim, class T>
[[nodiscard]] constexpr Vec<Dim, T> closestPoint(Ray<Dim, T> const& a,
                                                 Vec<Dim, T> const& b)
{
	auto t = dot(b - a.origin, a.direction);
	return a.origin + a.direction * std::max(t, T(0));
}

/**************************************************************************************
|                                                                                     |
|                                      Triangle                                       |
|                                                                                     |
**************************************************************************************/

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Triangle<Dim, T> const& a,
//                                                  AABB<Dim, T> const&     b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Triangle<Dim, T> const& a,
//                                                  Sphere<Dim, T> const&       b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Triangle<Dim, T> const& a,
//                                                  Capsule<Dim, T> const&  b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Triangle<3, T> const& a,
//                                                Frustum<T> const&     b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Triangle<Dim, T> const&    a,
//                                                  LineSegment<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Triangle<Dim, T> const& a,
//                                                  OBB<Dim, T> const&      b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Triangle<3, T> const& a, Plane<T> const&
// b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Triangle<Dim, T> const& a,
//                                                  Ray<Dim, T> const&      b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Triangle<Dim, T> const& a,
//                                                  Triangle<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Triangle<Dim, T> const& a,
//                                                  Vec<Dim, T> const&      b)
// {
// 	// TODO: Implement
// }

/**************************************************************************************
|                                                                                     |
|                                         Vec                                         |
|                                                                                     |
**************************************************************************************/

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Vec<Dim, T> const&  a,
//                                                  AABB<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Vec<Dim, T> const& a,
//                                                  Sphere<Dim, T> const&  b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Vec<Dim, T> const&     a,
//                                                  Capsule<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Vec<3, T> const& a, Frustum<T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Vec<Dim, T> const&         a,
//                                                  LineSegment<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Vec<Dim, T> const& a,
//                                                  OBB<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <class T>
// [[nodiscard]] constexpr Vec<3, T> closestPoint(Vec<3, T> const& a, Plane<T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Vec<Dim, T> const& a,
//                                                  Ray<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Vec<Dim, T> const&      a,
//                                                  Triangle<Dim, T> const& b)
// {
// 	// TODO: Implement
// }

// template <std::size_t Dim, class T>
// [[nodiscard]] constexpr Vec<Dim, T> closestPoint(Vec<Dim, T> const& a,
//                                                  Vec<Dim, T> const& b)
// {
// 	// TODO: Implement
// }
}  // namespace ufo

#endif  // UFO_GEOMETRY_CLOSEST_POINT_HPP