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

#ifndef UFO_CLOUD_PLY_HPP
#define UFO_CLOUD_PLY_HPP

// UFO
#include <ufo/math/vec.hpp>
#include <ufo/cloud/cloud.hpp>
#include <ufo/vision/color.hpp>

// Happly
#include <happly/happly.h>

// STL
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>

namespace ufo
{
namespace detail
{
template <class... T>
void readCloudPLY(happly::PLYData& ply, Cloud<T...>& cloud)
{
	cloud.clear();

	std::size_t max_size{};

	auto vertices = ply.getVertexPositions();

	// std::vector<std::vector<std::size_t>> indices;
	// if (ply.hasElement("face")) {
	// 	auto& face = ply.getElement("face");
	// 	if (face.hasProperty("vertex_indices") || face.hasProperty("vertex_index")) {
	// 		// FIXME: Maybe check type as well?
	// 		indices = ply.getFaceIndices<std::size_t>();
	// 	}
	// }

	if constexpr (contains_type_v<Vec3f, T...>) {
		auto& points = get<Vec3f>(cloud);

		points.reserve(vertices.size());

		for (auto const& v : vertices) {
			points.emplace_back(v[0], v[1], v[2]);
		}

		max_size = std::max(max_size, points.size());
	}

	if constexpr (contains_type_v<Vec3d, T...>) {
		auto& points = get<Vec3d>(cloud);

		points.reserve(vertices.size());

		for (auto const& v : vertices) {
			points.emplace_back(v[0], v[1], v[2]);
		}

		max_size = std::max(max_size, points.size());
	}

	if constexpr (contains_type_v<Color, T...>) {
		auto& vertex = ply.getElement("vertex");
		if (vertex.hasProperty("red") && vertex.hasProperty("green") &&
		    vertex.hasProperty("blue")) {
			auto v_colors = ply.getVertexColors();

			auto& colors = get<Color>(cloud);

			colors.reserve(v_colors.size());

			for (auto const& c : v_colors) {
				colors.emplace_back(c[0], c[1], c[2]);
			}

			max_size = std::max(max_size, colors.size());
		}
	}

	(get<T>(cloud).resize(max_size), ...);
}
}  // namespace detail

template <class... T>
void readCloudPLY(std::filesystem::path const& file, Cloud<T...>& cloud)
{
	happly::PLYData ply(file);
	detail::readCloudPLY(ply, cloud);
}

template <class... T>
void readCloudPLY(std::istream& in, Cloud<T...>& cloud)
{
	happly::PLYData ply(in);
	detail::readCloudPLY(ply, cloud);
}

template <class... T>
void writeCloudPLY(std::filesystem::path const& file, Cloud<T...> const& cloud)
{
	std::ofstream ofs;
	ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
	ofs.imbue(std::locale());
	ofs.open(file, std::ios::out | std::ios::binary);

	writeCloudPLY(ofs, cloud);
}

template <class... T>
void writeCloudPLY(std::ostream& out, Cloud<T...> const& cloud)
{
}
}  // namespace ufo

#endif  // UFO_CLOUD_PLY_HPP