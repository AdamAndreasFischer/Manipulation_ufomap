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

#ifndef UFO_VISION_CAMERA_HPP
#define UFO_VISION_CAMERA_HPP

// UFO
#include <ufo/geometry/ray.hpp>
#include <ufo/math/mat4x4.hpp>
#include <ufo/math/transform3.hpp>
#include <ufo/math/vec.hpp>
#include <ufo/utility/index_iterator.hpp>
#include <ufo/vision/image.hpp>

// STL
#include <cassert>
#include <cmath>
#include <numeric>

namespace ufo
{
enum class ProjectionType { PERSPECTIVE, ORTHOGONAL };

/*!
 * @brief Camera
 *
 */
struct Camera {
	// The pose of the camera in the world, same as the transform that takes you from camera
	// to world
	Transform3f    pose;
	std::size_t    rows;
	std::size_t    cols;
	float          vertical_fov;
	float          near_clip;
	float          far_clip;
	float          zoom;  // TODO: Implement
	Vec3f          up{0.0f, 0.0f, 1.0f};
	ProjectionType projection_type = ProjectionType::PERSPECTIVE;

	float w{};  // TODO: Only used for 4D

	template <bool RightHanded = true>
	void lookAt(Vec3f center, Vec3f const& target)
	{
		// We should probably inverse here
		pose = static_cast<Transform3f>(ufo::lookAt<float, RightHanded>(center, target, up));
	}

	[[nodiscard]] Mat4x4f projection() const
	{
		switch (projection_type) {
			case ProjectionType::PERSPECTIVE: return projectionPerspective();
			case ProjectionType::ORTHOGONAL: return projectionOrthogonal();
		}

		assert(false);
		return Mat4x4f();
	}

	[[nodiscard]] Mat4x4f view() const { return Mat4x4f(pose); }

	[[nodiscard]] Image<Ray3> rays() const
	{
		switch (projection_type) {
			case ProjectionType::PERSPECTIVE: return raysPerspective();
			case ProjectionType::ORTHOGONAL: return raysOrthogonal();
		}

		assert(false);
		return Image<Ray3>(0, 0);
	}

	template <class ExecutionPolicy>
	[[nodiscard]] Image<Ray3> rays(ExecutionPolicy&& policy) const
	{
		switch (projection_type) {
			case ProjectionType::PERSPECTIVE:
				return raysPerspective(std::forward<ExecutionPolicy>(policy));
			case ProjectionType::ORTHOGONAL:
				return raysOrthogonal(std::forward<ExecutionPolicy>(policy));
		}

		assert(false);
		return Image<Ray3>(0, 0);
	}

 private:
	[[nodiscard]] Image<Ray3> raysPerspective() const
	{
		auto proj_inv = inverse(projection());
		auto view_inv = inverse(view());

		Image<Ray3> rays(rows, cols);

		Vec3f origin(view_inv * Vec4f(Vec3f(0), 1));

		for (std::size_t row{}; rows > row; ++row) {
			auto r = ((row + 0.5f) / rows) * 2.0f - 1.0f;
			for (std::size_t col{}; cols > col; ++col) {
				auto  c = ((col + 0.5f) / cols) * 2.0f - 1.0f;
				Vec4f p_nds_h(c, r, -1.0f, 1.0f);
				auto  dir_eye            = proj_inv * p_nds_h;
				dir_eye.w                = 0.0f;
				rays(row, col).origin    = origin;
				rays(row, col).direction = normalize(Vec3f(view_inv * dir_eye));
			}
		}

		return rays;
	}

	[[nodiscard]] Image<Ray3> raysOrthogonal() const
	{
		// TODO: Implement

		Image<Ray3> rays(rows, cols);

		// TODO: Implement

		return rays;
	}

