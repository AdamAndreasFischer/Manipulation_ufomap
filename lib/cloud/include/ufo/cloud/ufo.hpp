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

#ifndef UFO_CLOUD_UFO_HPP
#define UFO_CLOUD_UFO_HPP

// UFO
#include <ufo/cloud/cloud.hpp>
#include <ufo/execution/execution.hpp>
#include <ufo/math/transform.hpp>
#include <ufo/math/vec.hpp>
#include <ufo/vision/color.hpp>

// STL
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>

namespace ufo
{
namespace detail
{
// template <
//     class ExecutionPolicy, class... T,
//     std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> =
//     true>
// void readCloudUFO(ExecutionPolicy&& policy, happly::UFOData& ply, Cloud<T...>& cloud)
// {
// 	cloud.clear();

// 	std::size_t max_size{};

// 	auto vertices = ply.getVertexPositions();

// 	// std::vector<std::vector<std::size_t>> indices;
// 	// if (ply.hasElement("face")) {
// 	// 	auto& face = ply.getElement("face");
// 	// 	if (face.hasProperty("vertex_indices") || face.hasProperty("vertex_index")) {
// 	// 		// FIXME: Maybe check type as well?
// 	// 		indices = ply.getFaceIndices<std::size_t>();
// 	// 	}
// 	// }

// 	if constexpr (contains_type_v<Vec3f, T...>) {
// 		auto& points = get<Vec3f>(cloud);

// 		if constexpr (std::is_same_v<execution::sequenced_policy,
// 		                             std::decay_t<ExecutionPolicy>>) {
// 			points.reserve(vertices.size());

// 			for (auto const& v : vertices) {
// 				points.emplace_back(v[0], v[1], v[2]);
// 			}
// 		} else {
// 			points.resize(vertices.size());

// 			std::transform(policy, vertices.begin(), vertices.end(), points.begin(),
// 			               [](auto const& v) { return Vec3f(v[0], v[1], v[2]); });
// 		}

// 		max_size = std::max(max_size, points.size());
// 	}

// 	if constexpr (contains_type_v<Vec3d, T...>) {
// 		auto& points = get<Vec3d>(cloud);

// 		if constexpr (std::is_same_v<execution::sequenced_policy,
// 		                             std::decay_t<ExecutionPolicy>>) {
// 			points.reserve(vertices.size());

// 			for (auto const& v : vertices) {
// 				points.emplace_back(v[0], v[1], v[2]);
// 			}
// 		} else {
// 			points.resize(vertices.size());

// 			std::transform(policy, vertices.begin(), vertices.end(), points.begin(),
// 			               [](auto const& v) { return Vec3d(v[0], v[1], v[2]); });
// 		}

// 		max_size = std::max(max_size, points.size());
// 	}

// 	if constexpr (contains_type_v<Color, T...>) {
// 		auto& vertex = ply.getElement("vertex");
// 		if (vertex.hasProperty("red") && vertex.hasProperty("green") &&
// 		    vertex.hasProperty("blue")) {
// 			auto v_colors = ply.getVertexColors();

// 			auto& colors = get<Color>(cloud);

// 			if constexpr (std::is_same_v<execution::sequenced_policy,
// 			                             std::decay_t<ExecutionPolicy>>) {
// 				colors.reserve(v_colors.size());

// 				for (auto const& c : v_colors) {
// 					colors.emplace_back(c[0], c[1], c[2]);
// 				}
// 			} else {
// 				colors.resize(v_colors.size());

// 				std::transform(policy, v_colors.begin(), v_colors.end(), colors.begin(),
// 				               [](auto const& c) { return Color(c[0], c[1], c[2]); });
// 			}

// 			max_size = std::max(max_size, colors.size());
// 		}
// 	}

// 	(get<T>(cloud).resize(max_size), ...);
// }

template <class T, class Cloud>
void writeCloudUFO(std::ostream& out, Cloud const& cloud)
{
	auto data = cloud.template view<T>();

	out << data.size() << ' ' << sizeof(T) << ' ';

	if constexpr (std::is_same_v<Vec3f, T>) {
		out << "vec3f";
	} else if constexpr (std::is_same_v<Color, T>) {
		out << "color";
	} else {
		out << "unknown";
	}
	out << '\n';

	out.write(reinterpret_cast<char const*>(data.data()), data.size() * sizeof(T));
}
}  // namespace detail

template <class... T>
void readCloudUFO(std::filesystem::path const& file, Cloud<T...>& cloud)
{
	std::ifstream ifs;
	ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	ifs.imbue(std::locale());
	ifs.open(file, std::ios::in | std::ios::binary);

	readCloudUFO(ifs, cloud);

	ifs.close();
}

template <class... T>
void readCloudUFO(std::istream& in, Cloud<T...>& cloud)
{
	std::string line;
	std::getline(in, line);
	if ("# UFO cloud file" != line) {
		return;
	}

	std::size_t num_elements;
	std::size_t element_size;
	std::string type;

	std::size_t max_num_elements{};

	while (std::getline(in, line) && in.good()) {
		std::stringstream ss(line);
		ss >> num_elements >> element_size >> type;

		// TODO: Add checks that cloud contains the data fields

		if ("vec3f" == type) {
			max_num_elements = std::max(max_num_elements, num_elements);

			auto data = cloud.template view<Vec3f>();
			data.resize(num_elements);
			in.read(reinterpret_cast<char*>(data.data()), num_elements * element_size);
		} else if ("color" == type) {
			max_num_elements = std::max(max_num_elements, num_elements);

			auto data = cloud.template view<Color>();
			data.resize(num_elements);
			in.read(reinterpret_cast<char*>(data.data()), num_elements * element_size);

			// TODO: Remove
			// for (auto& c : get<Color>(cloud)) {
			// 	std::swap(c.alpha, c.red);
			// 	std::swap(c.green, c.blue);
			// }
		} else {
			in.seekg(num_elements * element_size, std::ios::cur);
		}
	}

	cloud.reisze(max_num_elements);
}

template <class... T, std::size_t Dim, class T2>
void readCloudUFO(std::filesystem::path const& file, Cloud<T...>& cloud,
                  Transform<Dim, T2>& pose)
{
	std::ifstream ifs;
	ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	ifs.imbue(std::locale());
	ifs.open(file, std::ios::in | std::ios::binary);

	readCloudUFO(ifs, cloud, pose);

	ifs.close();
}

template <class... T, std::size_t Dim, class T2>
void readCloudUFO(std::istream& in, Cloud<T...>& cloud, Transform<Dim, T2>& pose)
{
	std::string line;
	std::getline(in, line);
	if ("# UFO cloud file" != line) {
		return;
	}

	std::size_t num_elements;
	std::size_t element_size;
	std::string type;

	std::size_t max_num_elements{};

	while (std::getline(in, line) && in.good()) {
		std::stringstream ss(line);
		ss >> num_elements >> element_size >> type;

		// TODO: Add checks that cloud contains the data fields

		if ("pose" == type) {
			in.read(reinterpret_cast<char*>(&pose), num_elements * element_size);
		} else if ("vec3f" == type) {
			max_num_elements = std::max(max_num_elements, num_elements);

			auto data = cloud.template view<Vec3f>();
			data.resize(num_elements);
			in.read(reinterpret_cast<char*>(data.data()), num_elements * element_size);
		} else if ("color" == type) {
			max_num_elements = std::max(max_num_elements, num_elements);

			auto data = cloud.template view<Color>();
			data.resize(num_elements);
			in.read(reinterpret_cast<char*>(data.data()), num_elements * element_size);

			// TODO: Remove
			// for (auto& c : get<Color>(cloud)) {
			// 	std::swap(c.alpha, c.red);
			// 	std::swap(c.green, c.blue);
			// }
		} else {
			in.seekg(num_elements * element_size, std::ios::cur);
		}
	}

	cloud.resize(max_num_elements);
}

template <class... T>
void writeCloudUFO(std::filesystem::path const& file, Cloud<T...> const& cloud)
{
	std::ofstream ofs;
	ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
	ofs.imbue(std::locale());
	ofs.open(file, std::ios::out | std::ios::binary);

	writeCloudUFO(ofs, cloud);

	ofs.close();
}

template <class... T>
void writeCloudUFO(std::ostream& out, Cloud<T...> const& cloud)
{
	out << "# UFO cloud file\n";

	(detail::writeCloudUFO<T>(out, cloud), ...);

	out << "# end";
}

template <class... T, std::size_t Dim, class T2>
void writeCloudUFO(std::filesystem::path const& file, Cloud<T...> const& cloud,
                   Transform<Dim, T2> const& pose)
{
	std::ofstream ofs;
	ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
	ofs.imbue(std::locale());
	ofs.open(file, std::ios::out | std::ios::binary);

	writeCloudUFO(ofs, cloud, pose);

	ofs.close();
}

template <class... T, std::size_t Dim, class T2>
void writeCloudUFO(std::ostream& out, Cloud<T...> const& cloud,
                   Transform<Dim, T2> const& pose)
{
	out << "# UFO cloud file\n";

	out << "1 " << sizeof(pose) << " pose\n";
	out.write(reinterpret_cast<char const*>(&pose), sizeof(pose));

	(detail::writeCloudUFO<T>(out, cloud), ...);

	out << "# end";
}
}  // namespace ufo

#endif  // UFO_CLOUD_UFO_HPP