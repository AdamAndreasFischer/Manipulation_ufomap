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

#ifndef UFO_CONTAINER_TREE_QUERY_NEAREST_ITERATOR_HPP
#define UFO_CONTAINER_TREE_QUERY_NEAREST_ITERATOR_HPP

// UFO
#include <ufo/container/tree/index.hpp>
#include <ufo/container/tree/predicate/filter.hpp>
#include <ufo/container/tree/predicate/predicate.hpp>
#include <ufo/geometry/dynamic_geometry.hpp>

// STL
#include <cmath>
#include <cstddef>
#include <iterator>
#include <queue>

namespace ufo
{
template <class Tree, class Predicate = pred::Predicate<Tree>,
          class Geometry = DynamicGeometry>
class TreeQueryNearestIterator
{
 private:
	//
	// Friends
	//

	template <class, class, class>
	friend class TreeQueryNearestIterator;

 private:
	static constexpr std::size_t const BF = Tree::branchingFactor();

	using Node         = typename Tree::Node;
	using DistanceNode = typename Tree::DistanceNode;
	using offset_t     = typename Tree::offset_t;

	using Filter = pred::Filter<Predicate>;

	struct S {
		float dist_sq;
		Node  node;
		bool  returnable;

		S(float dist_sq, Node node, bool returnable) noexcept
		    : dist_sq(dist_sq), node(node), returnable(returnable)
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
	using value_type        = DistanceNode;
	using reference         = value_type const&;
	using pointer           = value_type const*;

	constexpr TreeQueryNearestIterator() = default;

	TreeQueryNearestIterator(Tree* t, Node const& node, Predicate const& pred,
	                         Geometry const& geometry, float epsilon, bool only_exists,
	                         bool early_stopping)
	    : t_(t)
	    , pred_(pred)
	    , query_(geometry)
	    , epsilon_sq_(epsilon * epsilon)
	    , only_exists_(only_exists)
	    , early_stopping_(early_stopping)
	{
		Filter::init(pred_, *t_);

		if (only_exists_ && !t_->exists(node)) {
			return;
		}

		float dist_sq = distanceSquared(query_, t_->bounds(node));
		if (returnable(node)) {
			queue_.emplace(dist_sq, node, true);

			if (!early_stopping_ && traversable(node)) {
				queue_.emplace(dist_sq + epsilon_sq_, node, false);
			}
		} else if (traversable(node)) {
			queue_.emplace(dist_sq + epsilon_sq_, node, false);
		}

		nextNode();
	}

	TreeQueryNearestIterator(TreeQueryNearestIterator const&) = default;

	template <class Predicate2, class Geometry2>
	TreeQueryNearestIterator(
	    TreeQueryNearestIterator<Tree, Predicate2, Geometry2> const& other)
	    : t_(other.t_)
	    , pred_(other.pred)
	    , query_(other.query_)
	    , epsilon_sq_(other.epsilon_sq_)
	    , queue_(other.queue_)
	    , ret_(other.ret_)
	    , only_exists_(other.only_exists_)
	    , early_stopping_(other.early_stopping_)
	{
	}

	TreeQueryNearestIterator& operator++()
	{
		queue_.pop();
		nextNode();
		return *this;
	}

	TreeQueryNearestIterator operator++(int)
	{
		TreeQueryNearestIterator tmp(*this);
		++*this;
		return tmp;
	}

	reference operator*() const { return ret_; }

	pointer operator->() const { return &ret_; }

	template <class Predicate2, class Geometry2>
	bool operator==(TreeQueryNearestIterator<Tree, Predicate2, Geometry2> const& other)
	{
		return ret_ == other.ret_;
	}

	template <class Predicate2, class Geometry2>
	bool operator!=(TreeQueryNearestIterator<Tree, Predicate2, Geometry2> const& other)
	{
		return !(*this == other);
	}

 private:
	[[nodiscard]] bool returnable(Node const& node) const
	{
		return Filter::returnable(pred_, *t_, node);
	}

	[[nodiscard]] bool returnable(S const& s) const { return s.returnable; }

	[[nodiscard]] bool traversable(Node const& node) const
	{
		return (t_->isParent(node.index) || (!only_exists_ && !t_->isPureLeaf(node.code))) &&
		       Filter::traversable(pred_, *t_, node);
	}

	[[nodiscard]] bool exists(Node const& node) const
	{
		return only_exists_ || t_->code(node.index) == node.code;
	}

	[[nodiscard]] Node sibling(Node const& node, offset_t sibling_index) const
	{
		return Node(t_->sibling(node.code, sibling_index),
		            exists(node) ? t_->sibling(node.index, sibling_index) : node.index);
	}

	[[nodiscard]] Node child(Node const& node, offset_t child_index) const
	{
		return Node(
		    t_->child(node.code, child_index),
		    t_->isParent(node.index) ? t_->child(node.index, child_index) : node.index);
	}

	[[nodiscard]] Node parent(Node const& node) const
	{
		return Node(t_->parent(node.code),
		            exists(node) ? t_->parent(node.index) : node.index);
	}

	[[nodiscard]] offset_t offset(Node const& node) const { return node.code.offset(); }

	void nextNode()
	{
		while (!queue_.empty()) {
			S cur = queue_.top();
			if (returnable(cur)) {
				ret_ = DistanceNode(cur.node, std::sqrt(cur.dist_sq));
				return;
			}

			queue_.pop();

			Node node = child(cur.node, 0);
			for (offset_t i{}; BF > i; ++i) {
				node = sibling(node, i);

				float dist_sq = distanceSquared(query_, t_->bounds(node));
				if (returnable(node)) {
					queue_.emplace(dist_sq, node, true);

					if (!early_stopping_ && traversable(node)) {
						queue_.emplace(dist_sq + epsilon_sq_, node, false);
					}
				} else if (traversable(node)) {
					queue_.emplace(dist_sq + epsilon_sq_, node, false);
				}
			}
		}
	}

 private:
	Tree* t_ = nullptr;

	Predicate pred_;

	Geometry query_;
	float    epsilon_sq_;

	Queue        queue_;
	DistanceNode ret_{};

	bool only_exists_{};
	bool early_stopping_{};
};
}  // namespace ufo

#endif  // UFO_CONTAINER_TREE_QUERY_NEAREST_ITERATOR_HPP