	template <class ExecutionPolicy>
	[[nodiscard]] Image<Ray3> raysPerspective(ExecutionPolicy&& policy) const
	{
		auto fun = [rows = rows, cols = cols, proj_inv = inverse(projection()),
		            view_inv = inverse(view())](std::size_t row, std::size_t col) {
			auto  r = ((row + 0.5f) / rows) * 2.0f - 1.0f;
			auto  c = ((col + 0.5f) / cols) * 2.0f - 1.0f;
			Vec4f p_nds_h(c, r, -1.0f, 1.0f);
			auto  dir_eye  = proj_inv * p_nds_h;
			dir_eye.w      = 0.0f;
			auto dir_world = normalize(Vec3f(view_inv * dir_eye));
			return Ray3(Vec3f(view_inv * Vec4f(Vec3f(0), 1)), dir_world);
		};

		if constexpr (execution::is_stl_v<ExecutionPolicy>) {
			Image<Ray3> rays(rows, cols);

			IndexIterator<std::size_t> indices(0, rows);
			std::for_each(execution::toSTL(policy), indices.begin(), indices.end(),
			              [&rays, fun, cols = cols](std::size_t row) {
				              for (std::size_t col{}; col < cols; ++col) {
					              rays(row, col) = fun(row, col);
				              }
			              });

			return rays;
		}
#if defined(UFO_PAR_GCD)
		else if constexpr (execution::is_gcd_v<ExecutionPolicy>) {
			__block Image<Ray3> rays(rows, cols);

			dispatch_apply(rows, dispatch_get_global_queue(0, 0), ^(std::size_t row) {
				for (std::size_t col{}; col < cols; ++col) {
					rays(row, col) = fun(row, col);
				}
			});

			return rays;
		}
#endif
#if defined(UFO_PAR_TBB)
		else if constexpr (execution::is_tbb_v<ExecutionPolicy>) {
			Image<Ray3> rays(rows, cols);

			oneapi::tbb::parallel_for(std::size_t(0), rows,
			                          [&rays, fun, cols = cols](std::size_t row) {
				                          for (std::size_t col{}; col < cols; ++col) {
					                          rays(row, col) = fun(row, col);
				                          }
			                          });

			return rays;
		}
#endif
		else if constexpr (execution::is_omp_v<ExecutionPolicy>) {
			Image<Ray3> rays(rows, cols);

			if constexpr (execution::is_seq_v<ExecutionPolicy>) {
				for (std::size_t row = 0; row < rows; ++row) {
					for (std::size_t col = 0; col < cols; ++col) {
						rays(col, row) = fun(row, col);
					}
				};
			} else if constexpr (execution::is_unseq_v<ExecutionPolicy>) {
#pragma omp simd
				for (std::size_t row = 0; row < rows; ++row) {
					for (std::size_t col = 0; col < cols; ++col) {
						rays(col, row) = fun(row, col);
					}
				};
			} else if constexpr (execution::is_par_v<ExecutionPolicy>) {
#pragma omp parallel for
				for (std::size_t row = 0; row < rows; ++row) {
					for (std::size_t col = 0; col < cols; ++col) {
						rays(col, row) = fun(row, col);
					}
				};
			} else if constexpr (execution::is_par_unseq_v<ExecutionPolicy>) {
#pragma omp parallel for simd
				for (std::size_t row = 0; row < rows; ++row) {
					for (std::size_t col = 0; col < cols; ++col) {
						rays(col, row) = fun(row, col);
					}
				};
			}

			return rays;
		} else {
			static_assert(dependent_false_v<ExecutionPolicy>,
			              "Not implemented for the execution policy");
		}
	}

