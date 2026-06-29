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
#include <ufo/utility/type_traits.hpp>

// STL
#include <algorithm>

namespace ufo
{
// mutating sequence operations
// copy
template <class InputIter, class OutputIter>
constexpr OutputIter copy(InputIter first, InputIter last, OutputIter result);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
ForwardIter2 copy(ExecutionPolicy&& exec, ForwardIter1 first, ForwardIter1 last,
                  ForwardIter2 result);

template <class InputIter, class Size, class OutputIter>
constexpr OutputIter copy_n(InputIter first, Size n, OutputIter result);
template <class ExecutionPolicy, class ForwardIter1, class Size, class ForwardIter2>
ForwardIter2 copy_n(ExecutionPolicy&& exec, ForwardIter1 first, Size n,
                    ForwardIter2 result);

template <class InputIter, class OutputIter, class Pred>
constexpr OutputIter copy_if(InputIter first, InputIter last, OutputIter result,
                             Pred pred);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class Pred>
ForwardIter2 copy_if(ExecutionPolicy&& exec, ForwardIter1 first, ForwardIter1 last,
                     ForwardIter2 result, Pred pred);

template <class BidirectionalIter1, class BidirectionalIter2>
constexpr BidirectionalIter2 copy_backward(BidirectionalIter1 first,
                                           BidirectionalIter1 last,
                                           BidirectionalIter2 result);

// move
template <class InputIter, class OutputIter>
constexpr OutputIter move(InputIter first, InputIter last, OutputIter result);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
ForwardIter2 move(ExecutionPolicy&& exec, ForwardIter1 first, ForwardIter1 last,
                  ForwardIter2 result);

template <class BidirectionalIter1, class BidirectionalIter2>
constexpr BidirectionalIter2 move_backward(BidirectionalIter1 first,
                                           BidirectionalIter1 last,
                                           BidirectionalIter2 result);

// swap
template <class ForwardIter1, class ForwardIter2>
constexpr ForwardIter2 swap_ranges(ForwardIter1 first1, ForwardIter1 last1,
                                   ForwardIter2 first2);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
ForwardIter2 swap_ranges(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
                         ForwardIter2 first2);

template <class ForwardIter1, class ForwardIter2>
constexpr void iter_swap(ForwardIter1 a, ForwardIter2 b);

// replace
template <class ForwardIter, class T = typename iterator_traits<ForwardIter>::value_type>
constexpr void replace(ForwardIter first, ForwardIter last, T const& old_value,
                       T const& new_value);
template <class ExecutionPolicy, class ForwardIter,
          class T = typename iterator_traits<ForwardIter>::value_type>
void replace(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
             T const& old_value, T const& new_value);
template <class ForwardIter, class Pred,
          class T = typename iterator_traits<ForwardIter>::value_type>
constexpr void replace_if(ForwardIter first, ForwardIter last, Pred pred,
                          T const& new_value);
template <class ExecutionPolicy, class ForwardIter, class Pred,
          class T = typename iterator_traits<ForwardIter>::value_type>
void replace_if(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last, Pred pred,
                T const& new_value);

template <class InputIter, class OutputIter, class T>
constexpr OutputIter replace_copy(InputIter first, InputIter last, OutputIter result,
                                  T const& old_value, T const& new_value);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class T>
ForwardIter2 replace_copy(ExecutionPolicy&& exec, ForwardIter1 first, ForwardIter1 last,
                          ForwardIter2 result, T const& old_value, T const& new_value);
template <class InputIter, class OutputIter, class Pred,
          class T = typename iterator_traits<OutputIter>::value_type>
constexpr OutputIter replace_copy_if(InputIter first, InputIter last, OutputIter result,
                                     Pred pred, T const& new_value);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class Pred,
          class T = typename iterator_traits<ForwardIter2>::value_type>
ForwardIter2 replace_copy_if(ExecutionPolicy&& exec, ForwardIter1 first,
                             ForwardIter1 last, ForwardIter2 result, Pred pred,
                             T const& new_value);

// fill
template <class ForwardIter, class T = typename iterator_traits<ForwardIter>::value_type>
constexpr void fill(ForwardIter first, ForwardIter last, T const& value);
template <class ExecutionPolicy, class ForwardIter,
          class T = typename iterator_traits<ForwardIter>::value_type>
void fill(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last, T const& value);
template <class OutputIter, class Size,
          class T = typename iterator_traits<OutputIter>::value_type>
constexpr OutputIter fill_n(OutputIter first, Size n, T const& value);
template <class ExecutionPolicy, class ForwardIter, class Size,
          class T = typename iterator_traits<OutputIter>::value_type>
ForwardIter fill_n(ExecutionPolicy&& exec, ForwardIter first, Size n, T const& value);

// generate
template <class ForwardIter, class Generator>
constexpr void generate(ForwardIter first, ForwardIter last, Generator gen);
template <class ExecutionPolicy, class ForwardIter, class Generator>
void generate(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last, Generator gen);
template <class OutputIter, class Size, class Generator>
constexpr OutputIter generate_n(OutputIter first, Size n, Generator gen);
template <class ExecutionPolicy, class ForwardIter, class Size, class Generator>
ForwardIter generate_n(ExecutionPolicy&& exec, ForwardIter first, Size n, Generator gen);

// remove
template <class ForwardIter, class T = typename iterator_traits<ForwardIter>::value_type>
constexpr ForwardIter remove(ForwardIter first, ForwardIter last, T const& value);
template <class ExecutionPolicy, class ForwardIter,
          class T = typename iterator_traits<ForwardIter>::value_type>
ForwardIter remove(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                   T const& value);
template <class ForwardIter, class Pred>
constexpr ForwardIter remove_if(ForwardIter first, ForwardIter last, Pred pred);
template <class ExecutionPolicy, class ForwardIter, class Pred>
ForwardIter remove_if(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                      Pred pred);

template <class InputIter, class OutputIter,
          class T = typename iterator_traits<InputIter>::value_type>
constexpr OutputIter remove_copy(InputIter first, InputIter last, OutputIter result,
                                 T const& value);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2,
          class T = typename iterator_traits<ForwardIter1>::value_type>
ForwardIter2 remove_copy(ExecutionPolicy&& exec, ForwardIter1 first, ForwardIter1 last,
                         ForwardIter2 result, T const& value);
template <class InputIter, class OutputIter, class Pred>
constexpr OutputIter remove_copy_if(InputIter first, InputIter last, OutputIter result,
                                    Pred pred);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class Pred>
ForwardIter2 remove_copy_if(ExecutionPolicy&& exec, ForwardIter1 first, ForwardIter1 last,
                            ForwardIter2 result, Pred pred);

// unique
template <class ForwardIter>
constexpr ForwardIter unique(ForwardIter first, ForwardIter last);
template <class ForwardIter, class BinaryPred>
constexpr ForwardIter unique(ForwardIter first, ForwardIter last, BinaryPred pred);
template <class ExecutionPolicy, class ForwardIter>
ForwardIter unique(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last);
template <class ExecutionPolicy, class ForwardIter, class BinaryPred>
ForwardIter unique(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                   BinaryPred pred);

template <class InputIter, class OutputIter>
constexpr OutputIter unique_copy(InputIter first, InputIter last, OutputIter result);
template <class InputIter, class OutputIter, class BinaryPred>
constexpr OutputIter unique_copy(InputIter first, InputIter last, OutputIter result,
                                 BinaryPred pred);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
ForwardIter2 unique_copy(ExecutionPolicy&& exec, ForwardIter1 first, ForwardIter1 last,
                         ForwardIter2 result);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class BinaryPred>
ForwardIter2 unique_copy(ExecutionPolicy&& exec, ForwardIter1 first, ForwardIter1 last,
                         ForwardIter2 result, BinaryPred pred);

// reverse
template <class BidirectionalIter>
constexpr void reverse(BidirectionalIter first, BidirectionalIter last);
template <class ExecutionPolicy, class BidirectionalIter>
void reverse(ExecutionPolicy&& exec, BidirectionalIter first, BidirectionalIter last);

template <class BidirectionalIter, class OutputIter>
constexpr OutputIter reverse_copy(BidirectionalIter first, BidirectionalIter last,
                                  OutputIter result);
template <class ExecutionPolicy, class BidirectionalIter, class ForwardIter>
ForwardIter reverse_copy(ExecutionPolicy&& exec, BidirectionalIter first,
                         BidirectionalIter last, ForwardIter result);

// rotate
template <class ForwardIter>
constexpr ForwardIter rotate(ForwardIter first, ForwardIter middle, ForwardIter last);
template <class ExecutionPolicy, class ForwardIter>
ForwardIter rotate(ExecutionPolicy&& exec, ForwardIter first, ForwardIter middle,
                   ForwardIter last);

template <class ForwardIter, class OutputIter>
constexpr OutputIter rotate_copy(ForwardIter first, ForwardIter middle, ForwardIter last,
                                 OutputIter result);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
ForwardIter2 rotate_copy(ExecutionPolicy&& exec, ForwardIter1 first, ForwardIter1 middle,
                         ForwardIter1 last, ForwardIter2 result);

// sample
template <class PopulationIter, class SampleIter, class Distance,
          class UniformRandomBitGenerator>
SampleIter sample(PopulationIter first, PopulationIter last, SampleIter out, Distance n,
                  UniformRandomBitGenerator&& g);

// shuffle
template <class RandomAccessIter, class UniformRandomBitGenerator>
void shuffle(RandomAccessIter first, RandomAccessIter last,
             UniformRandomBitGenerator&& g);

// shift
template <class ForwardIter>
constexpr ForwardIter shift_left(
    ForwardIter first, ForwardIter last,
    typename iterator_traits<ForwardIter>::difference_type n);
template <class ExecutionPolicy, class ForwardIter>
ForwardIter shift_left(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                       typename iterator_traits<ForwardIter>::difference_type n);

template <class ForwardIter>
constexpr ForwardIter shift_right(
    ForwardIter first, ForwardIter last,
    typename iterator_traits<ForwardIter>::difference_type n);
template <class ExecutionPolicy, class ForwardIter>
ForwardIter shift_right(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                        typename iterator_traits<ForwardIter>::difference_type n);

// sorting and related operations
// sorting
template <class RandomAccessIter>
constexpr void sort(RandomAccessIter first, RandomAccessIter last);
template <class RandomAccessIter, class Compare>
constexpr void sort(RandomAccessIter first, RandomAccessIter last, Compare comp);
template <class ExecutionPolicy, class RandomAccessIter>
void sort(ExecutionPolicy&& exec, RandomAccessIter first, RandomAccessIter last);
template <class ExecutionPolicy, class RandomAccessIter, class Compare>
void sort(ExecutionPolicy&& exec, RandomAccessIter first, RandomAccessIter last,
          Compare comp);

template <class RandomAccessIter>
void stable_sort(RandomAccessIter first, RandomAccessIter last);
template <class RandomAccessIter, class Compare>
void stable_sort(RandomAccessIter first, RandomAccessIter last, Compare comp);
template <class ExecutionPolicy, class RandomAccessIter>
void stable_sort(ExecutionPolicy&& exec, RandomAccessIter first, RandomAccessIter last);
template <class ExecutionPolicy, class RandomAccessIter, class Compare>
void stable_sort(ExecutionPolicy&& exec, RandomAccessIter first, RandomAccessIter last,
                 Compare comp);

template <class RandomAccessIter>
constexpr void partial_sort(RandomAccessIter first, RandomAccessIter middle,
                            RandomAccessIter last);
template <class RandomAccessIter, class Compare>
constexpr void partial_sort(RandomAccessIter first, RandomAccessIter middle,
                            RandomAccessIter last, Compare comp);
template <class ExecutionPolicy, class RandomAccessIter>
void partial_sort(ExecutionPolicy&& exec, RandomAccessIter first, RandomAccessIter middle,
                  RandomAccessIter last);
template <class ExecutionPolicy, class RandomAccessIter, class Compare>
void partial_sort(ExecutionPolicy&& exec, RandomAccessIter first, RandomAccessIter middle,
                  RandomAccessIter last, Compare comp);

template <class InputIter, class RandomAccessIter>
constexpr RandomAccessIter partial_sort_copy(InputIter first, InputIter last,
                                             RandomAccessIter result_first,
                                             RandomAccessIter result_last);
template <class InputIter, class RandomAccessIter, class Compare>
constexpr RandomAccessIter partial_sort_copy(InputIter first, InputIter last,
                                             RandomAccessIter result_first,
                                             RandomAccessIter result_last, Compare comp);
template <class ExecutionPolicy, class ForwardIter, class RandomAccessIter>
RandomAccessIter partial_sort_copy(ExecutionPolicy&& exec, ForwardIter first,
                                   ForwardIter last, RandomAccessIter result_first,
                                   RandomAccessIter result_last);
template <class ExecutionPolicy, class ForwardIter, class RandomAccessIter, class Compare>
RandomAccessIter partial_sort_copy(ExecutionPolicy&& exec, ForwardIter first,
                                   ForwardIter last, RandomAccessIter result_first,
                                   RandomAccessIter result_last, Compare comp);

template <class ForwardIter>
constexpr bool is_sorted(ForwardIter first, ForwardIter last);
template <class ForwardIter, class Compare>
constexpr bool is_sorted(ForwardIter first, ForwardIter last, Compare comp);
template <class ExecutionPolicy, class ForwardIter>
bool is_sorted(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last);
template <class ExecutionPolicy, class ForwardIter, class Compare>
bool is_sorted(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last, Compare comp);

template <class ForwardIter>
constexpr ForwardIter is_sorted_until(ForwardIter first, ForwardIter last);
template <class ForwardIter, class Compare>
constexpr ForwardIter is_sorted_until(ForwardIter first, ForwardIter last, Compare comp);
template <class ExecutionPolicy, class ForwardIter>
ForwardIter is_sorted_until(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last);
template <class ExecutionPolicy, class ForwardIter, class Compare>
ForwardIter is_sorted_until(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                            Compare comp);

// Nth element
template <class RandomAccessIter>
constexpr void nth_element(RandomAccessIter first, RandomAccessIter nth,
                           RandomAccessIter last);
template <class RandomAccessIter, class Compare>
constexpr void nth_element(RandomAccessIter first, RandomAccessIter nth,
                           RandomAccessIter last, Compare comp);
template <class ExecutionPolicy, class RandomAccessIter>
void nth_element(ExecutionPolicy&& exec, RandomAccessIter first, RandomAccessIter nth,
                 RandomAccessIter last);
template <class ExecutionPolicy, class RandomAccessIter, class Compare>
void nth_element(ExecutionPolicy&& exec, RandomAccessIter first, RandomAccessIter nth,
                 RandomAccessIter last, Compare comp);

// binary search
template <class ForwardIter, class T = typename iterator_traits<ForwardIter>::value_type>
constexpr ForwardIter lower_bound(ForwardIter first, ForwardIter last, T const& value);
template <class ForwardIter, class T = typename iterator_traits<ForwardIter>::value_type,
          class Compare>
constexpr ForwardIter lower_bound(ForwardIter first, ForwardIter last, T const& value,
                                  Compare comp);

template <class ForwardIter, class T = typename iterator_traits<ForwardIter>::value_type>
constexpr ForwardIter upper_bound(ForwardIter first, ForwardIter last, T const& value);
template <class ForwardIter, class T = typename iterator_traits<ForwardIter>::value_type,
          class Compare>
constexpr ForwardIter upper_bound(ForwardIter first, ForwardIter last, T const& value,
                                  Compare comp);

template <class ForwardIter, class T = typename iterator_traits<ForwardIter>::value_type>
constexpr pair<ForwardIter, ForwardIter> equal_range(ForwardIter first, ForwardIter last,
                                                     T const& value);
template <class ForwardIter, class T = typename iterator_traits<ForwardIter>::value_type,
          class Compare>
constexpr pair<ForwardIter, ForwardIter> equal_range(ForwardIter first, ForwardIter last,
                                                     T const& value, Compare comp);

template <class ForwardIter, class T = typename iterator_traits<ForwardIter>::value_type>
constexpr bool binary_search(ForwardIter first, ForwardIter last, T const& value);
template <class ForwardIter, class T = typename iterator_traits<ForwardIter>::value_type,
          class Compare>
constexpr bool binary_search(ForwardIter first, ForwardIter last, T const& value,
                             Compare comp);

// partitions
template <class InputIter, class Pred>
constexpr bool is_partitioned(InputIter first, InputIter last, Pred pred);
template <class ExecutionPolicy, class ForwardIter, class Pred>
bool is_partitioned(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                    Pred pred);

template <class ForwardIter, class Pred>
constexpr ForwardIter partition(ForwardIter first, ForwardIter last, Pred pred);
template <class ExecutionPolicy, class ForwardIter, class Pred>
ForwardIter partition(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                      Pred pred);

template <class BidirectionalIter, class Pred>
BidirectionalIter stable_partition(BidirectionalIter first, BidirectionalIter last,
                                   Pred pred);
template <class ExecutionPolicy, class BidirectionalIter, class Pred>
BidirectionalIter stable_partition(ExecutionPolicy&& exec, BidirectionalIter first,
                                   BidirectionalIter last, Pred pred);

template <class InputIter, class OutputIter1, class OutputIter2, class Pred>
constexpr pair<OutputIter1, OutputIter2> partition_copy(InputIter first, InputIter last,
                                                        OutputIter1 out_true,
                                                        OutputIter2 out_false, Pred pred);
template <class ExecutionPolicy, class ForwardIter, class ForwardIter1,
          class ForwardIter2, class Pred>
pair<ForwardIter1, ForwardIter2> partition_copy(ExecutionPolicy&& exec, ForwardIter first,
                                                ForwardIter last, ForwardIter1 out_true,
                                                ForwardIter2 out_false, Pred pred);

template <class ForwardIter, class Pred>
constexpr ForwardIter partition_point(ForwardIter first, ForwardIter last, Pred pred);

// merge
template <class InputIter1, class InputIter2, class OutputIter>
constexpr OutputIter merge(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                           InputIter2 last2, OutputIter result);
template <class InputIter1, class InputIter2, class OutputIter, class Compare>
constexpr OutputIter merge(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                           InputIter2 last2, OutputIter result, Compare comp);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2,
          class ForwardIter>
ForwardIter merge(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
                  ForwardIter2 first2, ForwardIter2 last2, ForwardIter result);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2,
          class ForwardIter, class Compare>
ForwardIter merge(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
                  ForwardIter2 first2, ForwardIter2 last2, ForwardIter result,
                  Compare comp);

template <class BidirectionalIter>
void inplace_merge(BidirectionalIter first, BidirectionalIter middle,
                   BidirectionalIter last);
template <class BidirectionalIter, class Compare>
void inplace_merge(BidirectionalIter first, BidirectionalIter middle,
                   BidirectionalIter last, Compare comp);
template <class ExecutionPolicy, class BidirectionalIter>
void inplace_merge(ExecutionPolicy&& exec, BidirectionalIter first,
                   BidirectionalIter middle, BidirectionalIter last);
template <class ExecutionPolicy, class BidirectionalIter, class Compare>
void inplace_merge(ExecutionPolicy&& exec, BidirectionalIter first,
                   BidirectionalIter middle, BidirectionalIter last, Compare comp);

// set operations
template <class InputIter1, class InputIter2>
constexpr bool includes(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                        InputIter2 last2);
template <class InputIter1, class InputIter2, class Compare>
constexpr bool includes(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                        InputIter2 last2, Compare comp);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
bool includes(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
              ForwardIter2 first2, ForwardIter2 last2);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class Compare>
bool includes(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
              ForwardIter2 first2, ForwardIter2 last2, Compare comp);

template <class InputIter1, class InputIter2, class OutputIter>
constexpr OutputIter set_union(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                               InputIter2 last2, OutputIter result);
template <class InputIter1, class InputIter2, class OutputIter, class Compare>
constexpr OutputIter set_union(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                               InputIter2 last2, OutputIter result, Compare comp);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2,
          class ForwardIter>
ForwardIter set_union(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
                      ForwardIter2 first2, ForwardIter2 last2, ForwardIter result);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2,
          class ForwardIter, class Compare>
ForwardIter set_union(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
                      ForwardIter2 first2, ForwardIter2 last2, ForwardIter result,
                      Compare comp);

template <class InputIter1, class InputIter2, class OutputIter>
constexpr OutputIter set_intersection(InputIter1 first1, InputIter1 last1,
                                      InputIter2 first2, InputIter2 last2,
                                      OutputIter result);
template <class InputIter1, class InputIter2, class OutputIter, class Compare>
constexpr OutputIter set_intersection(InputIter1 first1, InputIter1 last1,
                                      InputIter2 first2, InputIter2 last2,
                                      OutputIter result, Compare comp);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2,
          class ForwardIter>
ForwardIter set_intersection(ExecutionPolicy&& exec, ForwardIter1 first1,
                             ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2,
                             ForwardIter result);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2,
          class ForwardIter, class Compare>
ForwardIter set_intersection(ExecutionPolicy&& exec, ForwardIter1 first1,
                             ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2,
                             ForwardIter result, Compare comp);

template <class InputIter1, class InputIter2, class OutputIter>
constexpr OutputIter set_difference(InputIter1 first1, InputIter1 last1,
                                    InputIter2 first2, InputIter2 last2,
                                    OutputIter result);
template <class InputIter1, class InputIter2, class OutputIter, class Compare>
constexpr OutputIter set_difference(InputIter1 first1, InputIter1 last1,
                                    InputIter2 first2, InputIter2 last2,
                                    OutputIter result, Compare comp);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2,
          class ForwardIter>
ForwardIter set_difference(ExecutionPolicy&& exec, ForwardIter1 first1,
                           ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2,
                           ForwardIter result);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2,
          class ForwardIter, class Compare>
ForwardIter set_difference(ExecutionPolicy&& exec, ForwardIter1 first1,
                           ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2,
                           ForwardIter result, Compare comp);

template <class InputIter1, class InputIter2, class OutputIter>
constexpr OutputIter set_symmetric_difference(InputIter1 first1, InputIter1 last1,
                                              InputIter2 first2, InputIter2 last2,
                                              OutputIter result);
template <class InputIter1, class InputIter2, class OutputIter, class Compare>
constexpr OutputIter set_symmetric_difference(InputIter1 first1, InputIter1 last1,
                                              InputIter2 first2, InputIter2 last2,
                                              OutputIter result, Compare comp);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2,
          class ForwardIter>
ForwardIter set_symmetric_difference(ExecutionPolicy&& exec, ForwardIter1 first1,
                                     ForwardIter1 last1, ForwardIter2 first2,
                                     ForwardIter2 last2, ForwardIter result);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2,
          class ForwardIter, class Compare>
ForwardIter set_symmetric_difference(ExecutionPolicy&& exec, ForwardIter1 first1,
                                     ForwardIter1 last1, ForwardIter2 first2,
                                     ForwardIter2 last2, ForwardIter result,
                                     Compare comp);

// heap operations
template <class RandomAccessIter>
constexpr void push_heap(RandomAccessIter first, RandomAccessIter last);
template <class RandomAccessIter, class Compare>
constexpr void push_heap(RandomAccessIter first, RandomAccessIter last, Compare comp);

template <class RandomAccessIter>
constexpr void pop_heap(RandomAccessIter first, RandomAccessIter last);
template <class RandomAccessIter, class Compare>
constexpr void pop_heap(RandomAccessIter first, RandomAccessIter last, Compare comp);

template <class RandomAccessIter>
constexpr void make_heap(RandomAccessIter first, RandomAccessIter last);
template <class RandomAccessIter, class Compare>
constexpr void make_heap(RandomAccessIter first, RandomAccessIter last, Compare comp);

template <class RandomAccessIter>
constexpr void sort_heap(RandomAccessIter first, RandomAccessIter last);
template <class RandomAccessIter, class Compare>
constexpr void sort_heap(RandomAccessIter first, RandomAccessIter last, Compare comp);

template <class RandomAccessIter>
constexpr bool is_heap(RandomAccessIter first, RandomAccessIter last);
template <class RandomAccessIter, class Compare>
constexpr bool is_heap(RandomAccessIter first, RandomAccessIter last, Compare comp);
template <class ExecutionPolicy, class RandomAccessIter>
bool is_heap(ExecutionPolicy&& exec, RandomAccessIter first, RandomAccessIter last);
template <class ExecutionPolicy, class RandomAccessIter, class Compare>
bool is_heap(ExecutionPolicy&& exec, RandomAccessIter first, RandomAccessIter last,
             Compare comp);

template <class RandomAccessIter>
constexpr RandomAccessIter is_heap_until(RandomAccessIter first, RandomAccessIter last);
template <class RandomAccessIter, class Compare>
constexpr RandomAccessIter is_heap_until(RandomAccessIter first, RandomAccessIter last,
                                         Compare comp);
template <class ExecutionPolicy, class RandomAccessIter>
RandomAccessIter is_heap_until(ExecutionPolicy&& exec, RandomAccessIter first,
                               RandomAccessIter last);
template <class ExecutionPolicy, class RandomAccessIter, class Compare>
RandomAccessIter is_heap_until(ExecutionPolicy&& exec, RandomAccessIter first,
                               RandomAccessIter last, Compare comp);

// minimum and maximum
template <class T>
constexpr T const& min(T const& a, T const& b);
template <class T, class Compare>
constexpr T const& min(T const& a, T const& b, Compare comp);
template <class T>
constexpr T min(initializer_list<T> t);
template <class T, class Compare>
constexpr T min(initializer_list<T> t, Compare comp);

template <class T>
constexpr T const& max(T const& a, T const& b);
template <class T, class Compare>
constexpr T const& max(T const& a, T const& b, Compare comp);
template <class T>
constexpr T max(initializer_list<T> t);
template <class T, class Compare>
constexpr T max(initializer_list<T> t, Compare comp);

template <class T>
constexpr pair<T const&, T const&> minmax(T const& a, T const& b);
template <class T, class Compare>
constexpr pair<T const&, T const&> minmax(T const& a, T const& b, Compare comp);
template <class T>
constexpr pair<T, T> minmax(initializer_list<T> t);
template <class T, class Compare>
constexpr pair<T, T> minmax(initializer_list<T> t, Compare comp);

template <class ForwardIter>
constexpr ForwardIter min_element(ForwardIter first, ForwardIter last);
template <class ForwardIter, class Compare>
constexpr ForwardIter min_element(ForwardIter first, ForwardIter last, Compare comp);
template <class ExecutionPolicy, class ForwardIter>
ForwardIter min_element(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last);
template <class ExecutionPolicy, class ForwardIter, class Compare>
ForwardIter min_element(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                        Compare comp);

template <class ForwardIter>
constexpr ForwardIter max_element(ForwardIter first, ForwardIter last);
template <class ForwardIter, class Compare>
constexpr ForwardIter max_element(ForwardIter first, ForwardIter last, Compare comp);
template <class ExecutionPolicy, class ForwardIter>
ForwardIter max_element(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last);
template <class ExecutionPolicy, class ForwardIter, class Compare>
ForwardIter max_element(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                        Compare comp);

template <class ForwardIter>
constexpr pair<ForwardIter, ForwardIter> minmax_element(ForwardIter first,
                                                        ForwardIter last);
template <class ForwardIter, class Compare>
constexpr pair<ForwardIter, ForwardIter> minmax_element(ForwardIter first,
                                                        ForwardIter last, Compare comp);
template <class ExecutionPolicy, class ForwardIter>
pair<ForwardIter, ForwardIter> minmax_element(ExecutionPolicy&& exec, ForwardIter first,
                                              ForwardIter last);
template <class ExecutionPolicy, class ForwardIter, class Compare>
pair<ForwardIter, ForwardIter> minmax_element(ExecutionPolicy&& exec, ForwardIter first,
                                              ForwardIter last, Compare comp);

// bounded value
template <class T>
constexpr T const& clamp(T const& v, T const& lo, T const& hi);
template <class T, class Compare>
constexpr T const& clamp(T const& v, T const& lo, T const& hi, Compare comp);

// lexicographical comparison
template <class InputIter1, class InputIter2>
constexpr bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                                       InputIter2 first2, InputIter2 last2);
template <class InputIter1, class InputIter2, class Compare>
constexpr bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                                       InputIter2 first2, InputIter2 last2, Compare comp);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
bool lexicographical_compare(ExecutionPolicy&& exec, ForwardIter1 first1,
                             ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class Compare>
bool lexicographical_compare(ExecutionPolicy&& exec, ForwardIter1 first1,
                             ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2,
                             Compare comp);

// three-way comparison algorithms
template <class InputIter1, class InputIter2, class Cmp>
constexpr auto lexicographical_compare_three_way(InputIter1 b1, InputIter1 e1,
                                                 InputIter2 b2, InputIter2 e2,
                                                 Cmp comp) -> decltype(comp(*b1, *b2));
template <class InputIter1, class InputIter2>
constexpr auto lexicographical_compare_three_way(InputIter1 b1, InputIter1 e1,
                                                 InputIter2 b2, InputIter2 e2);

// permutations
template <class BidirectionalIter>
constexpr bool next_permutation(BidirectionalIter first, BidirectionalIter last);
template <class BidirectionalIter, class Compare>
constexpr bool next_permutation(BidirectionalIter first, BidirectionalIter last,
                                Compare comp);

template <class BidirectionalIter>
constexpr bool prev_permutation(BidirectionalIter first, BidirectionalIter last);
template <class BidirectionalIter, class Compare>
constexpr bool prev_permutation(BidirectionalIter first, BidirectionalIter last,
                                Compare comp);

/**************************************************************************************
|                                                                                     |
|                          Non-modifying sequence operations                          |
|                                                                                     |
**************************************************************************************/

template <class InputIter, class Pred>
constexpr bool all_of(InputIter first, InputIter last, Pred pred)
{
	return std::all_of(first, last, pred);
}

template <class ExecutionPolicy, class ForwardIter, class Pred>
bool all_of(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last, Pred pred)
{
	if constexpr (execution::is_stl_v<ExecutionPolicy>) {
		return std::all_of(execution::to_stl_t<ExecutionPolicy>(), first, last, pred);
	}
#if defined(UFO_PAR_GCD)
	else if constexpr (execution::is_gcd_v<ExecutionPolicy>) {
		// TODO: Implement
	}
#endif
#if defined(UFO_PAR_TBB)
	else if constexpr (execution::is_tbb_v<ExecutionPolicy>) {
		// TODO: Implement
	}
#endif
	else if constexpr (execution::is_omp_v<ExecutionPolicy>) {
		// TODO: Implement
	} else {
		static_assert(dependent_false_v<ExecutionPolicy>,
		              "create not implemented for the execution policy");
	}
}

template <class InputIter, class Pred>
constexpr bool any_of(InputIter first, InputIter last, Pred pred)
{
	return std::any_of(first, last, pred);
}

template <class ExecutionPolicy, class ForwardIter, class Pred>
bool any_of(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last, Pred pred)
{
	// TODO: Implement
}

template <class InputIter, class Pred>
constexpr bool none_of(InputIter first, InputIter last, Pred pred)
{
	return std::none_of(first, last, pred);
}

template <class ExecutionPolicy, class ForwardIter, class Pred>
bool none_of(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last, Pred pred)
{
	// TODO: Implement
}

template <class InputIter, class Function>
constexpr Function for_each(InputIter first, InputIter last, Function f)
{
	return std::for_each(first, last, f);
}

template <class ExecutionPolicy, class ForwardIter, class Function>
void for_each(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last, Function f)
{
	// TODO: Implement
}

template <class InputIter, class Size, class Function>
constexpr InputIter for_each_n(InputIter first, Size n, Function f)
{
	return std::for_each_n(first, n, f);
}

template <class ExecutionPolicy, class ForwardIter, class Size, class Function>
ForwardIter for_each_n(ExecutionPolicy&& exec, ForwardIter first, Size n, Function f)
{
	// TODO: Implement
}

template <class InputIter, class T = typename iterator_traits<InputIter>::value_type>
constexpr InputIter find(InputIter first, InputIter last, T const& value)
{
	return std::find(first, last, value);
}

template <class ExecutionPolicy, class ForwardIter,
          class T = typename iterator_traits<InputIter>::value_type>
ForwardIter find(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                 T const& value)
{
	// TODO: Implement
}

template <class InputIter, class Pred>
constexpr InputIter find_if(InputIter first, InputIter last, Pred pred)
{
	return std::find_if(first, last, pred);
}

template <class ExecutionPolicy, class ForwardIter, class Pred>
ForwardIter find_if(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                    Pred pred)
{
	// TODO: Implement
}

template <class InputIter, class Pred>
constexpr InputIter find_if_not(InputIter first, InputIter last, Pred pred)
{
	return std::find_if_not(first, last, pred);
}

template <class ExecutionPolicy, class ForwardIter, class Pred>
ForwardIter find_if_not(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                        Pred pred)
{
	// TODO: Implement
}

template <class ForwardIter1, class ForwardIter2>
constexpr ForwardIter1 find_end(ForwardIter1 first1, ForwardIter1 last1,
                                ForwardIter2 first2, ForwardIter2 last2)
{
	return std::find_end(first1, last1, first2, last2);
}

template <class ForwardIter1, class ForwardIter2, class BinaryPred>
constexpr ForwardIter1 find_end(ForwardIter1 first1, ForwardIter1 last1,
                                ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred)
{
	return std::find_end(first1, last1, first2, last2, pred);
}

template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
ForwardIter1 find_end(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
                      ForwardIter2 first2, ForwardIter2 last2)
{
	// TODO: Implement
}

template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class BinaryPred>
ForwardIter1 find_end(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
                      ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred)
{
	// TODO: Implement
}

template <class InputIter, class ForwardIter>
constexpr InputIter find_first_of(InputIter first1, InputIter last1, ForwardIter first2,
                                  ForwardIter last2)
{
	return std::find_first_of(first1, last1, first2, last2);
}

template <class InputIter, class ForwardIter, class BinaryPred>
constexpr InputIter find_first_of(InputIter first1, InputIter last1, ForwardIter first2,
                                  ForwardIter last2, BinaryPred pred)
{
	return std::find_first_of(first1, last1, first2, last2, pred);
}

template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
ForwardIter1 find_first_of(ExecutionPolicy&& exec, ForwardIter1 first1,
                           ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2)
{
	// TODO: Implement
}

template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class BinaryPred>
ForwardIter1 find_first_of(ExecutionPolicy&& exec, ForwardIter1 first1,
                           ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2,
                           BinaryPred pred)
{
	// TODO: Implement
}

// adjacent find
template <class ForwardIter>
constexpr ForwardIter adjacent_find(ForwardIter first, ForwardIter last);
template <class ForwardIter, class BinaryPred>
constexpr ForwardIter adjacent_find(ForwardIter first, ForwardIter last, BinaryPred pred);
template <class ExecutionPolicy, class ForwardIter>
ForwardIter adjacent_find(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last);
template <class ExecutionPolicy, class ForwardIter, class BinaryPred>
ForwardIter adjacent_find(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                          BinaryPred pred);

// count
template <class InputIter, class T = typename iterator_traits<InputIter>::value_type>
constexpr typename iterator_traits<InputIter>::difference_type count(InputIter first,
                                                                     InputIter last,
                                                                     T const&  value);
template <class ExecutionPolicy, class ForwardIter,
          class T = typename iterator_traits<InputIterator>::value_type>
typename iterator_traits<ForwardIter>::difference_type count(ExecutionPolicy&& exec,
                                                             ForwardIter       first,
                                                             ForwardIter       last,
                                                             T const&          value);
template <class InputIter, class Pred>
constexpr typename iterator_traits<InputIter>::difference_type count_if(InputIter first,
                                                                        InputIter last,
                                                                        Pred      pred);
template <class ExecutionPolicy, class ForwardIter, class Pred>
typename iterator_traits<ForwardIter>::difference_type count_if(ExecutionPolicy&& exec,
                                                                ForwardIter       first,
                                                                ForwardIter       last,
                                                                Pred              pred);

// mismatch
template <class InputIter1, class InputIter2>
constexpr pair<InputIter1, InputIter2> mismatch(InputIter1 first1, InputIter1 last1,
                                                InputIter2 first2);
template <class InputIter1, class InputIter2, class BinaryPred>
constexpr pair<InputIter1, InputIter2> mismatch(InputIter1 first1, InputIter1 last1,
                                                InputIter2 first2, BinaryPred pred);
template <class InputIter1, class InputIter2>
constexpr pair<InputIter1, InputIter2> mismatch(InputIter1 first1, InputIter1 last1,
                                                InputIter2 first2, InputIter2 last2);
template <class InputIter1, class InputIter2, class BinaryPred>
constexpr pair<InputIter1, InputIter2> mismatch(InputIter1 first1, InputIter1 last1,
                                                InputIter2 first2, InputIter2 last2,
                                                BinaryPred pred);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
pair<ForwardIter1, ForwardIter2> mismatch(ExecutionPolicy&& exec, ForwardIter1 first1,
                                          ForwardIter1 last1, ForwardIter2 first2);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class BinaryPred>
pair<ForwardIter1, ForwardIter2> mismatch(ExecutionPolicy&& exec, ForwardIter1 first1,
                                          ForwardIter1 last1, ForwardIter2 first2,
                                          BinaryPred pred);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
pair<ForwardIter1, ForwardIter2> mismatch(ExecutionPolicy&& exec, ForwardIter1 first1,
                                          ForwardIter1 last1, ForwardIter2 first2,
                                          ForwardIter2 last2);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class BinaryPred>
pair<ForwardIter1, ForwardIter2> mismatch(ExecutionPolicy&& exec, ForwardIter1 first1,
                                          ForwardIter1 last1, ForwardIter2 first2,
                                          ForwardIter2 last2, BinaryPred pred);

// equal
template <class InputIter1, class InputIter2>
constexpr bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2);
template <class InputIter1, class InputIter2, class BinaryPred>
constexpr bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                     BinaryPred pred);
template <class InputIter1, class InputIter2>
constexpr bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                     InputIter2 last2);
template <class InputIter1, class InputIter2, class BinaryPred>
constexpr bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                     InputIter2 last2, BinaryPred pred);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
bool equal(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
           ForwardIter2 first2);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class BinaryPred>
