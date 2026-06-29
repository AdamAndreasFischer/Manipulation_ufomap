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

#ifndef UFO_CONTAINER_TREE_PREDICATE_OR_HPP
#define UFO_CONTAINER_TREE_PREDICATE_OR_HPP

// UFO
#include <ufo/container/tree/predicate/filter.hpp>
#include <ufo/utility/type_traits.hpp>

// STL
#include <tuple>
#include <type_traits>
#include <utility>

namespace ufo::pred
{
template <class... Preds>
struct Or {
	Or(Preds&&... preds) : preds(std::forward<Preds>(preds)...) {}

	std::tuple<Preds...> preds;
};

template <class PredLeft, class PredRight,
          std::enable_if_t<!is_specialization_of_v<Or, PredLeft> &&
                               !is_specialization_of_v<Or, PredRight>,
                           bool> = true>
constexpr Or<remove_cvref_t<PredLeft>, remove_cvref_t<PredRight>> operator||(
    PredLeft&& p1, PredRight&& p2)
{
	return {std::forward<PredLeft>(p1), std::forward<PredRight>(p2)};
}

template <class... PredsLeft, class... PredsRight>
constexpr Or<PredsLeft..., PredsRight...> operator||(Or<PredsLeft...>&&  p1,
                                                     Or<PredsRight...>&& p2)
{
	return {std::tuple_cat(std::move(p1.preds), std::move(p2.preds))};
}

template <class... PredsLeft, class... PredsRight>
constexpr Or<PredsLeft..., PredsRight...> operator||(Or<PredsLeft...> const&  p1,
                                                     Or<PredsRight...> const& p2)
{
	return {std::tuple_cat(p1.preds, p2.preds)};
}

template <class... PredsLeft, class PredRight>
constexpr Or<PredsLeft..., remove_cvref_t<PredRight>> operator||(Or<PredsLeft...>&& p1,
                                                                 PredRight&&        p2)
{
	return {
	    std::tuple_cat(std::move(p1.preds), std::make_tuple(std::forward<PredRight>(p2)))};
}

template <class... PredsLeft, class PredRight>
constexpr Or<PredsLeft..., remove_cvref_t<PredRight>> operator||(
    Or<PredsLeft...> const& p1, PredRight&& p2)
{
	return {std::tuple_cat(p1.preds, std::make_tuple(std::forward<PredRight>(p2)))};
}

template <class PredLeft, class... PredsRight>
constexpr Or<remove_cvref_t<PredLeft>, PredsRight...> operator||(PredLeft&&          p1,
                                                                 Or<PredsRight...>&& p2)
{
	return {
	    std::tuple_cat(std::make_tuple(std::forward<PredLeft>(p1)), std::move(p2.preds))};
}

template <class PredLeft, class... PredsRight>
constexpr Or<remove_cvref_t<PredLeft>, PredsRight...> operator||(
    PredLeft&& p1, Or<PredsRight...> const& p2)
{
	return {std::tuple_cat(std::make_tuple(std::forward<PredLeft>(p1)), p2.preds)};
}

template <class... Preds>
struct Filter<Or<Preds...>> : public FilterBase<Or<Preds...>> {
	using Pred = Or<Preds...>;

	template <class Tree>
	static constexpr void init(Pred& p, Tree const& t)
	{
		std::apply(
		    [&t](auto&... p) { ((Filter<remove_cvref_t<decltype(p)>>::init(p, t)), ...); },
		    p.preds);
	}

	template <class Value>
	[[nodiscard]] static constexpr bool returnable(Pred const& p, Value const& v)
	{
		return std::apply(
		    [&v](auto const&... p) {
			    return ((Filter<remove_cvref_t<decltype(p)>>::returnable(p, v)) || ...);
		    },
		    p.preds);
	}

	template <class Tree>
	[[nodiscard]] static constexpr bool returnable(Pred const& p, Tree const& t,
	                                               typename Tree::Node const& n)
	{
		return std::apply(
		    [&t, &n](auto const&... p) {
			    return ((Filter<remove_cvref_t<decltype(p)>>::returnable(p, t, n)) || ...);
		    },
		    p.preds);
	}

	template <class Tree>
	[[nodiscard]] static constexpr bool traversable(Pred const& p, Tree const& t,
	                                                typename Tree::Node const& n)
	{
		return std::apply(
		    [&t, &n](auto const&... p) {
			    return ((Filter<remove_cvref_t<decltype(p)>>::traversable(p, t, n)) || ...);
		    },
		    p.preds);
	}
};

namespace detail
{
template <class T, class... Ts>
struct contains_pred<T, Or<Ts...>> : std::disjunction<contains_pred<T, Ts>...> {
};

template <class T, class... Ts>
struct contains_always_pred<T, Or<Ts...>>
    : std::conjunction<contains_always_pred<T, Ts>...> {
};
}  // namespace detail
}  // namespace ufo::pred

#endif  // UFO_CONTAINER_TREE_PREDICATE_OR_HPP