	template <class ExecutionPolicy>
	[[nodiscard]] Image<Ray3> raysOrthogonal(ExecutionPolicy&& policy) const
	{
		Mat4x4f proj     = projectionOrthogonal();
		Mat4x4f proj_inv = inverse(proj);

		Mat4x4f view(pose);
		Mat4x4f view_inv = inverse(view);

		// std::cout << "Projection\n" << proj << '\n';
		// std::cout << "View\n" << view << '\n';

		Image<Ray3> rays(rows, cols);

		auto fun = [&](std::size_t row) {
			auto r = ((row + 0.5f) / rows) * 2.0f - 1.0f;
			for (std::size_t col{}; col < cols; ++col) {
				auto  c = ((col + 0.5f) / cols) * 2.0f - 1.0f;
				Vec4f p_nds_h(c, r, -1.0f, 1.0f);
				auto  dir_eye              = proj_inv * p_nds_h;
				dir_eye.w                  = 0.0f;
				auto dir_world             = normalize(Vec3f(view_inv * dir_eye));
				rays(row, col).direction   = dir_world;
				rays(row, col).direction.x = 0;
				rays(row, col).direction.y = 0;
				// rays(row, col).direction.z = 0;
				rays(row, col).origin = Vec3f(view_inv * Vec4f(Vec3f(0), 1));

				// static auto the_id = std::this_thread::get_id();
				// if (std::this_thread::get_id() == the_id && 0 == row && 0 == col) {
				// std::cout << pose.translation << '\n';
				// std::cout << rays(row, col).direction << "\n\n";
				// }

				// rays(row, col).origin = Vec3f(5.0f, 4.0f, 1.5f);
			}
		};

		return rays;

		// 		if constexpr (execution::is_seq_v<ExecutionPolicy> ||
		// 		              execution::is_unseq_v<ExecutionPolicy>) {
		// 			for (std::size_t row{}; row < rows; ++row) {
		// 				fun(row);
		// 			};

		// 			return rays;
		// 		} else if constexpr (execution::is_par_v<ExecutionPolicy> ||
		// 		                     execution::is_par_unseq_v<ExecutionPolicy>) {
		// 			std::vector<std::size_t> indices(rows);
		// 			std::iota(indices.begin(), indices.end(), 0);
		// 			if constexpr (execution::is_par_v<ExecutionPolicy>) {
		// 				std::for_each(UFO_PAR_STL_PAR indices.begin(), indices.end(), fun);
		// 			} else {
		// 				std::for_each(UFO_PAR_STL_PAR_UNSEQ indices.begin(), indices.end(), fun);
		// 			}
		// 			return rays;
		// 		}
		// #if defined(UFO_PAR_GCD)
		// 		else if constexpr (execution::is_gcd_v<ExecutionPolicy> ||
		// 		                   execution::is_gcd_unseq_v<ExecutionPolicy>) {
		// 			// TODO: Implement
		// 			static_assert(dependent_false_v<ExecutionPolicy>,
		// 			              "Not implemented for the execution policy");
		// 		}
		// #endif
		// 		else if constexpr (execution::is_tbb_v<ExecutionPolicy> ||
		// 		                   execution::is_tbb_unseq_v<ExecutionPolicy>) {
		// 			// TODO: Implement
		// 			static_assert(dependent_false_v<ExecutionPolicy>,
		// 			              "Not implemented for the execution policy");
		// 		} else if constexpr (execution::is_omp_v<ExecutionPolicy> ||
		// 		                     execution::is_omp_unseq_v<ExecutionPolicy>) {
		// #pragma omp parallel for
		// 			for (std::size_t row = 0; row < rows; ++row) {
		// 				fun(row);
		// 			};
		// 			return rays;
		// 		} else {
		// 			static_assert(dependent_false_v<ExecutionPolicy>,
		// 			              "Not implemented for the execution policy");
		// 		}
	}

