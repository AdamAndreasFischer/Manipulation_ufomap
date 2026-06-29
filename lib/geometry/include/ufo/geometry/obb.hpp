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

#ifndef UFO_GEOMETRY_OBB_HPP
#define UFO_GEOMETRY_OBB_HPP

// UFO
#include <ufo/math/mat.hpp>
#include <ufo/math/quat.hpp>
#include <ufo/math/vec.hpp>

// STL
#include <cmath>
#include <cstddef>
#include <ostream>
#include <type_traits>

namespace ufo
{
template <std::size_t Dim = 3, class T = float>
struct OBB;

template <class T>
struct OBB<2, T> {
	static_assert(std::is_floating_point_v<T>, "T is required to be floating point.");

	using value_type = T;

	Vec<2, T>    center;
	Vec<2, T>    half_length;
	Mat<2, 2, T> rotation;

	constexpr OBB() noexcept = default;

	constexpr OBB(Vec<2, T> const& start, Vec<2, T> const& end,
	              Vec<1, T> const& half_length)
	    : center((start + end) * T(0.5))
	{
		auto dir = end - start;

		this->half_length = Vec<2, T>(norm(dir), half_length);

		auto theta     = std::atan2(dir.y, dir.x);
		auto cos_theta = std::cos(theta);
		auto sin_theta = std::sin(theta);

		rotation[0][0] = cos_theta;
		rotation[0][1] = sin_theta;
		rotation[1][0] = -sin_theta;
		rotation[1][1] = cos_theta;
	}

	constexpr OBB(Vec<2, T> const& start, Vec<2, T> const& end, T const& half_length)
	    : OBB(start, end, Vec<1, T>(half_length))
	{
	}

	constexpr OBB(Vec<2, T> const& center, Vec<2, T> const& half_length) noexcept
	    : center(center), half_length(half_length)
	{
	}

	constexpr OBB(Vec<2, T> const& center, Vec<2, T> const& half_length,
	              Mat<2, 2, T> const& rotation) noexcept
	    : center(center), half_length(half_length), rotation(rotation)
	{
	}

	constexpr OBB(OBB const&) noexcept = default;

	template <class U>
	constexpr explicit OBB(OBB<2, U> const& other) noexcept
	    : center(other.center), half_length(other.half_length), rotation(other.rotation)
	{
	}

	[[nodiscard]] constexpr Vec<2, T> rotatedHalfLength() const
	{
		return half_length * rotation;
	}

	void setRotation(T angle)
	{
		auto cos_theta = std::cos(angle);
		auto sin_theta = std::sin(angle);

		rotation[0][0] = cos_theta;
		rotation[0][1] = sin_theta;
		rotation[1][0] = -sin_theta;
		rotation[1][1] = cos_theta;
	}
};

template <class T>
struct OBB<3, T> {
	static_assert(std::is_floating_point_v<T>, "T is required to be floating point.");

	using value_type = T;

	Vec<3, T>    center;
	Vec<3, T>    half_length;
	Mat<3, 3, T> rotation;

	constexpr OBB() noexcept = default;

	constexpr OBB(Vec<3, T> const& start, Vec<3, T> const& end,
	              Vec<2, T> const& half_length,
	              Vec<3, T> const& up = Vec<3, T>(T(0), T(0), T(1)))
	    : center((start + end) * T(0.5))
	{
		auto dir = end - start;

		this->half_length = Vec<3, T>(norm(dir), half_length);

		// Similar to right handed lookAt

		Vec<3, T> const f(normalize(dir));
		Vec<3, T> const s(normalize(cross(f, up)));
		Vec<3, T> const u(cross(s, f));

		rotation[0][0] = s.x;
		rotation[1][0] = s.y;
		rotation[2][0] = s.z;
		rotation[0][1] = u.x;
		rotation[1][1] = u.y;
		rotation[2][1] = u.z;
		rotation[0][2] = -f.x;
		rotation[1][2] = -f.y;
		rotation[2][2] = -f.z;
	}

