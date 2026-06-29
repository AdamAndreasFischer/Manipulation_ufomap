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

#ifndef UFO_GEOMETRY_HELPER_HPP
#define UFO_GEOMETRY_HELPER_HPP

// UFO
#include <ufo/geometry/aabb.hpp>
#include <ufo/geometry/fun.hpp>
#include <ufo/geometry/line_segment.hpp>
#include <ufo/geometry/obb.hpp>
#include <ufo/geometry/plane.hpp>
#include <ufo/geometry/ray.hpp>
#include <ufo/geometry/sphere.hpp>

// STL
#include <algorithm>
#include <cmath>

namespace ufo::detail
{
//
// Intersects line
//

template <std::size_t Dim, class T>
[[nodiscard]] constexpr bool intersectsLine(AABB<Dim, T> const& aabb,
                                            Ray<Dim, T> const& ray, T t_near,
                                            T t_far) noexcept
{
	auto min = ufo::min(aabb);
	auto max = ufo::max(aabb);

	for (std::size_t i{}; Dim > i; ++i) {
		if (T(0) != ray.direction[i]) {
			T reciprocal_direction = T(1) / ray.direction[i];
			T t1                   = (min[i] - ray.origin[i]) * reciprocal_direction;
			T t2                   = (max[i] - ray.origin[i]) * reciprocal_direction;

			if (t1 < t2) {
				t_near = std::max(t1, t_near);
				t_far  = std::min(t2, t_far);
			} else {
				t_near = std::max(t2, t_near);
				t_far  = std::min(t1, t_far);
			}

			if (t_near > t_far) {
				return false;
			}
		} else if (min[i] > ray.origin[i] || max[i] < ray.origin[i]) {
			return false;
		}
	}
	return true;
}

// constexpr bool intersectsLine(AABC aabc, Ray const& ray, float t_near,
//                               float t_far) noexcept
// {
// 	Point min = aabc.min();
// 	Point max = aabc.max();

// 	for (std::size_t i{}; 3 > i; ++i) {
// 		if (0 != ray.direction[i]) {
// 			float reciprocal_direction = 1.0f / ray.direction[i];
// 			float t1                   = (min[i] - ray.origin[i]) * reciprocal_direction;
// 			float t2                   = (max[i] - ray.origin[i]) * reciprocal_direction;

// 			if (t1 < t2) {
// 				t_near = std::max(t1, t_near);
// 				t_far  = std::min(t2, t_far);
// 			} else {
// 				t_near = std::max(t2, t_near);
// 				t_far  = std::min(t1, t_far);
// 			}

// 			if (t_near > t_far) {
// 				return false;
// 			}
// 		} else if (min[i] > ray.origin[i] || max[i] < ray.origin[i]) {
// 			return false;
// 		}
// 	}
// 	return true;
// }

//
// Classify
//

template <class T>
[[nodiscard]] constexpr T classify(AABB<3, T> const& aabb, Plane<T> const& plane) noexcept
{
	auto hl = aabb.halfLength();
	auto c  = aabb.center();

	T r = std::abs(hl.x * plane.normal.x) + std::abs(hl.y * plane.normal.y) +
	      std::abs(hl.z * plane.normal.z);
	T d = ufo::dot(plane.normal, c) + plane.distance;
	if (std::abs(d) < r) {
		return T(0);
	} else if (T(0) > d) {
		return d + r;
	}
	return d - r;
}

// constexpr float classify(AABC aabc, Plane const& plane) noexcept
// {
// 	float r = std::abs(aabc.half_size * plane.normal.x) +
// 	          std::abs(aabc.half_size * plane.normal.y) +
// 	          std::abs(aabc.half_size * plane.normal.z);
// 	float d = Point::dot(plane.normal, aabc.center) + plane.distance;
// 	if (std::abs(d) < r) {
// 		return 0;
// 	} else if (d < 0) {
// 		return d + r;
// 	}
// 	return d - r;
// }

// constexpr float classify(OBB const& obb, Plane const& plane) noexcept
// {
// 	// FIXME: Check if correct
// 	Point normal = plane.normal * obb.rotation;
// 	float r = std::abs(obb.half_size.x() * normal.x()) +
// 	          std::abs(obb.half_size.y() * normal.y()) +
// 	          std::abs(obb.half_size.z() * normal.z());
// 	float d = Point::dot(plane.normal, obb.center) + plane.distance;
// 	if (std::abs(d) < r) {
// 		return 0;
// 	} else if (d < 0) {
// 		return d + r;
// 	}
// 	return d - r;
// }

//
// Get interval
//

// constexpr std::pair<float, float> getInterval(AABB const& aabb, Point axis) noexcept
// {
// 	Point i = aabb.min();
// 	Point a = aabb.max();

// 	Point vertex[8] = {Point(i.x, a.y, a.z), Point(i.x, a.y, i.z), Point(i.x, i.y, a.z),
// 	                   Point(i.x, i.y, i.z), Point(a.x, a.y, a.z), Point(a.x, a.y, i.z),
// 	                   Point(a.x, i.y, a.z), Point(a.x, i.y, i.z)};

// 	std::pair<float, float> result;
// 	result.first = result.second = Point::dot(axis, vertex[0]);

// 	for (int i = 1; i < 8; ++i) {
// 		float projection = Point::dot(axis, vertex[i]);
// 		result.first     = std::min(result.first, projection);
// 		result.second    = std::max(result.second, projection);
// 	}

// 	return result;
// }

// constexpr std::pair<float, float> getInterval(AABC aabc, Point axis) noexcept
// {
// 	Point i = aabc.min();
// 	Point a = aabc.max();

// 	Point vertex[8] = {Point(i.x, a.y, a.z), Point(i.x, a.y, i.z), Point(i.x, i.y, a.z),
// 	                   Point(i.x, i.y, i.z), Point(a.x, a.y, a.z), Point(a.x, a.y, i.z),
// 	                   Point(a.x, i.y, a.z), Point(a.x, i.y, i.z)};

// 	std::pair<float, float> result;
// 	result.first = result.second = Point::dot(axis, vertex[0]);

// 	for (int i = 1; i < 8; ++i) {
// 		float projection = Point::dot(axis, vertex[i]);
// 		result.first     = std::min(result.first, projection);
// 		result.second    = std::max(result.second, projection);
// 	}

// 	return result;
// }

// constexpr std::pair<float, float> getInterval(OBB const& obb, Point axis) noexcept
// {
// 	Point vertex[8];

// 	Point C = obb.center;     // OBB Center
// 	Point E = obb.half_size;  // OBB Extents

// 	std::array<float, 9> obb_rot_matrix = obb.rotation.rotMatrix();

// 	Point A[] = {
// 	    // OBB Axis
// 	    Point(obb_rot_matrix[0], obb_rot_matrix[1], obb_rot_matrix[2]),
// 	    Point(obb_rot_matrix[3], obb_rot_matrix[4], obb_rot_matrix[5]),
// 	    Point(obb_rot_matrix[6], obb_rot_matrix[7], obb_rot_matrix[8]),
// 	};

// 	vertex[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
// 	vertex[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
// 	vertex[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
// 	vertex[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
// 	vertex[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
// 	vertex[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
// 	vertex[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
// 	vertex[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

// 	std::pair<float, float> result;
// 	result.first = result.second = Point::dot(axis, vertex[0]);

// 	for (int i = 1; i < 8; ++i) {
// 		float projection = Point::dot(axis, vertex[i]);
// 		result.first     = std::min(result.first, projection);
// 		result.second    = std::max(result.second, projection);
// 	}

// 	return result;
// }

//
// Overlap on axis
//

// constexpr bool overlapOnAxis(AABB const& aabb, OBB const& obb, Point axis) noexcept
// {
// 	auto [a_min, a_max] = getInterval(aabb, axis);
// 	auto [b_min, b_max] = getInterval(obb, axis);
// 	return ((b_min <= a_max) && (a_min <= b_max));
// }

// constexpr bool overlapOnAxis(AABC aabc, OBB const& obb, Point axis) noexcept
// {
// 	auto [a_min, a_max] = getInterval(aabc, axis);
// 	auto [b_min, b_max] = getInterval(obb, axis);
// 	return ((b_min <= a_max) && (a_min <= b_max));
// }

// constexpr bool overlapOnAxis(OBB const& obb_1, OBB const& obb_2, Point axis) noexcept
// {
// 	auto [a_min, a_max] = getInterval(obb_1, axis);
// 	auto [b_min, b_max] = getInterval(obb_2, axis);
// 	return ((b_min <= a_max) && (a_min <= b_max));
// }

// SOMETHING BELOW

// Returns the closest point from an infinite 3D line to 3D box face, plus the closest "t"
// along o+t*d Based on
// https://www.geometrictools.com/Documentation/DistanceLine3Rectangle3.pdf The box is
// axis aligned, centered at the origin, and d is expected to point towards the first
// octant (reflected s.t. all d components are positive).
//   i - the indirection indices for the face
//   o - the origin of the segment
//   d - the direction along the segment (which does not need to be normalized)
//   b - the box radius (3 half side lengths)
inline Vec4f lineFaceQuery(Vec3i i, Vec3f o, Vec3f d, Vec3f b)
{
	Vec3f PmE = o - b;
	Vec3f PpE = o + b;

	Vec3f bi   = Vec3f(b[i[0]], b[i[1]], b[i[2]]);
	Vec3f oi   = Vec3f(o[i[0]], o[i[1]], o[i[2]]);
	Vec3f di   = Vec3f(d[i[0]], d[i[1]], d[i[2]]);
	Vec3f PmEi = Vec3f(PmE[i[0]], PmE[i[1]], PmE[i[2]]);
	Vec3f PpEi = Vec3f(PpE[i[0]], PpE[i[1]], PpE[i[2]]);

	Vec4f c;
	if (di[0] * PpEi[1] >= di[1] * PmEi[0]) {
		if (di[0] * PpEi[2] >= di[2] * PmEi[0]) {
			// v[i1] >= -e[i1], v[i2] >= -e[i2] (distance = 0)
			c = Vec4f(bi[0], oi[1] - di[1] * PmEi[0] / di[0], oi[2] - di[2] * PmEi[0] / di[0],
			          -PmEi[0] / di[0]);
		} else {
			// v[i1] >= -e[i1], v[i2] < -e[i2]
			float lenSqr = di[0] * di[0] + di[2] * di[2];
			float tmp    = lenSqr * PpEi[1] - di[1] * (di[0] * PmEi[0] + di[2] * PpEi[2]);
			if (tmp <= 2. * lenSqr * bi[1]) {
				float t = tmp / lenSqr;
				lenSqr += di[1] * di[1];
				tmp         = PpEi[1] - t;
				float delta = di[0] * PmEi[0] + di[1] * tmp + di[2] * PpEi[2];
				c           = Vec4f(bi[0], t - bi[1], -bi[2], -delta / lenSqr);
			} else {
				lenSqr += di[1] * di[1];
				float delta = di[0] * PmEi[0] + di[1] * PmEi[1] + di[2] * PpEi[2];
				c           = Vec4f(bi[0], bi[1], -bi[2], -delta / lenSqr);
			}
		}
	} else {
		if (di[0] * PpEi[2] >= di[2] * PmEi[0]) {
			// v[i1] < -e[i1], v[i2] >= -e[i2]
			float lenSqr = di[0] * di[0] + di[1] * di[1];
			float tmp    = lenSqr * PpEi[2] - di[2] * (di[0] * PmEi[0] + di[1] * PpEi[1]);
			if (tmp <= 2. * lenSqr * bi[2]) {
				float t = tmp / lenSqr;
				lenSqr += di[2] * di[2];
				tmp         = PpEi[2] - t;
				float delta = di[0] * PmEi[0] + di[1] * PpEi[1] + di[2] * tmp;
				c           = Vec4f(bi[0], -bi[1], t - bi[2], -delta / lenSqr);
			} else {
				lenSqr += di[2] * di[2];
				float delta = di[0] * PmEi[0] + di[1] * PpEi[1] + di[2] * PmEi[2];
				c           = Vec4f(bi[0], -bi[1], bi[2], -delta / lenSqr);
			}
		} else {
			// v[i1] < -e[i1], v[i2] < -e[i2]
			float lenSqr = di[0] * di[0] + di[2] * di[2];
			float tmp    = lenSqr * PpEi[1] - di[1] * (di[0] * PmEi[0] + di[2] * PpEi[2]);
			if (tmp >= 0.) {
				// v[i1]-edge is c
				if (tmp <= 2. * lenSqr * bi[1]) {
					float t = tmp / lenSqr;
					lenSqr += di[1] * di[1];
					tmp         = PpEi[1] - t;
					float delta = di[0] * PmEi[0] + di[1] * tmp + di[2] * PpEi[2];
					c           = Vec4f(bi[0], t - bi[1], -bi[2], -delta / lenSqr);
				} else {
					lenSqr += di[1] * di[1];
					float delta = di[0] * PmEi[0] + di[1] * PmEi[1] + di[2] * PpEi[2];
					c           = Vec4f(bi[0], bi[1], -bi[2], -delta / lenSqr);
				}
			} else {
				lenSqr = di[0] * di[0] + di[1] * di[1];
				tmp    = lenSqr * PpEi[2] - di[2] * (di[0] * PmEi[0] + di[1] * PpEi[1]);
				if (tmp >= 0.) {
					// v[i2]-edge is c
					if (tmp <= 2. * lenSqr * bi[2]) {
						float t = tmp / lenSqr;
						lenSqr += di[2] * di[2];
						tmp         = PpEi[2] - t;
						float delta = di[0] * PmEi[0] + di[1] * PpEi[1] + di[2] * tmp;
						c           = Vec4f(bi[0], -bi[1], t - bi[2], -delta / lenSqr);
					} else {
						lenSqr += di[2] * di[2];
						float delta = di[0] * PmEi[0] + di[1] * PpEi[1] + di[2] * PmEi[2];
						c           = Vec4f(bi[0], -bi[1], bi[2], -delta / lenSqr);
					}
				} else {
					// (v[i1],v[i2])-corner is c
					lenSqr += di[2] * di[2];
					float delta = di[0] * PmEi[0] + di[1] * PpEi[1] + di[2] * PpEi[2];
					c           = Vec4f(bi[0], -bi[1], -bi[2], -delta / lenSqr);
				}
			}
		}
	}

	Vec3i map;
	map[i[0]] = 0;
	map[i[1]] = 1;
	map[i[2]] = 2;
	return Vec4f(c[map[0]], c[map[1]], c[map[2]], c.w);
}

// Returns the closest point from an infinite 3D line to a 3D box,  plus the closest "t"
// along o+t*d
//   o - the origin of the segment
//   d - the direction along the segment (which does not need to be normalized)
//   b - the box radius (3 half side lengths)
inline Vec4f lineBoxQuery(Vec3f o, Vec3f d, Vec3f b)
{
	Vec4f closest;

	// Transform the line direction to the first octant using reflections.
	Vec<3, bool> reflected = lessThan(d, Vec3f(0.0f));
	o                      = mix(o, -o, reflected);
	d                      = mix(d, -d, reflected);

	// point minus extent
	Vec3f PmE = o - b;

	// face indices
	Vec3i i;

	// line intersects planes x or z
	if (d[1] * PmE[0] >= d[0] * PmE[1])
		// line intersects x = e0 if true, z = e2 if false
		i = (d[2] * PmE[0] >= d[0] * PmE[2]) ? Vec3i(0, 1, 2) : Vec3i(2, 0, 1);
	// line intersects planes y or z
	else
		// line intersects y = e1 if true, z = e2 if false
		i = (d[2] * PmE[1] >= d[1] * PmE[2]) ? Vec3i(1, 2, 0) : Vec3i(2, 0, 1);

	// Query closest point on face
	closest = lineFaceQuery(i, o, d, b);

	// Account for previously applied reflections.
	auto c = mix(static_cast<Vec3f>(closest), static_cast<Vec3f>(-closest), reflected);
	return Vec4f(c.x, c.y, c.z, closest.w);
}

// Returns the closest point from a 3D point to a 3D box
// The box is axis aligned, and centered at the origin
//   o - the query origin
//   b - the box radius (3 half side lengths)
inline Vec3f pointBoxQuery(Vec3f o, Vec3f b)
{
	Vec3f closest = o;
	for (int i = 0; i < 3; ++i) {
		if (o[i] < -b[i])
			closest[i] = -b[i];
		else if (o[i] > b[i])
			closest[i] = b[i];
	}
	return closest;
}

// Returns the closest point from a finite 3D segment to a 3D box, plus the closest "t"
// along o+t*d
//   s - the start of the segment
//   e - the end of the segment
//   b - the box radius (3 half side lengths)
inline Vec4f segmentBoxQuery(Vec3f s, Vec3f e, Vec3f b)
{
	Vec3f o        = s;
	Vec3f d        = e - s;
	Vec4f lbOutput = lineBoxQuery(o, d, b);
	// If closest is within the segment, return that result directly
	if (lbOutput.w >= 0. && lbOutput.w <= 1.) return lbOutput;

	// Otherwise, compute the closest point to either side of the segment
	float parameter = (lbOutput.w < 0.) ? 0. : 1.;
	Vec3f pbOutput  = pointBoxQuery(o + d * parameter, b);
	return Vec4f(pbOutput, parameter);
}

}  // namespace ufo::detail

#endif  // UFO_GEOMETRY_HELPER_HPP