	[[nodiscard]] Mat4x4f projectionPerspective() const
	{
		// if (std::isinf(far_clip)) {
		// 	return infinitePerspective(vertical_fov, cols / static_cast<float>(rows),
		// 	                           near_clip);
		// } else {
		// 	return perspective<float, true, true>(vertical_fov, cols /
		// static_cast<float>(rows), 	                                      near_clip,
		// far_clip);
		// }

		// double fx     = 424.712982;
		// double fy     = 423.633575;
		double tan_half_fovy = std::tan(vertical_fov / 2.0);
		double fx     = (cols / 2) / ((cols / static_cast<double>(rows)) * tan_half_fovy);
		double fy     = (rows / 2) / tan_half_fovy;
		double cx     = 427.887634;
		double cy     = 239.666061;
		double s      = 0;
		int    width  = cols;
		int    height = rows;
		double near   = near_clip;
		double far    = far_clip;  // 0;  // far_clip, 0 = inf

		double left   = cx * near / -fx;
		double top    = cy * near / fy;
		double right  = -(width - cx) * near / -fx;
		double bottom = -(height - cy) * near / fy;

		double inv_w = 1 / (right - left);
		double inv_h = 1 / (top - bottom);
		double inv_d = 1 / (far - near);

		double A = 2 * near * inv_w;
		double B = 2 * near * inv_h;
		double C = (right + left) * inv_w;
		double D = (top + bottom) * inv_h;
		double q, qn;
		if (far == 0) {
			double const INFINITE_FAR_PLANE_ADJUST = 0.00001;
			// Infinite far plane
			q  = INFINITE_FAR_PLANE_ADJUST - 1;
			qn = near * (INFINITE_FAR_PLANE_ADJUST - 2);
		} else {
			q  = -(far + near) * inv_d;
			qn = -2 * (far * near) * inv_d;
			// q  = far / (near - far);
			// qn = -(far * near) / (far - near);
		}

		// clang-format off
		// return Mat4x4f(
		// 	2 * fx / W, 0, 0, 0,
		// 	2 * s / W, 2 * fy / H, 0, 0,
		// 	2 * (cx / W) - 1, 2 * (cy / H) - 1, (zmax + zmin) / (zmax - zmin), 1,
		// 	0, 0, 2 * zmax * zmin / (zmin - zmax), 0
		// );
		// return Mat4x4f(
		// 	2 * fx / W, 0, 0, 0,
		// 	2 * s / W, 2 * fy / H, 0, 0,
		// 	2 * (cx / W) - 1, 2 * (cy / H) - 1, - (zmax + zmin) / (zmax - zmin), -1,
		// 	0, 0, -2 * (zmax * zmin) / (zmax - zmin), 0
		// );
		return ufo::transpose(Mat4x4f(
			A, 0, C, 0,
			0, B, D, 0,
			0, 0, q, qn,
			0, 0, -1, 0
		));
		// clang-format on
	}

	[[nodiscard]] Mat4x4f projectionOrthogonal() const
	{
		if (std::isinf(far_clip)) {
			return orthogonal<float>(
			    -(static_cast<float>(cols) / 2.0f), static_cast<float>(cols) / 2.0f,
			    static_cast<float>(rows) / 2.0f, -(static_cast<float>(rows) / 2.0f));
		} else {
			return orthogonal<float, true, true>(
			    -(static_cast<float>(cols) / 2.0f), static_cast<float>(cols) / 2.0f,
			    static_cast<float>(rows) / 2.0f, -(static_cast<float>(rows) / 2.0f), near_clip,
			    far_clip);
		}
	}
};

inline bool operator==(Camera const& lhs, Camera const& rhs)
{
	return lhs.pose == rhs.pose && lhs.rows == rhs.rows && lhs.cols == rhs.cols &&
	       lhs.vertical_fov == rhs.vertical_fov && lhs.near_clip == rhs.near_clip &&
	       lhs.far_clip == rhs.far_clip && lhs.zoom == rhs.zoom && lhs.up == rhs.up &&
	       lhs.projection_type == rhs.projection_type && lhs.w == rhs.w;
}

inline bool operator!=(Camera const& lhs, Camera const& rhs) { return !(lhs == rhs); }

inline std::ostream& operator<<(std::ostream& os, Camera const& camera)
{
	os << "Pose: " << camera.pose << '\n';
	os << "Rows: " << camera.rows << ", Cols: " << camera.cols << '\n';
	os << "Vertical FoV: " << camera.vertical_fov << '\n';
	os << "Near clip: " << camera.near_clip << ", Far clip: " << camera.far_clip << '\n';
	os << "Zoom: " << camera.zoom << '\n';
	os << "Up: " << camera.up << '\n';
	return os;
}
}  // namespace ufo

#endif  // UFO_VISION_CAMERA_HPP