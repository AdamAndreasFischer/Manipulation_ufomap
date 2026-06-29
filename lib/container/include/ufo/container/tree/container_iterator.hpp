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

#ifndef UFO_CONTAINER_TREE_CONTAINER_ITERATOR_HPP
#define UFO_CONTAINER_TREE_CONTAINER_ITERATOR_HPP

// STL
#include <iterator>
#include <type_traits>

namespace ufo
{
// Forward declare
template <class... Ts>
class TreeContainer;

template <class T, bool Const, class... Ts>
class TreeContainterIterator
{
 private:
	//
	// Friends
	//

	friend class TreeContainterIterator<T, !Const, Ts...>;

	friend class TreeContainer<Ts...>;

 public:
	//
	// Tags
	//

	using iterator_category = std::random_access_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = T;
	using pointer           = std::conditional_t<Const, T const*, T*>;
	using reference         = std::conditional_t<Const, T const&, T&>;

	TreeContainterIterator() = default;

	TreeContainterIterator(TreeContainterIterator const&) = default;

	template <bool Const2, class = std::enable_if_t<Const && !Const2>>
	TreeContainterIterator(TreeContainterIterator<T, Const2, Ts...> const& other)
	    : container_(other.container_), idx_(other.idx_)
	{
	}

	TreeContainterIterator& operator++()
	{
		++idx_;
		return *this;
	}

	TreeContainterIterator& operator--()
	{
		--idx_;
		return *this;
	}

	TreeContainterIterator operator++(int)
	{
		TreeContainterIterator tmp(*this);
		++*this;
		return tmp;
	}

	TreeContainterIterator operator--(int)
	{
		TreeContainterIterator tmp(*this);
		--*this;
		return tmp;
	}

	TreeContainterIterator& operator+=(difference_type n)
	{
		idx_ += n;
		return *this;
	}

	TreeContainterIterator& operator-=(difference_type n)
	{
		idx_ -= n;
		return *this;
	}

	TreeContainterIterator operator+(difference_type n)
	{
		TreeContainterIterator tmp(*this);
		tmp += n;
		return tmp;
	}

	TreeContainterIterator operator-(difference_type n)
	{
		TreeContainterIterator tmp(*this);
		tmp -= n;
		return tmp;
	}

	reference operator[](difference_type pos) const
	{
		return container_->template get<T>(idx_ + pos);
	}

	reference operator*() const { return container_->template get<T>(idx_); }

	pointer operator->() const { return &(container_->template get<T>(idx_)); }

	template <bool Const1, bool Const2>
	friend difference_type operator-(TreeContainterIterator<T, Const1, Ts...> const& lhs,
	                                 TreeContainterIterator<T, Const2, Ts...> const& rhs)
	{
		return lhs.idx_ - rhs.idx_;
	}

	template <bool Const1, bool Const2>
	friend bool operator==(TreeContainterIterator<T, Const1, Ts...> const& lhs,
	                       TreeContainterIterator<T, Const2, Ts...> const& rhs)
	{
		return lhs.idx_ == rhs.idx_;
	}

	template <bool Const1, bool Const2>
	friend bool operator!=(TreeContainterIterator<T, Const1, Ts...> const& lhs,
	                       TreeContainterIterator<T, Const2, Ts...> const& rhs)
	{
		return lhs.idx_ != rhs.idx_;
	}

	template <bool Const1, bool Const2>
	friend bool operator<(TreeContainterIterator<T, Const1, Ts...> const& lhs,
	                      TreeContainterIterator<T, Const2, Ts...> const& rhs)
	{
		return lhs.idx_ < rhs.idx_;
	}

	template <bool Const1, bool Const2>
	friend bool operator<=(TreeContainterIterator<T, Const1, Ts...> const& lhs,
	                       TreeContainterIterator<T, Const2, Ts...> const& rhs)
	{
		return lhs.idx_ <= rhs.idx_;
	}

	template <bool Const1, bool Const2>
	friend bool operator>(TreeContainterIterator<T, Const1, Ts...> const& lhs,
	                      TreeContainterIterator<T, Const2, Ts...> const& rhs)
	{
		return lhs.idx_ > rhs.idx_;
	}

	template <bool Const1, bool Const2>
	friend bool operator>=(TreeContainterIterator<T, Const1, Ts...> const& lhs,
	                       TreeContainterIterator<T, Const2, Ts...> const& rhs)
	{
		return lhs.idx_ >= rhs.idx_;
	}

 protected:
	TreeContainterIterator(TreeContainer<Ts...>* container, difference_type idx)
	    : container_(container), idx_(idx)
	{
	}

	TreeContainterIterator(TreeContainer<Ts...>& container, difference_type idx)
	    : TreeContainterIterator(&container, idx)
	{
	}

 private:
	TreeContainer<Ts...>* container_ = nullptr;

	difference_type idx_;
};
}  // namespace ufo

#endif  // UFO_CONTAINER_TREE_CONTAINER_ITERATOR_HPP