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

#ifndef UFO_CONTAINER_TREE_PREDICATE_COORD_HPP
#define UFO_CONTAINER_TREE_PREDICATE_COORD_HPP

// UFO
#include <ufo/container/tree/predicate/filter.hpp>
#include <ufo/container/tree/predicate/predicate_compare.hpp>

namespace ufo::pred
{
template <std::size_t Axis, PredicateCompare PC = PredicateCompare::EQUAL>
struct Coord {
	float coord;

	constexpr Coord(float coord = 0.0f) noexcept : coord(coord) {}
};

template <PredicateCompare PC = PredicateCompare::EQUAL>
struct X : Coord<0, PC> {
	constexpr X(float x = 0.0f) : Coord<0, PC>(x) {}
};

template <PredicateCompare PC = PredicateCompare::EQUAL>
struct Y : Coord<1, PC> {
	constexpr Y(float y = 0.0f) : Coord<1, PC>(y) {}
};

template <PredicateCompare PC = PredicateCompare::EQUAL>
struct Z : Coord<2, PC> {
	constexpr Z(float z = 0.0f) : Coord<2, PC>(z) {}
};

template <PredicateCompare PC = PredicateCompare::EQUAL>
struct W : Coord<3, PC> {
	constexpr W(float w = 0.0f) : Coord<3, PC>(w) {}
};

template <std::size_t Axis, PredicateCompare PC>
struct Filter<Coord<Axis, PC>> : public FilterBase<Coord<Axis, PC>> {
	using Pred = Coord<Axis, PC>;

	template <class Tree>
	static constexpr void init(Pred&, Tree const&)
	{
	}

	template <class Tree>
	[[nodiscard]] static constexpr bool returnable(Pred const& p, Tree const& t,
	                                               typename Tree::Node const& n)
	{
		auto c  = t.centerAxis(n, Axis);
		auto hl = t.halfLength(n)[Axis];

		if constexpr (PredicateCompare::EQUAL == PC) {
			return c - hl <= p.coord && c + hl >= p.coord;
		} else if constexpr (PredicateCompare::NOT_EQUAL == PC) {
			return c - hl > p.coord || c + hl < p.coord;
		} else if constexpr (PredicateCompare::LESS_EQUAL == PC) {
			return c - hl <= p.coord;
		} else if constexpr (PredicateCompare::GREATER_EQUAL == PC) {
			return c + hl >= p.coord;
		} else if constexpr (PredicateCompare::LESS == PC) {
			return c + hl < p.coord;
		} else if constexpr (PredicateCompare::GREATER == PC) {
			return c - hl > p.coord;
		}
	}

	template <class Tree>
	[[nodiscard]] static constexpr bool traversable(Pred const& p, Tree const& t,
	                                                typename Tree::Node const& n)
	{
		auto c  = t.centerAxis(n, Axis);
		auto hl = t.halfLength(n)[Axis];

		if constexpr (PredicateCompare::EQUAL == PC) {
			return c - hl <= p.coord && c + hl >= p.coord;
		} else if constexpr (PredicateCompare::NOT_EQUAL == PC) {
			return true;
		} else if constexpr (PredicateCompare::LESS_EQUAL == PC) {
			return c - hl <= p.coord;
		} else if constexpr (PredicateCompare::GREATER_EQUAL == PC) {
			return c + hl >= p.coord;
		} else if constexpr (PredicateCompare::LESS == PC) {
			return c - hl + t.length(0)[Axis] < p.coord;
		} else if constexpr (PredicateCompare::GREATER == PC) {
			return c + hl - t.length(0)[Axis] > p.coord;
		}
	}
};

template <PredicateCompare PC>
struct Filter<X<PC>> : Filter<Coord<0, PC>> {
};

template <PredicateCompare PC>
struct Filter<Y<PC>> : Filter<Coord<1, PC>> {
};

template <PredicateCompare PC>
struct Filter<Z<PC>> : Filter<Coord<2, PC>> {
};

template <PredicateCompare PC>
struct Filter<W<PC>> : Filter<Coord<3, PC>> {
};
}  // namespace ufo::pred

#endif  // UFO_CONTAINER_TREE_PREDICATE_COORD_HPP