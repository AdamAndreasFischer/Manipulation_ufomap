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

#ifndef UFO_VISION_LAB_HPP
#define UFO_VISION_LAB_HPP

// UFO
#include <ufo/vision/detail/color.hpp>

namespace ufo
{
template <>
struct Color<ColorType::LAB32F> {
	using value_type = float;

	value_type lightness;  // [0, 1]
	value_type a;          // [-0.4, 0.4]
	value_type b;          // [-0.4, 0.4]
};

template <>
struct Color<ColorType::LABA32F> {
	using value_type = float;

	value_type lightness;  // [0, 1]
	value_type a;          // [-0.4, 0.4]
	value_type b;          // [-0.4, 0.4]
	value_type alpha;      // [0, 1]
};

template <>
struct Color<ColorType::LABW32F> {
	using value_type = float;

	value_type lightness;  // [0, 1] * weight
	value_type a;          // [-0.4, 0.4] * weight
	value_type b;          // [-0.4, 0.4] * weight
	value_type weight;
};

template <>
struct Color<ColorType::LABAW32F> {
	using value_type = float;

	value_type lightness;  // [0, 1] * weight
	value_type a;          // [-0.4, 0.4] * weight
	value_type b;          // [-0.4, 0.4] * weight
	value_type alpha;      // [0, 1] * weight
	value_type weight;
};

using Lab32f   = Color<ColorType::LAB32F>;
using LabA32f  = Color<ColorType::LABA32F>;
using LabW32f  = Color<ColorType::LABW32F>;
using LabAW32f = Color<ColorType::LABAW32F>;
}  // namespace ufo

#endif  // UFO_VISION_LAB_HPP