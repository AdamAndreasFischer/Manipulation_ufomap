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

#ifndef UFO_VISION_COLOR_TYPE_HPP
#define UFO_VISION_COLOR_TYPE_HPP

namespace ufo
{
enum class ColorType {
	RGB8U,   // Red, green, and blue channels. 8 bit unsigned integer per channel.
	RGBA8U,  // Red, green, blue, and alpha channels. 8 bit unsigned integer per
	         // channel.

	// TODO: RGB8UW32F,  // Red, green, and blue channels. 8 bit unsigned integer per
	// channel. Weight channel, 32 bit float.
	// TODO: RGBA8UW32F,  // Red, green, blue, alpha channels. 8 bit unsigned integer per
	// channel. Weight channel, 32 bit float.

	RGB32F,    // Red, green, and blue channels. 32 bit float per channel.
	RGBA32F,   // Red, green, blue, and alpha channels. 32 bit float per channel.
	RGBW32F,   // Red, green, blue, and weight channels. 32 bit float per channel.
	RGBAW32F,  // Red, green, blue, alpha, and weight channels. 32 bit float per
	           // channel.

	LAB32F,    // Lightness, a, and b channels. 32 bit float per channel.
	LABA32F,   // Lightness, a, b, and alpha channels. 32 bit float per channel.
	LABW32F,   // Lightness, a, b, and weight channels. 32 bit float per channel.
	LABAW32F,  // Lightness, a, b, alpha, and weight channels. 32 bit float per channel.

	LCH32F,   // Lightness, chroma, and hue channels. 32 bit float per channel.
	LCHA32F,  // Lightness, chroma, hue, and alpha channels. 32 bit float per channel.
	LCHW32F,  // Lightness, chroma, hue, and weight channels. 32 bit float per channel.
	LCHAW32F  // Lightness, chroma, hue, alpha, and weight channels. 32 bit float per
	          // channel.

	// Maybe add xyz, Luv, and LCh_uv (then the current LCh needs to be called LCh_ab)
};
}  // namespace ufo

#endif  // UFO_VISION_COLOR_TYPE_HPP