bool equal(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
           ForwardIter2 first2, BinaryPred pred);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
bool equal(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
           ForwardIter2 first2, ForwardIter2 last2);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class BinaryPred>
bool equal(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
           ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred);

// is permutation
template <class ForwardIter1, class ForwardIter2>
constexpr bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2);
template <class ForwardIter1, class ForwardIter2, class BinaryPred>
constexpr bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, BinaryPred pred);
template <class ForwardIter1, class ForwardIter2>
constexpr bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, ForwardIter2 last2);
template <class ForwardIter1, class ForwardIter2, class BinaryPred>
constexpr bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred);

// search
template <class ForwardIter1, class ForwardIter2>
constexpr ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, ForwardIter2 last2);
template <class ForwardIter1, class ForwardIter2, class BinaryPred>
constexpr ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
                              ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2>
ForwardIter1 search(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
                    ForwardIter2 first2, ForwardIter2 last2);
template <class ExecutionPolicy, class ForwardIter1, class ForwardIter2, class BinaryPred>
ForwardIter1 search(ExecutionPolicy&& exec, ForwardIter1 first1, ForwardIter1 last1,
                    ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred);

template <class ForwardIter, class Size,
          class T = typename iterator_traits<ForwardIter>::value_type>
constexpr ForwardIter search_n(ForwardIter first, ForwardIter last, Size count,
                               T const& value);
