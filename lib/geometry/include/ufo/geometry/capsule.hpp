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

#ifndef UFO_GEOMETRY_CAPSULE_HPP
#define UFO_GEOMETRY_CAPSULE_HPP

// UFO
#include <ufo/math/vec.hpp>

// STL
#include <cstddef>
#include <ostream>
#include <type_traits>

namespace ufo
{
template <std::size_t Dim = 3, class T = float>
struct Capsule {
	static_assert(std::is_floating_point_v<T>, "T is required to be floating point.");

	using value_type = T;

	Vec<Dim, T> start;
	Vec<Dim, T> end;
	T           radius{};

	constexpr Capsule() noexcept = default;

	constexpr Capsule(Vec<Dim, T> const& start, Vec<Dim, T> const& end, T radius) noexcept
	    : start(start), end(end), radius(radius)
	{
	}
	constexpr Capsule(Capsule const&) noexcept = default;

	template <class U>
	constexpr explicit Capsule(Capsule<Dim, U> const& other) noexcept
	    : start(other.start), end(other.end), radius(static_cast<T>(other.radius))
	{
	}
};

//
// Deduction guide
//

template <std::size_t Dim, class T>
Capsule(Vec<Dim, T>, Vec<Dim, T>, T) -> Capsule<Dim, T>;

/*!
 * @brief Compare two Capsules.
 *
 * @param lhs,rhs The Capsules to compare
 * @return `true` if they compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator==(Capsule<Dim, T> const& lhs, Capsule<Dim, T> const& rhs)
{
	return lhs.start == rhs.start && lhs.end == rhs.end && lhs.radius == rhs.radius;
}

/*!
 * @brief Compare two Capsules.
 *
 * @param lhs,rhs The Capsules to compare
 * @return `true` if they do not compare equal, `false` otherwise.
 */
template <std::size_t Dim, class T>
bool operator!=(Capsule<Dim, T> const& lhs, Capsule<Dim, T> const& rhs)
{
	return !(lhs == rhs);
}

template <std::size_t Dim, class T>
std::ostream& operator<<(std::ostream& out, Capsule<Dim, T> const& capsule)
{
	return out << "Start: " << capsule.start << ", End: " << capsule.end
	           << ", Radius: " << capsule.radius;
}

template <class T>
using Capsule2 = Capsule<2, T>;
template <class T>
using Capsule3 = Capsule<3, T>;
template <class T>
using Capsule4 = Capsule<4, T>;

using Capsule2f = Capsule<2, float>;
using Capsule3f = Capsule<3, float>;
using Capsule4f = Capsule<4, float>;

using Capsule2d = Capsule<2, double>;
using Capsule3d = Capsule<3, double>;
using Capsule4d = Capsule<4, double>;
}  // namespace ufo

#endif  // UFO_GEOMETRY_CAPSULE_HPP
