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

#ifndef UFO_UTILITY_TUPLE_ITERATOR_HPP
#define UFO_UTILITY_TUPLE_ITERATOR_HPP

// STL
#include <iterator>
#include <tuple>

namespace ufo
{
template <class... Ts>
class TupleIterator
{
 public:
	// Tags
	using value_type      = std::tuple<typename Ts::value_type...>;
	using size_type       = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference       = std::tuple<typename Ts::value_type&...>;
	using const_reference = std::tuple<typename Ts::value_type const&...>;

	template <bool Const>
	class Iterator
	{
	 public:
		// Tags
		using iterator_category = std::random_access_iterator_tag;
		using difference_type   = std::ptrdiff_t;
		using value_type        = typename TupleIterator::value_type;
		using pointer           = void;  // TODO: What should this be?
		using reference = std::conditional_t<Const, typename TupleIterator::const_reference,
		                                     typename TupleIterator::reference>;

	 private:
		using data_ptr =
		    std::conditional_t<Const, std::tuple<Ts&...> const*, std::tuple<Ts&...>*>;

	 public:
		constexpr explicit Iterator(data_ptr data, std::size_t idx) : data_(data), idx_(idx)
		{
		}

		constexpr Iterator& operator++()
		{
			++idx_;
			return *this;
		}

		constexpr Iterator& operator--()
		{
			--idx_;
			return *this;
		}

		constexpr Iterator operator++(int)
		{
			Iterator tmp(*this);
			++idx_;
			return tmp;
		}

		constexpr Iterator operator--(int)
		{
			Iterator tmp(*this);
			--idx_;
			return tmp;
		}

		constexpr Iterator operator+(difference_type n)
		{
			Iterator tmp(*this);
			tmp.idx_ += n;
			return tmp;
		}

		constexpr Iterator operator-(difference_type n)
		{
			Iterator tmp(*this);
			tmp.idx_ -= n;
			return tmp;
		}

		constexpr Iterator& operator+=(difference_type n)
		{
			idx_ += n;
			return *this;
		}

		constexpr Iterator& operator-=(difference_type n)
		{
			idx_ -= n;
			return *this;
		}

		[[nodiscard]] constexpr reference operator[](difference_type pos) const
		{
			if constexpr (Const) {
				return std::apply(
				    [i = idx_ + pos](Ts const&... ts) { return std::tie(ts[i]...); }, *data_);
			} else {
				return std::apply([i = idx_ + pos](Ts&... ts) { return std::tie(ts[i]...); },
				                  *data_);
			}
		}

		[[nodiscard]] constexpr reference operator*() const { return operator[](0); }

		constexpr difference_type operator-(Iterator const& rhs) const
		{
			return idx_ - rhs.idx_;
		}

		[[nodiscard]] constexpr bool operator==(Iterator other) const
		{
			return idx_ == other.idx_ && data_ == other.data_;
		}

		[[nodiscard]] constexpr bool operator!=(Iterator other) const
		{
			return !(*this == other);
		}

		[[nodiscard]] constexpr bool operator<(Iterator other) const
		{
			return idx_ < other.idx_;
		}

		[[nodiscard]] constexpr bool operator<=(Iterator other) const
		{
			return idx_ <= other.idx_;
		}

		[[nodiscard]] constexpr bool operator>(Iterator other) const
		{
			return idx_ > other.idx_;
		}

		[[nodiscard]] constexpr bool operator>=(Iterator other) const
		{
			return idx_ >= other.idx_;
		}

	 private:
		data_ptr    data_ = nullptr;
		std::size_t idx_  = 0;
	};

	using iterator               = Iterator<false>;
	using const_iterator         = Iterator<true>;
	using reverse_iterator       = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	TupleIterator(Ts&... ts) : ts_(ts...)
	{
		assert(((ts.size() == std::get<0>(ts_).size()) && ...));
	}

	[[nodiscard]] iterator begin() { return iterator(&ts_, 0); }

	[[nodiscard]] const_iterator begin() const { return const_iterator(&ts_, 0); }

	[[nodiscard]] const_iterator cbegin() const { return begin(); }

	[[nodiscard]] iterator end() { return iterator(&ts_, std::get<0>(ts_).size()); }

	[[nodiscard]] const_iterator end() const
	{
		return const_iterator(&ts_, std::get<0>(ts_).size());
	}

	[[nodiscard]] const_iterator cend() const { return end(); }

	[[nodiscard]] reverse_iterator rbegin() { return std::reverse_iterator(end()); }

	[[nodiscard]] const_reverse_iterator rbegin() const
	{
		return std::reverse_iterator(end());
	}

	[[nodiscard]] const_reverse_iterator crbegin() const { return rbegin(); }

	[[nodiscard]] reverse_iterator rend() { return std::reverse_iterator(begin()); }

	[[nodiscard]] const_reverse_iterator rend() const
	{
		return std::reverse_iterator(begin());
	}

	[[nodiscard]] const_reverse_iterator crend() const { return rend(); }

	[[nodiscard]] auto operator[](std::size_t pos)
	{
		return std::apply([i = pos](Ts&... ts) { return std::tie(ts[i]...); }, *ts_);
	}

	[[nodiscard]] auto operator[](std::size_t pos) const
	{
		return std::apply([i = pos](Ts const&... ts) { return std::tie(ts[i]...); }, *ts_);
	}

 private:
	std::tuple<Ts&...> ts_;
};
}  // namespace ufo

#endif  // UFO_UTILITY_TUPLE_ITERATOR_HPP