template <class ForwardIter, class Size,
          class T = typename iterator_traits<ForwardIter>::value_type, class BinaryPred>
constexpr ForwardIter search_n(ForwardIter first, ForwardIter last, Size count,
                               T const& value, BinaryPred pred);
template <class ExecutionPolicy, class ForwardIter, class Size,
          class T = typename iterator_traits<ForwardIter>::value_type>
ForwardIter search_n(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                     Size count, T const& value);
template <class ExecutionPolicy, class ForwardIter, class Size,
          class T = typename iterator_traits<ForwardIter>::value_type, class BinaryPred>
ForwardIter search_n(ExecutionPolicy&& exec, ForwardIter first, ForwardIter last,
                     Size count, T const& value, BinaryPred pred);

template <class ForwardIter, class Searcher>
constexpr ForwardIter search(ForwardIter first, ForwardIter last,
                             Searcher const& searcher);

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
    class ExecutionPolicy, class ForwardIt1, class ForwardIt2, class UnaryOp,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
ForwardIt2 transform(ExecutionPolicy&& policy, ForwardIt1 first1, ForwardIt1 last1,
                     ForwardIt2 d_first, UnaryOp unary_op)
{
	if constexpr (execution::is_stl_v<ExecutionPolicy>) {
		return std::transform(execution::to_stl_t<ExecutionPolicy>(), first1, last1, d_first,
		                      unary_op);
	}
#if defined(UFO_PAR_GCD)
	else if constexpr (execution::is_gcd_v<ExecutionPolicy>) {
		// TODO: Implement
	}
#endif
#if defined(UFO_PAR_TBB)
	else if constexpr (execution::is_tbb_v<ExecutionPolicy>) {
		// TODO: Implement
	}
#endif
	else if constexpr (execution::is_omp_v<ExecutionPolicy>) {
		// TODO: Implement
	} else {
		static_assert(dependent_false_v<ExecutionPolicy>,
		              "create not implemented for the execution policy");
	}
}

