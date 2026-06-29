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

#ifndef UFO_VISION_IMAGE_IO_HPP
#define UFO_VISION_IMAGE_IO_HPP

// UFO
#include <ufo/vision/color.hpp>
#include <ufo/vision/image.hpp>

// STL
#include <filesystem>

// STB
#define STB_IMAGE_IMPLEMENTATION
#include <ufo/vision/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ufo/vision/stb_image_write.h>

namespace ufo
{
inline void write(std::filesystem::path const& file, Image<Color> const& image)
{
	auto ext = file.extension();
	if (".png" == ext) {
		stbi_write_png(file.c_str(), image.cols(), image.rows(), 4, image.data(),
		               4 * image.cols());
	} else if (".jpg" == ext) {
		stbi_write_jpg(file.c_str(), image.cols(), image.rows(), 4, image.data(), 42);
	}
}

[[nodiscard]] inline Image<Color> read(std::filesystem::path const& file)
{
	int            x, y, n;
	unsigned char* data = stbi_load(file.c_str(), &x, &y, &n, 0);

	if (nullptr == data) {
		// TODO: Error
	}

	Image<Color> image(y, x);
	if (1 == n) {
		for (int i{}; x > i; ++i) {
			for (int j{}; y > j; ++j) {
				image(j, i).red   = *data;
				image(j, i).green = *data;
				image(j, i).blue  = *data;
				image(j, i).alpha = std::numeric_limits<Color::value_type>::max();
				++data;
			}
		}
	} else if (2 == n) {
		for (int i{}; x > i; ++i) {
			for (int j{}; y > j; ++j) {
				image(j, i).red   = data[0];
				image(j, i).green = data[0];
				image(j, i).blue  = data[0];
				image(j, i).alpha = data[1];
				data += 2;
			}
		}
	} else if (3 == n) {
		for (int i{}; x > i; ++i) {
			for (int j{}; y > j; ++j) {
				image(j, i).red   = data[0];
				image(j, i).green = data[1];
				image(j, i).blue  = data[2];
				image(j, i).alpha = std::numeric_limits<Color::value_type>::max();
				data += 3;
			}
		}
	} else if (4 == n) {
		for (int i{}; x > i; ++i) {
			for (int j{}; y > j; ++j) {
				image(j, i).red   = data[0];
				image(j, i).green = data[1];
				image(j, i).blue  = data[2];
				image(j, i).alpha = data[3];
				data += 4;
			}
		}
	} else {
		// TODO: Error
	}

	return image;
}
}  // namespace ufo

#endif  // UFO_VISION_IMAGE_IO_HPP