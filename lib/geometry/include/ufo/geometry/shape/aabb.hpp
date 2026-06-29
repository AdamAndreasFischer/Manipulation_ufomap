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

#ifndef UFO_GEOMETRY_SHAPE_AXIS_ALIGNED_BOUNDING_BOX_HPP
#define UFO_GEOMETRY_SHAPE_AXIS_ALIGNED_BOUNDING_BOX_HPP

// UFO
#include <ufo/math/vec.hpp>

// STL
#include <cstddef>
#include <ostream>
#include <type_traits>

namespace ufo
{
/*!
 * @brief Something something
 * @author Daniel Duberg
 *
 */
template <std::size_t Dim = 3, class T = float>
struct AABB {
	static_assert(std::is_floating_point_v<T>, "T is required to be floating point.");

	using value_type = T;

	Vec<Dim, T> min;
	Vec<Dim, T> max;

	constexpr AABB() noexcept            = default;
	constexpr AABB(AABB const&) noexcept = default;

	constexpr AABB(Vec<Dim, T> const& min, Vec<Dim, T> const& max) noexcept
	    : min(min), max(max)
	{
	}

	constexpr AABB(Vec<Dim, T> const& center, T half_length) noexcept
	    : min(center - half_length), max(center + half_length)
	{
	}

	template <class U>
	constexpr explicit AABB(AABB<Dim, U> const& other) noexcept
	    : min(other.min), max(other.max)
	{
	}

	[[nodiscard]] constexpr Vec<Dim, T> center() const noexcept
	{
		return min + (max - min) * static_cast<T>(0.5);
	}

	[[nodiscard]] constexpr Vec<Dim, T> length() const noexcept { return max - min; }

	[[nodiscard]] constexpr Vec<Dim, T> halfLength() const noexcept
	{
		return (max - min) * static_cast<T>(0.5);
	}
};

using AABB1 = AABB<1, float>;
using AABB2 = AABB<2, float>;
using AABB3 = AABB<3, float>;
using AABB4 = AABB<4, float>;

using AABB1f = AABB<1, float>;
using AABB2f = AABB<2, float>;
using AABB3f = AABB<3, float>;
using AABB4f = AABB<4, float>;

using AABB1d = AABB<1, double>;
using AABB2d = AABB<2, double>;
using AABB3d = AABB<3, double>;
using AABB4d = AABB<4, double>;

/*!
 * @brief Compare two AABBs.
 *
 * @param lhs,rhs The AABBs to compare
 * @return `true` if they compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator==(AABB<Dim, T> const& lhs, AABB<Dim, T> const& rhs)
{
	return lhs.min == rhs.min && lhs.min == rhs.min;
}

/*!
 * @brief Compare two AABBs.
 *
 * @param lhs,rhs The AABBs to compare
 * @return `true` if they do not compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator!=(AABB<Dim, T> const& lhs, AABB<Dim, T> const& rhs)
{
	return !(lhs == rhs);
}

template <std::size_t Dim, class T>
std::ostream& operator<<(std::ostream& out, AABB<Dim, T> const& aabb)
{
	return out << "Min: " << aabb.min << ", Max: " << aabb.max;
}
}  // namespace ufo

#endif  // UFO_GEOMETRY_SHAPE_AXIS_ALIGNED_BOUNDING_BOX_HPP