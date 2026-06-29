/*!
 * UFOMap: An Efficient Probabilistic 3D Mapping Framework That Embraces the
 * Unknown
 *
 * @author Daniel Duberg (dduberg@kth.se)
 * @see https://github.com/UnknownFreeOccupied/ufomap
 * @version 1.0
 * @date 2022-05-13
 *
 * @copyright Copyright (c) 2022, Daniel Duberg, KTH Royal Institute of
 * Technology
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Daniel Duberg, KTH Royal Institute of Technology
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
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
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UFO_VISION_DETAIL_COLOR_HPP
#define UFO_VISION_DETAIL_COLOR_HPP

// UFO
#include <ufo/utility/type_traits.hpp>
#include <ufo/vision/color_type.hpp>

// STL
#include <cmath>
#include <limits>
#include <type_traits>

namespace ufo
{
template <ColorType CT = ColorType::RGBA8U>
struct Color;

namespace detail
{
template <class To, class From>
[[nodiscard]] constexpr To convertRGBComponent(From x)
{
	// TODO: Make it more generic. For now we only have `float` and `uint8`.

	if constexpr (std::is_same_v<From, To>) {
		return x;
	} else if constexpr (std::is_unsigned_v<From> && std::is_floating_point_v<To>) {
		return static_cast<To>(x) / static_cast<To>(std::numeric_limits<From>::max());
	} else if constexpr (std::is_floating_point_v<From> && std::is_unsigned_v<To>) {
		return static_cast<To>(x * static_cast<From>(std::numeric_limits<To>::max()));
	} else if constexpr (std::is_floating_point_v<From> && std::is_floating_point_v<To>) {
		return static_cast<To>(x);
	} else {
		static_assert(dependent_false_v<To>, "Not supported");
	}
}

[[nodiscard]] inline float toLinearRGBComponent(float x)
{
	return 0.0031308f <= x ? 1.055f * std::pow(x, 1.0f / 2.4f) - 0.055f : 12.92f * x;
}

[[nodiscard]] inline float fromLinearRGBComponent(float x)
{
	return 0.04045f <= x ? std::pow((x + 0.055f) / (1.055f), 2.4f) : x / 12.92f;
}

template <class T>
[[nodiscard]] constexpr T minAlpha(T)
{
	if constexpr (std::is_unsigned_v<T> || std::is_floating_point_v<T>) {
		return T(0);
	} else {
		static_assert(dependent_false_v<T>, "Not supported");
	}
}

template <class T>
[[nodiscard]] constexpr T maxAlpha(T)
{
	if constexpr (std::is_unsigned_v<T>) {
		return std::numeric_limits<T>::max();
	} else if constexpr (std::is_floating_point_v<T>) {
		return T(1);
	} else {
		static_assert(dependent_false_v<T>, "Not supported");
	}
}

template <class To, class From>
[[nodiscard]] constexpr To convertAlpha(From x)
{
	// TODO: Make it more generic. For now we only have `float` and `uint8`.

	if constexpr (std::is_same_v<From, To>) {
		return x;
	} else if constexpr (std::is_unsigned_v<From> && std::is_floating_point_v<To>) {
		return static_cast<To>(x) / static_cast<To>(std::numeric_limits<From>::max());
	} else if constexpr (std::is_floating_point_v<From> && std::is_unsigned_v<To>) {
		return static_cast<To>(x * static_cast<From>(std::numeric_limits<To>::max()));
	} else if constexpr (std::is_floating_point_v<From> && std::is_floating_point_v<To>) {
		return static_cast<To>(x);
	} else {
		static_assert(dependent_false_v<To>, "Not supported");
	}
}

template <class T>
[[nodiscard]] constexpr T initWeight(T)
{
	if constexpr (std::is_floating_point_v<T>) {
		return T(1);
	} else {
		static_assert(dependent_false_v<T>, "Not supported");
	}
}
}  // namespace detail
}  // namespace ufo

#endif  // UFO_VISION_DETAIL_COLOR_HPP