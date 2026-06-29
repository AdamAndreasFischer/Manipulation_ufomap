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

#ifndef UFO_GEOMETRY_SHAPE_LINE_SEGMENT_HPP
#define UFO_GEOMETRY_SHAPE_LINE_SEGMENT_HPP

// UFO
#include <ufo/math/vec.hpp>

// STL
#include <cstddef>
#include <ostream>

namespace ufo
{
template <std::size_t Dim = 3, class T = float>
struct LineSegment {
	using value_type = T;

	Vec<Dim, T> start;
	Vec<Dim, T> end;

	constexpr LineSegment() noexcept                   = default;
	constexpr LineSegment(LineSegment const&) noexcept = default;

	constexpr LineSegment(Vec<Dim, T> start, Vec<Dim, T> end) noexcept
	    : start(start), end(end)
	{
	}

	template <class U>
	constexpr explicit LineSegment(LineSegment<Dim, U> const& other) noexcept
	    : start(other.start), end(other.end)
	{
	}
};

using LineSegment1 = LineSegment<1, float>;
using LineSegment2 = LineSegment<2, float>;
using LineSegment3 = LineSegment<3, float>;
using LineSegment4 = LineSegment<4, float>;

using LineSegment1d = LineSegment<1, double>;
using LineSegment2d = LineSegment<2, double>;
using LineSegment3d = LineSegment<3, double>;
using LineSegment4d = LineSegment<4, double>;

/*!
 * @brief Compare two LineSegments.
 *
 * @param lhs,rhs The LineSegments to compare
 * @return `true` if they compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator==(LineSegment<Dim, T> const& lhs, LineSegment<Dim, T> const& rhs)
{
	return lhs.start == rhs.start && lhs.end == rhs.end;
}

/*!
 * @brief Compare two LineSegments.
 *
 * @param lhs,rhs The LineSegments to compare
 * @return `true` if they do not compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator!=(LineSegment<Dim, T> const& lhs, LineSegment<Dim, T> const& rhs)
{
	return !(lhs == rhs);
}

template <std::size_t Dim, class T>
std::ostream& operator<<(std::ostream& out, LineSegment<Dim, T> const& ls)
{
	return out << "Start: " << ls.start << ", End: " << ls.end;
}
}  // namespace ufo

#endif  // UFO_GEOMETRY_SHAPE_LINE_SEGMENT_HPP