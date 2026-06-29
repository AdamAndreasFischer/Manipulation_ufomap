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

#ifndef UFO_CONTAINER_TREE_RENDER_HPP
#define UFO_CONTAINER_TREE_RENDER_HPP

// UFO
#include <ufo/container/tree/node.hpp>
#include <ufo/container/tree/predicate.hpp>
#include <ufo/container/tree/trace_result.hpp>
#include <ufo/utility/type_traits.hpp>
#include <ufo/vision/camera.hpp>

// STL
#include <utility>

namespace ufo
{
//
// New
//

template <class Tree, class Predicate,
          std::enable_if_t<pred::is_pred_v<Predicate>, bool> = true>
[[nodiscard]] Image<TraceResult<Tree::dimensions()>> render(Tree const&      tree,
                                                            Camera const&    camera,
                                                            Predicate const& pred)
{
	return render(tree.index(), tree, camera, pred);
}

template <class Tree, class NodeType, class Predicate,
          std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool> = true,
          std::enable_if_t<pred::is_pred_v<Predicate>, bool>              = true>
[[nodiscard]] Image<TraceResult<Tree::dimensions()>> render(Tree const&      tree,
                                                            NodeType const&  node,
                                                            Camera const&    camera,
                                                            Predicate const& pred)
{
	if constexpr (2 == Tree::dimensions()) {
		// TODO: Implement
	} else if constexpr (3 == Tree::dimensions()) {
		// TODO: fix this
		return tree.trace(node, camera.rays(), pred);
	} else if constexpr (4 == Tree::dimensions()) {
		// TODO: Implement
	} else {
		static_assert(dependent_false_v<Tree>, "Does not support dimensions");
	}
}

template <
    class ExecutionPolicy, class Tree, class Predicate,
    std::enable_if_t<pred::is_pred_v<Predicate>, bool>                        = true,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
[[nodiscard]] Image<TraceResult<Tree::dimensions()>> render(ExecutionPolicy&& policy,
                                                            Tree const&       tree,
                                                            Camera const&     camera,
                                                            Predicate const&  pred)
{
	return render(std::forward<ExecutionPolicy>(policy), tree, tree.index(), camera, pred);
}

template <
    class ExecutionPolicy, class Tree, class NodeType, class Predicate,
    std::enable_if_t<Tree::template is_node_type_v<NodeType>, bool>           = true,
    std::enable_if_t<pred::is_pred_v<Predicate>, bool>                        = true,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
[[nodiscard]] Image<TraceResult<Tree::dimensions()>> render(ExecutionPolicy&& policy,
                                                            Tree const&       tree,
                                                            NodeType const&   node,
                                                            Camera const&     camera,
                                                            Predicate const&  pred)
{
	if constexpr (2 == Tree::dimensions()) {
		// TODO: Implement
	} else if constexpr (3 == Tree::dimensions()) {
		Image<TraceResult<Tree::dimensions()>> image(camera.rows, camera.cols);
		auto                                   rays = camera.rays();
		tree.trace(std::forward<ExecutionPolicy>(policy), node, rays.begin(), rays.end(),
		           image.begin(), pred, camera.near_clip, camera.far_clip);
		return image;
	} else if constexpr (4 == Tree::dimensions()) {
		// TODO: Implement
	} else {
		static_assert(dependent_false_v<Tree>, "Does not support dimensions");
	}
}

template <class Map, class Predicate = pred::True,
          std::enable_if_t<pred::is_pred_v<Predicate, Map>, bool> = true>
[[nodiscard]] std::pair<Image<typename Map::Node>, Image<float>> renderGPU(
    Map const& map, Camera const& camera, Predicate const& pred = pred::True{},
    bool only_exists = true)
{
	return renderGPU(map.code(), map, camera, pred, only_exists);
}

template <class NodeType, class Map, class Predicate = pred::True,
          std::enable_if_t<Map::template is_node_type_v<NodeType>, bool> = true,
          std::enable_if_t<pred::is_pred_v<Predicate, Map>, bool>        = true>
[[nodiscard]] std::pair<Image<typename Map::Node>, Image<float>> renderGPU(
    NodeType const& node, Map const& map, Camera const& camera,
    Predicate pred = pred::True{}, bool only_exists = true)
{
	using Filter = pred::Filter<Predicate>;

	Filter::init(pred, map);

	// TODO: Init GPU

	// TODO: Add shader to some list

	// TODO: Call

	std::pair<Image<typename Map::Node>, Image<float>> ret;

	auto& nodes  = ret.first;
	auto& depths = ret.second;

	nodes.resize(camera.cols, camera.rows);
	depths.resize(camera.cols, camera.rows);

	// TODO: Implement

	return ret;
}

#if 0
//
// Quadtree
//

template <class InnerFun, class HitFun, class T>
void render(Camera const& camera, Image<T>& image, InnerFun inner_f, HitFun hit_f,
            T const& miss)
{
	render(Base::index(), camera, image, inner_f, hit_f, miss);
}

template <class InnerFun, class HitFun, class T>
[[nodiscard]] Image<T> render(Camera const& camera, std::size_t rows, std::size_t cols,
                              InnerFun inner_f, HitFun hit_f, T const& miss)
{
	return render(Base::index(), camera, rows, cols, inner_f, hit_f, miss);
}

template <class NodeType, class InnerFun, class HitFun, class T,
          std::enable_if_t<Base::template is_node_type_v<NodeType>, bool> = true>
void render(NodeType node, Camera const& camera, Image<T>& image, InnerFun inner_f,
            HitFun hit_f, T const& miss)
{
	Image<Ray3> rays = camera.rays(image.rows(), image.cols());
	trace3D(node, rays.begin(), rays.end(), image.begin(), inner_f, hit_f, miss);
}

template <class NodeType, class InnerFun, class HitFun, class T,
          std::enable_if_t<Base::template is_node_type_v<NodeType>, bool> = true>
[[nodiscard]] Image<T> render(NodeType node, Camera const& camera, std::size_t rows,
                              std::size_t cols, InnerFun inner_f, HitFun hit_f,
                              T const& miss)
{
	Image<T> image(rows, cols);
	render(node, camera, image, inner_f, hit_f, miss);
	return image;
}

template <
    class ExecutionPolicy, class InnerFun, class HitFun, class T,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
void render(ExecutionPolicy&& policy, Camera const& camera, Image<T>& image,
            InnerFun inner_f, HitFun hit_f, T const& miss)
{
	render(std::forward<ExecutionPolicy>(policy), Base::index(), camera, image, inner_f,
	       hit_f, miss);
}

template <
    class ExecutionPolicy, class InnerFun, class HitFun, class T,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
[[nodiscard]] Image<T> render(ExecutionPolicy&& policy, Camera const& camera,
                              std::size_t rows, std::size_t cols, InnerFun inner_f,
                              HitFun hit_f, T const& miss)
{
	return render(std::forward<ExecutionPolicy>(policy), Base::index(), camera, rows, cols,
	              inner_f, hit_f, miss);
}

template <class ExecutionPolicy, class NodeType, class InnerFun, class HitFun, class T>
void render(ExecutionPolicy&& policy, NodeType node, Camera const& camera,
            Image<T>& image, InnerFun inner_f, HitFun hit_f, T const& miss)
{
	auto rows = image.rows();
	auto cols = image.cols();

	Image<Ray3> rays = camera.rays(policy, rows, cols);

	trace3D(std::forward<ExecutionPolicy>(policy), node, rays.begin(), rays.end(),
	        image.begin(), inner_f, hit_f, miss);
}

template <class ExecutionPolicy, class NodeType, class InnerFun, class HitFun, class T>
[[nodiscard]] Image<T> render(ExecutionPolicy&& policy, NodeType node,
                              Camera const& camera, std::size_t rows, std::size_t cols,
                              InnerFun inner_f, HitFun hit_f, T const& miss)
{
	Image<T> image(rows, cols);
	render(policy, node, camera, image, inner_f, hit_f, miss);
	return image;
}

template <class NodeType, class InputIt, class OutputIt, class InnerFun, class HitFun,
          class T, std::enable_if_t<Base::template is_node_type_v<NodeType>, bool> = true>
OutputIt trace3D(NodeType node, InputIt first, InputIt last, OutputIt d_first,
                 InnerFun inner_f, HitFun hit_f, T const& miss)
{
	if constexpr (!std::is_same_v<Index, std::decay_t<NodeType>>) {
		// Unless NodeType is Index, we need to check that the node actually exists
		if (!Base::exists(node)) {
			return miss;
		}
	}

	Index n = Base::index(node);

	auto center      = Base::center(n);
	auto half_length = Base::halfLength(n);

	return std::transform(first, last, d_first, [&](auto const& ray) {
		return trace3D(n, center, half_length, ray, inner_f, hit_f, miss);
	});
}

template <class ExecutionPolicy, class NodeType, class RandomIt1, class RandomIt2,
          class InnerFun, class HitFun, class T>
RandomIt2 trace3D(ExecutionPolicy&& policy, NodeType node, RandomIt1 first,
                  RandomIt1 last, RandomIt2 d_first, InnerFun inner_f, HitFun hit_f,
                  T const& miss)
{
	if constexpr (std::is_same_v<execution::sequenced_policy,
	                             std::decay_t<ExecutionPolicy>>) {
		return trace3D(node, first, last, d_first, inner_f, hit_f, miss);
	}

#if !defined(UFO_TBB) && !defined(UFO_OMP)
	return trace3D(node, first, last, d_first, inner_f, hit_f, miss);
#endif

	if constexpr (!std::is_same_v<Index, std::decay_t<NodeType>>) {
		// Unless NodeType is Index, we need to check that the node actually exists
		if (!Base::exists(node)) {
			return miss;
		}
	}

	Index n = Base::index(node);

	auto center      = Base::center(n);
	auto half_length = Base::halfLength(n);

#if defined(UFO_TBB)
	return std::transform(
	    std::forward<ExecutionPolicy>(policy), first, last, d_first, [&](auto const& ray) {
		    return trace3D(n, center, half_length, ray, inner_f, hit_f, miss);
	    });
#elif defined(UFO_OMP)
	std::size_t size = std::distance(first, last);

#pragma omp parallel for
	for (std::size_t i = 0; i != size; ++i) {
		auto const& ray = first[i];
		d_first[i]      = trace3D(n, center, half_length, ray, inner_f, hit_f, miss);
	}

	return std::next(d_first, size);
#endif
}

template <class InnerFun, class HitFun, class T>
[[nodiscard]] T trace3D(Index node, Vec2f const& center, float half_length,
                        Ray3 const& ray, InnerFun inner_f, HitFun hit_f,
                        T const& miss)
{
	auto wrapped_inner_f = [&ray, inner_f](Index node, float distance) {
		return inner_f(node, ray, distance);
	};

	auto wrapped_hit_f = [&ray, hit_f](Index node, float distance) {
		return hit_f(node, ray, distance);
	};

	if (0.0f == ray.origin.z && 0.0f == ray.direction.z) {
		// 2D case
		Ray2 ray2(Vec2f(ray.origin), Vec2f(ray.direction));
		auto params = Base::traceInit(ray2, center, half_length);
		return Base::trace(node, params, wrapped_inner_f, wrapped_hit_f, miss);
	} else if (0.0f == ray.direction.z || sign(ray.origin.z) == sign(ray.direction.z)) {
		// Ray never intersects the 2D XY plane at Z=0
		return miss;
	}

	// Check where 3D ray intersects with 2D XY plane at z=0
	float t    = -ray.origin.z / ray.direction.z;
	Vec2f v    = Vec2f(ray.origin) + t * Vec2f(ray.direction);
	auto  code = Base::code(v);

	if (Base::code(node) != code.toDepth(Base::depth(node))) {
		// Different part of the tree, so return miss
		return miss;
	}

	for (int depth = Base::depth(node); 0 < depth; --depth) {
		if (auto [hit, value] = wrapped_hit_f(node, t); hit) {
			return value;
		} else if (Base::isLeaf(node) || !wrapped_inner_f(node, t)) {
			return miss;
		}

		node = Base::child(node, code.offset(depth - 1));
	}

	if (auto [hit, value] = wrapped_hit_f(node, t); hit) {
		return value;
	}

	return miss;
}

//
// Octree
//

template <class InnerFun, class HitFun, class T>
void render(Camera const& camera, Image<T>& image, InnerFun inner_f, HitFun hit_f,
            T const& miss)
{
	render(Base::index(), camera, image, inner_f, hit_f, miss);
}

template <class InnerFun, class HitFun, class T>
[[nodiscard]] Image<T> render(Camera const& camera, std::size_t rows, std::size_t cols,
                              InnerFun inner_f, HitFun hit_f, T const& miss)
{
	return render(Base::index(), camera, rows, cols, inner_f, hit_f, miss);
}

template <class NodeType, class InnerFun, class HitFun, class T,
          std::enable_if_t<Base::template is_node_type_v<NodeType>, bool> = true>
void render(NodeType node, Camera const& camera, Image<T>& image, InnerFun inner_f,
            HitFun hit_f, T const& miss)
{
	Image<Ray3> rays = camera.rays(image.rows(), image.cols());
	Base::trace(node, rays.begin(), rays.end(), image.begin(), inner_f, hit_f, miss);
}

template <class NodeType, class InnerFun, class HitFun, class T,
          std::enable_if_t<Base::template is_node_type_v<NodeType>, bool> = true>
[[nodiscard]] Image<T> render(NodeType node, Camera const& camera, std::size_t rows,
                              std::size_t cols, InnerFun inner_f, HitFun hit_f,
                              T const& miss)
{
	Image<T> image(rows, cols);
	render(node, camera, image, inner_f, hit_f, miss);
	return image;
}

template <
    class ExecutionPolicy, class InnerFun, class HitFun, class T,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
void render(ExecutionPolicy&& policy, Camera const& camera, Image<T>& image,
            InnerFun inner_f, HitFun hit_f, T const& miss)
{
	render(std::forward<ExecutionPolicy>(policy), Base::index(), camera, image, inner_f,
	       hit_f, miss);
}

template <
    class ExecutionPolicy, class InnerFun, class HitFun, class T,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
[[nodiscard]] Image<T> render(ExecutionPolicy&& policy, Camera const& camera,
                              std::size_t rows, std::size_t cols, InnerFun inner_f,
                              HitFun hit_f, T const& miss)
{
	return render(std::forward<ExecutionPolicy>(policy), Base::index(), camera, rows, cols,
	              inner_f, hit_f, miss);
}

template <class ExecutionPolicy, class NodeType, class InnerFun, class HitFun, class T>
void render(ExecutionPolicy&& policy, NodeType node, Camera const& camera,
            Image<T>& image, InnerFun inner_f, HitFun hit_f, T const& miss)
{
	auto rows = image.rows();
	auto cols = image.cols();

	Image<Ray3> rays = camera.rays(policy, rows, cols);

	constexpr std::size_t const step_size = 1;

	if constexpr (1 == step_size) {
		Base::trace(std::forward<ExecutionPolicy>(policy), node, rays.begin(), rays.end(),
		            image.begin(), inner_f, hit_f, miss);
	} else {
		// Image<std::pair<TreeIndex, float>> iter_image(
		//     rows, cols, std::make_pair(TreeIndex{}, std::numeric_limits<float>::max()));

		Index n = Base::index(node);

		auto center      = Base::center(n);
		auto half_length = Base::halfLength(n);

		std::vector<std::size_t> row_idx(rows);
		std::iota(row_idx.begin(), row_idx.end(), 0);

		// std::vector<std::size_t> row_idx_samples()

		// std::generate(row_idx.begin(), row_idx.end(), [n = 0]() mutable {
		// 	auto v = n;
		// 	n += 2;
		// 	return v;
		// });

		// struct IterElement {
		// 	Index node;
		// 	float distance;
		// 	T     value;

		// 	IterElement() = default;

		// 	IterElement(Index node, float distance, T value)
		// 	    : node(node), distance(distance), value(value)
		// 	{
		// 	}
		// };

		// auto tmp_miss = IterElement(TreeIndex{}, std::numeric_limits<float>::max(),
		// miss);

		// std::sample(rays.begin(), rays.end(), std::back_inserter(out), 4,
		//             std::mt19937{std::random_device{}()});

		// std::transform(
		//     policy, rays.begin(), rays.end(), image.begin(), [&, this](auto const& ray) {
		// 	    auto wrapped_inner_f = [inner_f, &ray](auto node, auto distance) {
		// 		    return inner_f(node, ray, distance);
		// 	    };

		// 	    auto wrapped_hit_f = [hit_f, &ray](auto node, auto distance) {
		// 		    return hit_f(node, ray, distance);
		// 		    // auto [hit, value] = hit_f(node, ray, distance);
		// 		    // return std::make_pair(hit, IterElement(node, distance, value));
		// 	    };

		// 	    auto params = Base::traceInit(ray, center, half_length);
		// 	    return Base::trace(n, params, wrapped_inner_f, wrapped_hit_f, miss);
		//     });

		static std::size_t s{};
		++s;

		std::for_each(policy, row_idx.begin(), row_idx.end(), [&](std::size_t row) {
			std::size_t offset = (row + s) % step_size;

			for (std::size_t col = offset; cols > col; col += step_size) {
				auto const& ray = rays(row, col);

				auto wrapped_inner_f = [inner_f, &ray](auto node, auto distance) {
					return inner_f(node, ray, distance);
				};

				auto wrapped_hit_f = [hit_f, &ray](auto node, auto distance) {
					return hit_f(node, ray, distance);
					// auto [hit, value] = hit_f(node, ray, distance);
					// return std::make_pair(hit, IterElement(node, distance, value));
				};

				auto params     = Base::traceInit(ray, center, half_length);
				image(row, col) = Base::trace(n, params, wrapped_inner_f, wrapped_hit_f, miss);
				// auto ie              = Base::trace(n, params, wrapped_inner_f, wrapped_hit_f,
				// tmp_miss); image(row, col)      = ie.value; iter_image(row, col) = {ie.node,
				// ie.distance};
			}
		});

		if constexpr (2 == step_size) {
			// std::for_each(
			//     policy, row_idx.begin() + 1, row_idx.end() - 1, [&](std::size_t row) {
			// 	    std::size_t const offset = 1 - (row % step_size);

			// 	    for (std::size_t col = step_size - offset; cols - 1 > col;
			// 	         col += step_size) {
			// 		    auto idx = minIndex(Vec4f(
			// 		        iter_image(row - 1, col).second, iter_image(row + 1, col).second,
			// 		        iter_image(row, col - 1).second, iter_image(row, col + 1).second));

			// 		    std::size_t min_row = 0 == idx ? row - 1 : (1 == idx ? row + 1 : row);
			// 		    std::size_t min_col = 2 == idx ? col - 1 : (3 == idx ? col + 1 : col);

			// 		    auto const& [node, distance] = iter_image(min_row, min_col);
			// 		    if (Base::valid(node)) {
			// 			    auto ray = rays(row, col);
			// 			    // float dist = distance(ray.origin, min(bounds));
			// 			    if (auto [hit, value] = hit_f(node, ray, distance); hit) {
			// 				    image(row, col) = value;
			// 			    } else {
			// 				    image(row, col) = miss;
			// 			    }
			// 		    } else {
			// 			    image(row, col) = miss;
			// 		    }
			// 	    }
			//     });
		} else {
			// std::for_each(
			//     policy, row_idx.begin() + 1, row_idx.end() - 1, [&](std::size_t row) {
			// 	    std::size_t const offset = row % step_size;

			// 	    for (std::size_t col = 1; cols - 1 > col; ++col) {
			// 		    if (offset == (col % step_size)) {
			// 			    continue;
			// 		    }

			// 		    auto idx = minIndex(Vec4f(
			// 		        iter_image(row - 1, col).second, iter_image(row + 1, col).second,
			// 		        iter_image(row, col - 1).second, iter_image(row, col + 1).second));

			// 		    std::size_t min_row = 0 == idx ? row - 1 : (1 == idx ? row + 1 : row);
			// 		    std::size_t min_col = 2 == idx ? col - 1 : (3 == idx ? col + 1 : col);

			// 		    auto const& [node, distance] = iter_image(min_row, min_col);
			// 		    if (Base::valid(node)) {
			// 			    auto ray = rays(row, col);
			// 			    // float dist = distance(ray.origin, min(bounds));
			// 			    if (auto [hit, value] = hit_f(node, ray, distance); hit) {
			// 				    image(row, col) = value;
			// 			    } else {
			// 				    image(row, col) = miss;
			// 			    }
			// 		    } else {
			// 			    image(row, col) = miss;
			// 		    }
			// 	    }
			//     });
		}
	}
}

template <class ExecutionPolicy, class NodeType, class InnerFun, class HitFun, class T>
[[nodiscard]] Image<T> render(ExecutionPolicy&& policy, NodeType node,
                              Camera const& camera, std::size_t rows, std::size_t cols,
                              InnerFun inner_f, HitFun hit_f, T const& miss)
{
	Image<T> image(rows, cols);
	render(policy, node, camera, image, inner_f, hit_f, miss);
	return image;
}

//
// Hextree
//

template <class InnerFun, class HitFun, class T>
void render(float w, Camera const& camera, Image<T>& image, InnerFun inner_f,
            HitFun hit_f, T const& miss)
{
	render(w, Base::index(), camera, image, inner_f, hit_f, miss);
}

template <class InnerFun, class HitFun, class T>
[[nodiscard]] Image<T> render(float w, Camera const& camera, std::size_t rows,
                              std::size_t cols, InnerFun inner_f, HitFun hit_f,
                              T const& miss)
{
	return render(Base::index(), w, camera, rows, cols, inner_f, hit_f, miss);
}

template <class NodeType, class InnerFun, class HitFun, class T,
          std::enable_if_t<Base::template is_node_type_v<NodeType>, bool> = true>
void render(NodeType node, float w, Camera const& camera, Image<T>& image,
            InnerFun inner_f, HitFun hit_f, T const& miss)
{
	Image<Ray3> rays = camera.rays(image.rows(), image.cols());
	Image<Ray4> rays4d(image.rows(), image.cols());
	for (std::size_t row{}; image.rows() > row; ++row) {
		for (std::size_t col{}; image.cols() > col; ++col) {
			rays4d(row, col) =
			    Ray4(Vec4f(rays(row, col).origin, w), Vec4f(rays(row, col).direction, 0));
		}
	}
	Base::trace(node, rays4d.begin(), rays4d.end(), image.begin(), inner_f, hit_f, miss);
}

template <class NodeType, class InnerFun, class HitFun, class T,
          std::enable_if_t<Base::template is_node_type_v<NodeType>, bool> = true>
[[nodiscard]] Image<T> render(NodeType node, float w, Camera const& camera,
                              std::size_t rows, std::size_t cols, InnerFun inner_f,
                              HitFun hit_f, T const& miss)
{
	Image<T> image(rows, cols);
	render(node, w, camera, image, inner_f, hit_f, miss);
	return image;
}

template <
    class ExecutionPolicy, class InnerFun, class HitFun, class T,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
void render(ExecutionPolicy&& policy, float w, Camera const& camera, Image<T>& image,
            InnerFun inner_f, HitFun hit_f, T const& miss)
{
	render(std::forward<ExecutionPolicy>(policy), Base::index(), w, camera, image, inner_f,
	       hit_f, miss);
}

template <
    class ExecutionPolicy, class InnerFun, class HitFun, class T,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
[[nodiscard]] Image<T> render(ExecutionPolicy&& policy, float w, Camera const& camera,
                              std::size_t rows, std::size_t cols, InnerFun inner_f,
                              HitFun hit_f, T const& miss)
{
	return render(std::forward<ExecutionPolicy>(policy), Base::index(), w, camera, rows,
	              cols, inner_f, hit_f, miss);
}

template <class ExecutionPolicy, class NodeType, class InnerFun, class HitFun, class T>
void render(ExecutionPolicy&& policy, NodeType node, float w, Camera const& camera,
            Image<T>& image, InnerFun inner_f, HitFun hit_f, T const& miss)
{
	auto rows = image.rows();
	auto cols = image.cols();

	Image<Ray3> rays = camera.rays(policy, rows, cols);
	Image<Ray4> rays4d(image.rows(), image.cols());
	for (std::size_t row{}; image.rows() > row; ++row) {
		for (std::size_t col{}; image.cols() > col; ++col) {
			rays4d(row, col) =
			    Ray4(Vec4f(rays(row, col).origin, w), Vec4f(rays(row, col).direction, 0));
		}
	}

	Base::trace(std::forward<ExecutionPolicy>(policy), node, rays4d.begin(), rays4d.end(),
	            image.begin(), inner_f, hit_f, miss);
}

template <class ExecutionPolicy, class NodeType, class InnerFun, class HitFun, class T>
[[nodiscard]] Image<T> render(ExecutionPolicy&& policy, NodeType node, float w,
                              Camera const& camera, std::size_t rows, std::size_t cols,
                              InnerFun inner_f, HitFun hit_f, T const& miss)
{
	Image<T> image(rows, cols);
	render(policy, node, w, camera, image, inner_f, hit_f, miss);
	return image;
}
#endif
}  // namespace ufo

#endif  // UFO_CONTAINER_TREE_RENDER_HPP