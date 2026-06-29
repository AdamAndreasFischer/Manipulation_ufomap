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

#ifndef UFO_CONTAINER_TREE_PREDICATE_LENGTH_HPP
#define UFO_CONTAINER_TREE_PREDICATE_LENGTH_HPP

// UFO
#include <ufo/container/tree/predicate/depth.hpp>
#include <ufo/container/tree/predicate/filter.hpp>
#include <ufo/container/tree/predicate/predicate_compare.hpp>

namespace ufo::pred
{
template <PredicateCompare PC = PredicateCompare::EQUAL>
struct Length {
	double length;

	constexpr Length(double length = 0.0) noexcept : length(length) {}

 private:
	Depth<PC> depth_;

	template <class T>
	friend class Filter;
};

using LengthE  = Length<PredicateCompare::EQUAL>;
using LengthNE = Length<PredicateCompare::NOT_EQUAL>;
using LengthLE = Length<PredicateCompare::LESS_EQUAL>;
using LengthGE = Length<PredicateCompare::GREATER_EQUAL>;
using LengthL  = Length<PredicateCompare::LESS>;
using LengthG  = Length<PredicateCompare::GREATER>;

using LengthMin = LengthGE;
using LengthMax = LengthLE;

template <PredicateCompare PC>
struct Filter<Length<PC>> : public FilterBase<Length<PC>> {
	using Pred = Length<PC>;

	template <class Tree>
	static constexpr void init(Pred& p, Tree const& t)
	{
		int max = t.maxNumDepthLevels();

		if constexpr (PredicateCompare::EQUAL == PC || PredicateCompare::NOT_EQUAL == PC) {
			for (int d{}; max > d; ++d) {
				if (p.length == t.length(d)) {
					p.depth_ = d;
					return;
				}
			}

			p.depth_ = max;
		} else if constexpr (PredicateCompare::LESS_EQUAL == PC) {
			p.depth_ = -1;

			for (int d{}; max > d; ++d) {
				if (p.length >= t.length(d)) {
					p.depth_ = d;
				}
			}
		} else if constexpr (PredicateCompare::LESS == PC) {
			p.depth_ = -1;

			for (int d{}; max > d; ++d) {
				if (p.length > t.length(d)) {
					p.depth_ = d;
				}
			}
		} else if constexpr (PredicateCompare::GREATER_EQUAL == PC) {
			for (int d{}; max > d; ++d) {
				if (p.length <= t.length(d)) {
					p.depth_ = d;
					return;
				}
			}

			p.depth_ = max;
		} else if constexpr (PredicateCompare::GREATER == PC) {
			for (int d{}; max > d; ++d) {
				if (p.length < t.length(d)) {
					p.depth_ = d;
					return;
				}
			}

			p.depth_ = max;
		}
	}

	template <class Tree>
	[[nodiscard]] static constexpr bool returnable(Pred const& p, Tree const& t,
	                                               typename Tree::Node const& n)
	{
		return Filter<Depth<PC>>::returnable(p, t, n);
	}

	template <class Tree>
	[[nodiscard]] static constexpr bool traversable(Pred const& p, Tree const& t,
	                                                typename Tree::Node const& n)
	{
		return Filter<Depth<PC>>::traversable(p, t, n);
	}
};
}  // namespace ufo::pred

#endif  // UFO_CONTAINER_TREE_PREDICATE_LENGTH_HPP