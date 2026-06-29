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

#ifndef UFO_GEOMETRY_SHAPE_ORIENTED_BOUNDING_BOX_HPP
#define UFO_GEOMETRY_SHAPE_ORIENTED_BOUNDING_BOX_HPP

// UFO
#include <ufo/math/mat.hpp>
#include <ufo/math/quat.hpp>
#include <ufo/math/vec.hpp>

// STL
#include <cstddef>

namespace ufo
{
template <std::size_t Dim = 3, class T = float>
struct OBB {
	using value_type = T;

	Vec<Dim, T>      center;
	Vec<Dim, T>      half_length;
	Mat<Dim, Dim, T> rotation;

	constexpr OBB() noexcept           = default;
	constexpr OBB(OBB const&) noexcept = default;

	constexpr OBB(Vec<Dim, T> center, Vec<Dim, T> half_length) noexcept
	    : center(center), half_length(half_length)
	{
	}

	constexpr OBB(Vec<Dim, T> center, Vec<Dim, T> half_length,
	              Mat<Dim, Dim, T> rotation) noexcept
	    : center(center), half_length(half_length), rotation(rotation)
	{
	}

	constexpr OBB(Vec<Dim, T> center, Vec<Dim, T> half_length,
	              Quat<T> const& rotation) noexcept
	    : center(center), half_length(half_length), rotation(rotation)
	{
	}

	template <class U>
	constexpr explicit OBB(OBB<Dim, U> const& other) noexcept
	    : center(other.center), half_length(other.half_length), rotation(other.rotation)
	{
	}

	[[nodiscard]] constexpr Vec<Dim, T> rotatedHalfLength() const
	{
		return rotation * half_length;
	}
};

using OBB2 = OBB<2, float>;
using OBB3 = OBB<3, float>;
using OBB4 = OBB<4, float>;

using OBB2d = OBB<2, double>;
using OBB3d = OBB<3, double>;
using OBB4d = OBB<4, double>;

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
}  // namespace ufo

#endif  // UFO_GEOMETRY_SHAPE_ORIENTED_BOUNDING_BOX_HPP