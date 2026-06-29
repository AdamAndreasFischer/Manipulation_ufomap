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

#ifndef UFO_EXECUTION_ALGORITHM_HPP
#define UFO_EXECUTION_ALGORITHM_HPP

// UFO
#include <ufo/execution/execution.hpp>
#include <ufo/utility/index_iterator.hpp>
#include <ufo/utility/type_traits.hpp>

// STL
#include <algorithm>
#include <iterator>
#include <type_traits>

namespace ufo
{
/**************************************************************************************
|                                                                                     |
|                                      For each                                       |
|                                                                                     |
**************************************************************************************/

template <class Index, class UnaryFunc,
          std::enable_if_t<std::is_integral_v<Index>, bool> = true>
UnaryFunc for_each(Index first, Index last, UnaryFunc f)
{
	for (; last != first; ++first) {
		f(first);
	}
	return f;
}

template <
    class ExecutionPolicy, class Index, class UnaryFunc,
    std::enable_if_t<std::is_integral_v<Index>, bool>                         = true,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
void for_each(ExecutionPolicy&& policy, Index first, Index last, UnaryFunc f)
{
	if constexpr (execution::is_stl_v<ExecutionPolicy>) {
		IndexIterator<Index> it(first, last);
		std::for_each(execution::toSTL(std::forward<ExecutionPolicy>(policy)), it.begin(),
		              it.end(), f);
	}
#if defined(UFO_PAR_GCD)
	else if constexpr (execution::is_gcd_v<ExecutionPolicy>) {
		dispatch_apply(last - first, dispatch_get_global_queue(0, 0), ^(Index i) {
			f(first + i);
		});
	}
#endif
#if defined(UFO_PAR_TBB)
	else if constexpr (execution::is_tbb_v<ExecutionPolicy>) {
		oneapi::tbb::parallel_for(first, last, f);
	}
#endif
	else if constexpr (execution::is_omp_v<ExecutionPolicy>) {
#pragma omp parallel for
		for (auto it = first; last != it; ++it) {
			f(it);
		}
	} else {
		static_assert(dependent_false_v<ExecutionPolicy>,
		              "Not implemented for the execution policy 'Unknown'");
	}
}

template <class InputIt, class UnaryFunc,
          std::enable_if_t<!std::is_integral_v<InputIt>, bool> = true>
UnaryFunc for_each(InputIt first, InputIt last, UnaryFunc f)
{
	return std::for_each(first, last, f);
}

template <
    class ExecutionPolicy, class RandomIt, class UnaryFunc,
    std::enable_if_t<!std::is_integral_v<RandomIt>, bool>                     = true,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
void for_each(ExecutionPolicy&& policy, RandomIt first, RandomIt last, UnaryFunc f)
{
	if constexpr (execution::is_stl_v<ExecutionPolicy>) {
		std::for_each(execution::toSTL(std::forward<ExecutionPolicy>(policy)), first, last,
		              f);
	}
#if defined(UFO_PAR_GCD)
	else if constexpr (execution::is_gcd_v<ExecutionPolicy>) {
		std::size_t s = std::distance(first, last);
		dispatch_apply(s, dispatch_get_global_queue(0, 0), ^(std::size_t i) {
			f(first[i]);
		});
	}
#endif
#if defined(UFO_PAR_TBB)
	else if constexpr (execution::is_tbb_v<ExecutionPolicy>) {
		// TODO: Benchmark against parallel_for_each

		std::size_t s = std::distance(first, last);
		oneapi::tbb::parallel_for(std::size_t(0), s,
		                          [first, f](std::size_t i) { f(first[i]); });
	}
#endif
	else if constexpr (execution::is_omp_v<ExecutionPolicy>) {
#pragma omp parallel for
		for (auto it = first; last != it; ++it) {
			f(*it);
		}
	} else {
		static_assert(dependent_false_v<ExecutionPolicy>,
		              "Not implemented for the execution policy 'Unknown'");
	}
}

/**************************************************************************************
|                                                                                     |
|                                      Transform                                      |
|                                                                                     |
**************************************************************************************/

template <class InputIt, class OutputIt, class UnaryOp>
OutputIt transform(InputIt first1, InputIt last1, OutputIt d_first, UnaryOp unary_op)
{
	return std::transform(first1, last1, d_first, unary_op);
}

template <
    class ExecutionPolicy, class RandomIt1, class RandomIt2, class UnaryOp,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
RandomIt2 transform(ExecutionPolicy&& policy, RandomIt1 first1, RandomIt1 last1,
                    RandomIt2 d_first, UnaryOp unary_op)
{
	if constexpr (execution::is_stl_v<ExecutionPolicy>) {
		return std::transform(execution::toSTL(std::forward<ExecutionPolicy>(policy)), first1,
		                      last1, d_first, unary_op);
	} else {
		std::size_t s = std::distance(first1, last1);
		for_each(
		    std::forward<ExecutionPolicy>(policy), std::size_t(0), s,
		    [first1, d_first, unary_op](std::size_t i) { d_first[i] = unary_op(first1[i]); });

		return d_first + s;
	}
}

template <class InputIt1, class InputIt2, class OutputIt, class BinaryOp>
OutputIt transform(InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt d_first,
                   BinaryOp binary_op)
{
	return std::transform(first1, last1, first2, d_first, binary_op);
}

template <
    class ExecutionPolicy, class RandomIt1, class RandomIt2, class RandomIt3,
    class BinaryOp,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
RandomIt3 transform(ExecutionPolicy&& policy, RandomIt1 first1, RandomIt1 last1,
                    RandomIt2 first2, RandomIt3 d_first, BinaryOp binary_op)
{
	if constexpr (execution::is_stl_v<ExecutionPolicy>) {
		return std::transform(execution::toSTL(std::forward<ExecutionPolicy>(policy)), first1,
		                      last1, first2, d_first, binary_op);
	} else {
		std::size_t s = std::distance(first1, last1);
		for_each(std::forward<ExecutionPolicy>(policy), std::size_t(0), s,
		         [first1, first2, d_first, binary_op](std::size_t i) {
			         d_first[i] = binary_op(first1[i], first2[i]);
		         });

		return d_first + s;
	}
}
}  // namespace ufo

#endif  // UFO_EXECUTION_ALGORITHM_HPP