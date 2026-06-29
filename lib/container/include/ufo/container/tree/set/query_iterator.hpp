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

#ifndef UFO_CONTAINER_TREE_SET_QUERY_ITERATOR_HPP
#define UFO_CONTAINER_TREE_SET_QUERY_ITERATOR_HPP

// UFO
#include <ufo/container/tree/index.hpp>
#include <ufo/container/tree/predicate/filter.hpp>
#include <ufo/container/tree/predicate/predicate.hpp>

// STL
#include <cassert>
#include <cstddef>
#include <iterator>

namespace ufo
{
// Forward declare
template <std::size_t Dim>
class TreeSet;

template <bool Const, std::size_t Dim, class Predicate = pred::Predicate<TreeSet<Dim>>>
class TreeSetQueryIterator
{
 private:
	//
	// Friends
	//

	template <bool, std::size_t, class>
	friend class TreeSetQueryIterator;

	friend class TreeSet<Dim>;

 private:
	static constexpr std::size_t const BF = TreeSet<Dim>::branchingFactor();

	using RawIterator =
	    std::conditional_t<Const, typename TreeSet<Dim>::container_type::const_iterator,
	                       typename TreeSet<Dim>::container_type::iterator>;

	using Filter = pred::Filter<Predicate>;

 public:
	//
	// Tags
	//

	using iterator_category = std::forward_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = typename std::iterator_traits<RawIterator>::value_type;
	using reference         = typename std::iterator_traits<RawIterator>::reference;
	using pointer           = typename std::iterator_traits<RawIterator>::pointer;

	TreeSetQueryIterator() = default;

	TreeSetQueryIterator(TreeSetQueryIterator const& other)
	    : ts_(other.ts_)
	    , pred_(other.pred_)
	    , root_(other.root_)
	    , cur_(other.cur_)
	    , it_(other.it_)
	    , last_(other.last_)
	{
	}

	// From non-const to const or change of predicate type
	template <
	    bool Const2, class Predicate2,
	    std::enable_if_t<(Const && !Const2) ||
	                         (Const == Const2 && !std::is_same_v<Predicate, Predicate2>),
	                     bool> = true>
	TreeSetQueryIterator(TreeSetQueryIterator<Const2, Dim, Predicate2> const& other)
	    : ts_(other.ts_)
	    , pred_(other.pred_)
	    , root_(other.root_)
	    , cur_(other.cur_)
	    , it_(other.it_)
	    , last_(other.last_)
	{
	}

	TreeSetQueryIterator& operator++()
	{
		++it_;
		while (!nextValue()) {
			if (!nextNode()) {
				break;
			}
		}
		return *this;
	}

	TreeSetQueryIterator operator++(int)
	{
		TreeSetQueryIterator tmp(*this);
		++*this;
		return tmp;
	}

	reference operator*() const { return *it_; }

	pointer operator->() const { return &*it_; }

	template <bool Const2, class Predicate2>
	bool operator==(TreeSetQueryIterator<Const2, Dim, Predicate2> const& other)
	{
		return it_ == other.it_;
	}

	template <bool Const2, class Predicate2>
	bool operator!=(TreeSetQueryIterator<Const2, Dim, Predicate2> const& other)
	{
		return !(*this == other);
	}

 private:
	[[nodiscard]] bool returnable(value_type const& value) const
	{
		return Filter::returnable(pred_, value);
	}

	[[nodiscard]] bool returnable(TreeIndex node) const
	{
		return ts_->isPureLeaf(node) && !ts_->empty(node);
		// FIXME: && Filter::returnable(pred_, *ts_, ts_->node(node));
		// FIXME: && Filter::traversable(pred_, *ts_, ts_->node(node));
	}

	[[nodiscard]] bool traversable(TreeIndex node) const
	{
		return ts_->isParent(node) && Filter::traversable(pred_, *ts_, ts_->node(node));
	}

	/*!
	 * @brief
	 *
	 * @return true if a new value was found, false otherwise.
	 */
	bool nextValue()
	{
		it_ = std::find_if(it_, last_, [this](auto const& v) { return returnable(v); });
		return it_ != last_;
	}

	/*!
	 * @brief
	 *
	 * @return true if a new node was found, false otherwise.
	 */
	bool nextNode()
	{
		while (root_ != cur_) {
			if (BF - 1 == cur_.offset) {
				cur_ = ts_->parent(cur_);
				continue;
			}

			++cur_.offset;

			if (nextNodeDownwards()) {
				return true;
			}
		}

		// We have visited all nodes
		it_   = {};
		last_ = {};
		return false;
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
				it_   = ts_->values(cur_).begin();
				last_ = ts_->values(cur_).end();
				return true;
			} else if (traversable(cur_)) {
				cur_ = ts_->child(cur_, 0);
			} else {
				break;
			}
		}
		return false;
	}

	[[nodiscard]] RawIterator iterator() { return it_; }

 private:
	TreeSetQueryIterator(TreeSet<Dim>* ts, TreeIndex node, Predicate const& pred)
	    : ts_(ts), pred_(pred), root_(node), cur_(node)
	{
		Filter::init(pred_, *ts_);

		nextNodeDownwards();
		while (!nextValue()) {
			if (!nextNode()) {
				break;
			}
		}
	}

	// From const to non-const
	template <bool Const2, class Predicate2,
	          std::enable_if_t<!Const && Const2, bool> = true>
	TreeSetQueryIterator(TreeSetQueryIterator<Const2, Dim, Predicate2> const& other)
	    : ts_(other.ts_), pred_(other.pred_), root_(other.root_), cur_(other.cur_)
	{
		// Remove const from other.it_ and other.last_
		it_   = ts_->values(cur_).erase(other.it_, other.it_);
		last_ = ts_->values(cur_).erase(other.last_, other.last_);
	}

 private:
	TreeSet<Dim>* ts_ = nullptr;

	Predicate pred_{};

	TreeIndex root_{};
	TreeIndex cur_{};

	RawIterator it_{};
	RawIterator last_{};
};
}  // namespace ufo

#endif  // UFO_CONTAINER_TREE_SET_QUERY_ITERATOR_HPP