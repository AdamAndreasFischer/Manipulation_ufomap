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

#ifndef UFO_CONTAINER_TREE_QUERY_ITERATOR_HPP
#define UFO_CONTAINER_TREE_QUERY_ITERATOR_HPP

// UFO
#include <ufo/container/tree/index.hpp>
#include <ufo/container/tree/predicate/filter.hpp>
#include <ufo/container/tree/predicate/predicate.hpp>

// STL
#include <cstddef>
#include <iterator>

namespace ufo
{
template <class Tree, class Predicate = pred::Predicate<Tree>>
class TreeQueryIterator
{
 private:
	//
	// Friends
	//

	template <class, class>
	friend class TreeQueryIterator;

 private:
	static constexpr std::size_t const BF = Tree::branchingFactor();

	using Node     = typename Tree::Node;
	using offset_t = typename Tree::offset_t;

	using Filter = pred::Filter<Predicate>;

 public:
	//
	// Tags
	//

	using iterator_category = std::forward_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = Node;
	using reference         = value_type const&;
	using pointer           = value_type const*;

	constexpr TreeQueryIterator() = default;

	TreeQueryIterator(Tree* t, Node const& node, Predicate const& pred, bool only_exists,
	                  bool early_stopping)
	    : t_(t)
	    , pred_(pred)
	    , root_(node)
	    , cur_(node)
	    , only_exists_(only_exists)
	    , early_stopping_(early_stopping)
	{
		Filter::init(pred_, *t_);

		if (only_exists_ && !t_->exists(root_)) {
			root_ = {};
			cur_  = {};
			return;
		}

		if (returnable(cur_)) {
			return;
		} else if (traversable(cur_)) {
			if (nextNodeDownwards()) {
				return;
			}
		}

		nextNode();
	}

	TreeQueryIterator(TreeQueryIterator const&) = default;

	template <class Predicate2>
	TreeQueryIterator(TreeQueryIterator<Tree, Predicate2> const& other)
	    : t_(other.t_)
	    , pred_(other.pred_)
	    , root_(other.root_)
	    , cur_(other.cur_)
	    , only_exists_(other.only_exists_)
	    , early_stopping_(other.early_stopping_)
	{
	}

	TreeQueryIterator& operator++()
	{
		if (!only_exists_) {
			auto min_depth = t_->depth(cur_.code);
			auto depth     = t_->depth(cur_.index);
			while (min_depth < depth && t_->isParent(cur_.index)) {
				cur_.index = t_->child(cur_.index, cur_.code.offset(--depth));
			}
		}
		nextNode();
		return *this;
	}

	TreeQueryIterator operator++(int)
	{
		TreeQueryIterator tmp(*this);
		++*this;
		return tmp;
	}

	reference operator*() const { return cur_; }

	pointer operator->() const { return &cur_; }

	template <class Predicate2>
	bool operator==(TreeQueryIterator<Tree, Predicate2> const& other)
	{
		return cur_ == other.cur_;
	}

	template <class Predicate2>
	bool operator!=(TreeQueryIterator<Tree, Predicate2> const& other)
	{
		return !(*this == other);
	}

 private:
	[[nodiscard]] bool returnable(Node const& node) const
	{
		return Filter::returnable(pred_, *t_, node);
	}

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
		if (!early_stopping_ && traversable(cur_)) {
			cur_ = child(cur_, 0);
			if (nextNodeDownwards()) {
				return;
			}
		}

		while (root_ != cur_) {
			auto branch = offset(cur_);
			if (BF - 1 == branch) {
				cur_ = parent(cur_);
				continue;
			}

			cur_ = sibling(cur_, branch + 1);

			if (returnable(cur_)) {
				return;
			}

			if (traversable(cur_)) {
				cur_ = child(cur_, 0);
				if (nextNodeDownwards()) {
					return;
				}
			}
		}

		// We have visited all nodes
		root_ = {};
		cur_  = {};
	}

	/*!
	 * @brief
	 *
	 * @return true if a new node was found, false otherwise.
	 */
	bool nextNodeDownwards()
	{
		while (true) {
			if (returnable(cur_)) {
				return true;
			} else if (traversable(cur_)) {
				cur_ = child(cur_, 0);
			} else {
				break;
			}
		}
		return false;
	}

 private:
	Tree* t_ = nullptr;

	Predicate pred_{};

	Node root_{};
	Node cur_{};

	bool only_exists_{};
	bool early_stopping_{};
};
}  // namespace ufo

#endif  // UFO_CONTAINER_TREE_QUERY_ITERATOR_HPP