	constexpr OBB(Vec<3, T> const& center, Vec<3, T> const& half_length) noexcept
	    : center(center), half_length(half_length)
	{
	}

	constexpr OBB(Vec<3, T> const& center, Vec<3, T> const& half_length,
	              Mat<3, 3, T> const& rotation) noexcept
	    : center(center), half_length(half_length), rotation(rotation)
	{
	}

	constexpr OBB(Vec<3, T> const& center, Vec<3, T> const& half_length,
	              Quat<T> const& rotation) noexcept
	    : center(center), half_length(half_length), rotation(rotation)
	{
	}

	constexpr OBB(OBB const&) noexcept = default;

	template <class U>
	constexpr explicit OBB(OBB<3, U> const& other) noexcept
	    : center(other.center), half_length(other.half_length), rotation(other.rotation)
	{
	}

	[[nodiscard]] constexpr Vec<3, T> rotatedHalfLength() const
	{
		return half_length * rotation;
	}

	void setRotation(Quat<T> const& rotation) { this->rotation = rotation; }
};

template <class T>
struct OBB<4, T> {
	static_assert(std::is_floating_point_v<T>, "T is required to be floating point.");

	using value_type = T;

	Vec<4, T>    center;
	Vec<4, T>    half_length;
	Mat<4, 4, T> rotation;

	constexpr OBB() noexcept = default;

	constexpr OBB(Vec<4, T> const& start, Vec<4, T> const& end,
	              Vec<3, T> const& half_length)
	{
		auto dir = end - start;

		center      = dir * T(0.5);
		half_length = Vec<4, T>(norm(dir), half_length);

		// TODO: Implement
	}

	constexpr OBB(Vec<4, T> const& center, Vec<4, T> const& half_length) noexcept
	    : center(center), half_length(half_length)
	{
	}

	constexpr OBB(Vec<4, T> const& center, Vec<4, T> const& half_length,
	              Mat<4, 4, T> const& rotation) noexcept
	    : center(center), half_length(half_length), rotation(rotation)
	{
	}

	constexpr OBB(OBB const&) noexcept = default;

	template <class U>
	constexpr explicit OBB(OBB<4, U> const& other) noexcept
	    : center(other.center), half_length(other.half_length), rotation(other.rotation)
	{
	}

	[[nodiscard]] constexpr Vec<4, T> rotatedHalfLength() const
	{
		return half_length * rotation;
	}
};

//
// Deduction guide
//

// template <std::size_t Dim, class T>
// OBB(Vec<Dim, T>, T) -> OBB<Dim, T>;

/*!
 * @brief Compare two OBBs.
 *
 * @param lhs,rhs The OBBs to compare
 * @return `true` if they compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator==(OBB<Dim, T> const& lhs, OBB<Dim, T> const& rhs)
{
	return lhs.center == rhs.center && lhs.half_length == rhs.half_length &&
	       lhs.rotation == rhs.rotation;
}

/*!
 * @brief Compare two OBBs.
 *
 * @param lhs,rhs The OBBs to compare
 * @return `true` if they do not compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator!=(OBB<Dim, T> const& lhs, OBB<Dim, T> const& rhs)
{
	return !(lhs == rhs);
}

template <std::size_t Dim, class T>
std::ostream& operator<<(std::ostream& out, OBB<Dim, T> const& obb)
{
	return out << "Center: " << obb.center << ", Half length: " << obb.half_length
	           << ", Rotation: " << obb.rotation;
}

template <class T>
using OBB2 = OBB<2, T>;
template <class T>
using OBB3 = OBB<3, T>;
template <class T>
using OBB4 = OBB<4, T>;

using OBB2f = OBB<2, float>;
using OBB3f = OBB<3, float>;
using OBB4f = OBB<4, float>;

using OBB2d = OBB<2, double>;
using OBB3d = OBB<3, double>;
using OBB4d = OBB<4, double>;
}  // namespace ufo

#endif  // UFO_GEOMETRY_OBB_HPP