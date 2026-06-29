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

#ifndef UFO_UTILITY_INDEX_ITERATOR_HPP
#define UFO_UTILITY_INDEX_ITERATOR_HPP

// UFO
#include <ufo/utility/proxy_arrow_result.hpp>

// STL
#include <iterator>

namespace ufo
{
template <class T = std::size_t>
struct IndexIterator {
	T first{};
	T last{};
	T inc = T(1);

	class Iterator
	{
		friend class IndexIterator<T>;

	 public:
		// Tags
		using iterator_category = std::random_access_iterator_tag;
		using difference_type   = std::ptrdiff_t;
		using value_type        = T;
		using reference         = value_type;  // TODO: WTF
		using pointer           = value_type*;
		// using pointer = proxy_arrow_result<reference>;  // TODO: What should this be?

	 public:
		constexpr Iterator() = default;

		constexpr Iterator& operator++()
		{
			pos += inc;
			return *this;
		}

		constexpr Iterator operator++(int)
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		constexpr Iterator& operator--()
		{
			pos -= inc;
			return *this;
		}

		constexpr Iterator operator--(int)
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}

		constexpr Iterator& operator+=(difference_type n)
		{
			pos += n * inc;
			return *this;
		}

		constexpr Iterator operator+(difference_type n)
		{
			auto tmp = *this;
			tmp += n;
			return tmp;
		}

		constexpr Iterator& operator-=(difference_type n)
		{
			pos -= n * inc;
			return *this;
		}

		constexpr Iterator operator-(difference_type n)
		{
			auto tmp = *this;
			tmp -= n;
			return tmp;
		}

		constexpr reference operator[](difference_type pos) const
		{
			return this->pos + pos * inc;
		}

		[[nodiscard]] constexpr reference operator*() const { return operator[](0); }

		// [[nodiscard]] constexpr pointer operator->() const { return **this; }

		friend constexpr difference_type operator-(Iterator const& lhs, Iterator const& rhs)
		{
			return lhs.inc * (lhs.pos - rhs.pos);
		}

		friend constexpr bool operator==(Iterator const& lhs, Iterator const& rhs)
		{
			return lhs.pos == rhs.pos;
		}

		friend constexpr bool operator!=(Iterator const& lhs, Iterator const& rhs)
		{
			return !(lhs == rhs);
		}

		friend constexpr bool operator<(Iterator const& lhs, Iterator const& rhs)
		{
			return lhs.pos < rhs.pos;
		}

		friend constexpr bool operator<=(Iterator const& lhs, Iterator const& rhs)
		{
			return lhs.pos <= rhs.pos;
		}

		friend constexpr bool operator>(Iterator const& lhs, Iterator const& rhs)
		{
			return lhs.pos > rhs.pos;
		}

		friend constexpr bool operator>=(Iterator const& lhs, Iterator const& rhs)
		{
			return lhs.pos >= rhs.pos;
		}

	 private:
		constexpr Iterator(T pos, T inc = T(0)) : pos(pos), inc(inc) {}

		T pos{};
		T inc{};
	};

	constexpr IndexIterator() = default;

	constexpr IndexIterator(T first, T last, T inc = T(1))
	    : first(first), last(last), inc(inc)
	{
		assert(T(0) != inc);
	}

	[[nodiscard]] Iterator begin() { return cbegin(); }

	[[nodiscard]] Iterator begin() const { return cbegin(); }

	[[nodiscard]] Iterator cbegin() const { return Iterator(first, inc); }

	[[nodiscard]] Iterator end() { return cend(); }

	[[nodiscard]] Iterator end() const { return cend(); }

	[[nodiscard]] Iterator cend() const { return Iterator(last, inc); }
};
}  // namespace ufo

#endif  // UFO_UTILITY_INDEX_ITERATOR_HPP