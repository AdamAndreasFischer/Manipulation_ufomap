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

#ifndef UFO_CONTAINER_DETAIL_TREE_SET_NEAREST_ITERATOR_HPP
#define UFO_CONTAINER_DETAIL_TREE_SET_NEAREST_ITERATOR_HPP

// UFO
#include <ufo/container/tree/index.hpp>
#include <ufo/geometry/distance.hpp>
#include <ufo/geometry/dynamic_geometry.hpp>

// STL
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <queue>
#include <type_traits>
#include <utility>

namespace ufo
{
// Forward declare
template <std::size_t Dim>
class TreeSet;

template <bool Const, std::size_t Dim, class Geometry = DynamicGeometry>
class TreeSetNearestIterator
{
 private:
	//
	// Friends
	//

	template <bool, std::size_t, class>
	friend class TreeSetNearestIterator;

	friend class TreeSet<Dim>;

 private:
	static constexpr std::size_t const BF = TreeSet<Dim>::branchingFactor();

	using RawIterator =
	    std::conditional_t<Const, typename TreeSet<Dim>::container_type::const_iterator,
	                       typename TreeSet<Dim>::container_type::iterator>;

	using Point = typename TreeSet<Dim>::Point;

	struct S {
		float       dist_sq;
		TreeIndex   node;
		RawIterator it;

		S(float dist_sq, TreeIndex node, RawIterator it = {}) noexcept
		    : dist_sq(dist_sq), node(node), it(it)
		{
		}

		bool operator>(S const& rhs) const noexcept { return dist_sq > rhs.dist_sq; }
	};

	using Queue = std::priority_queue<S, std::vector<S>, std::greater<S>>;

 public:
	//
	// Tags
	//

	using iterator_category = std::forward_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type =
	    std::pair<typename std::iterator_traits<RawIterator>::value_type, float>;
	using reference = value_type const&;
	using pointer   = value_type const*;

	TreeSetNearestIterator() = default;

	TreeSetNearestIterator(TreeSetNearestIterator const&) = default;

	// From non-const to const or change of geometry type
	template <bool Const2, class Geometry2,
	          std::enable_if_t<(Const && !Const2) || (Const == Const2 &&
	                                                  !std::is_same_v<Geometry, Geometry2>),
	                           bool> = true>
	TreeSetNearestIterator(TreeSetNearestIterator<Const2, Dim, Geometry2> const& other)
	    : ts_(other.ts_)
	    , query_(other.query_)
	    , epsilon_sq_(other.epsilon_sq_)
	    , ret_(other.ret_)
	{
		auto queue = other.queue_;
		while (!queue.empty()) {
			queue_.emplace(queue.top().dist_sq, queue.top().node, queue.top().it);
			queue.pop();
		}
	}

	TreeSetNearestIterator& operator++()
	{
		queue_.pop();
		next();
		return *this;
	}

	TreeSetNearestIterator operator++(int)
	{
		TreeSetNearestIterator tmp(*this);
		++*this;
		return tmp;
	}

	reference operator*() const { return ret_; }

	pointer operator->() const { return &ret_; }

	template <bool Const2, class Geometry2>
	bool operator==(TreeSetNearestIterator<Const2, Dim, Geometry2> const& other)
	{
		return queue_.empty() == other.queue_.empty() &&
		       (queue_.empty() || queue_.top().it == other.queue_.top().it);
	}

	template <bool Const2, class Geometry2>
	bool operator!=(TreeSetNearestIterator<Const2, Dim, Geometry2> const& other)
	{
		return !(*this == other);
	}

 private:
	[[nodiscard]] bool returnable(TreeIndex node) const
	{
		return ts_->isPureLeaf(node) && !ts_->empty(node);
	}

	[[nodiscard]] bool returnable(S const& s) const { return RawIterator{} != s.it; }

	[[nodiscard]] bool traversable(TreeIndex node) const { return ts_->isParent(node); }

	void next()
	{
		while (!queue_.empty()) {
			S cur = queue_.top();
			if (returnable(cur)) {
				ret_.first  = *cur.it;
				ret_.second = std::sqrt(cur.dist_sq);
				return;
			}

			queue_.pop();

			if (returnable(cur.node)) {
				auto& v = ts_->values(cur.node);
				using std::begin;
				using std::end;
				for (auto it = begin(v), last = end(v); it != last; ++it) {
					Point p       = *it;
					float dist_sq = distanceSquared(query_, p);
					queue_.emplace(dist_sq, cur.node, it);
				}
				continue;
			}

			TreeIndex node = ts_->child(cur.node, 0);
			for (; BF > node.offset; ++node.offset) {
				if (!traversable(node) && !returnable(node)) {
					continue;
				}

				float dist_sq = distanceSquared(query_, ts_->bounds(node)) + epsilon_sq_;
				queue_.emplace(dist_sq, node);
			}
		}
	}

	[[nodiscard]] RawIterator iterator() { return queue_.top().it; }

 private:
	TreeSetNearestIterator(TreeSet<Dim>* ts, TreeIndex node, Geometry const& query,
	                       float epsilon = 0.0f)
	    : ts_(ts), query_(query), epsilon_sq_(epsilon * epsilon)
	{
		if (traversable(node) || returnable(node)) {
			float dist_sq = distanceSquared(query_, ts_->bounds(node));
			queue_.emplace(dist_sq, node);
			next();
		}
	}

	// From const to non-const
	template <bool Const2, class Geometry2, std::enable_if_t<!Const && Const2, bool> = true>
	TreeSetNearestIterator(TreeSetNearestIterator<Const2, Dim, Geometry2> const& other)
	    : ts_(other.ts)
	    , query_(other.query_)
	    , epsilon_sq_(other.epsilon_sq_)
	    , ret_(other.ret_)
	{
		auto queue = other.queue_;
		while (!queue.empty()) {
			auto const& cur = queue.top();
			// Remove const from it
			queue_.emplace(cur.dist_sq, cur.node, ts_->values(cur.node).erase(cur.it, cur.it));
			queue.pop();
		}
	}

 private:
	TreeSet<Dim>* ts_ = nullptr;

	Geometry query_;
	float    epsilon_sq_;

	Queue      queue_;
	value_type ret_;
};
}  // namespace ufo

#endif  // UFO_CONTAINER_DETAIL_TREE_SET_NEAREST_ITERATOR_HPP