template <class InputIt1, class InputIt2, class OutputIt, class BinaryOp>
OutputIt transform(InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt d_first,
                   BinaryOp binary_op)
{
	return std::transform(first1, last1, first2, d_first, binary_op);
}

template <
    class ExecutionPolicy, class ForwardIt1, class ForwardIt2, class ForwardIt3,
    class BinaryOp,
    std::enable_if_t<execution::is_execution_policy_v<ExecutionPolicy>, bool> = true>
ForwardIt3 transform(ExecutionPolicy&& policy, ForwardIt1 first1, ForwardIt1 last1,
                     ForwardIt2 first2, ForwardIt3 d_first, BinaryOp binary_op)
{
	if constexpr (execution::is_stl_v<ExecutionPolicy>) {
		return std::transform(execution::to_stl_t<ExecutionPolicy>(), first1, last1, first2,
		                      d_first, binary_op);
	}
#if defined(UFO_PAR_GCD)
	else if constexpr (execution::is_gcd_v<ExecutionPolicy>) {
		// TODO: Implement
	}
#endif
#if defined(UFO_PAR_TBB)
	else if constexpr (execution::is_tbb_v<ExecutionPolicy>) {
		// TODO: Implement
	}
#endif
	else if constexpr (execution::is_omp_v<ExecutionPolicy>) {
		// TODO: Implement
	} else {
		static_assert(dependent_false_v<ExecutionPolicy>,
		              "create not implemented for the execution policy");
	}
}
}  // namespace ufo

#endif  // UFO_EXECUTION_ALGORITHM_HPP