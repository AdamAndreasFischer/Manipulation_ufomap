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

#ifndef UFO_UTILITY_TYPE_TRAITS
#define UFO_UTILITY_TYPE_TRAITS

// STL
#include <cstddef>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ufo
{
//
// Dependent false
//

template <typename... T>
struct dependent_false {
	static constexpr bool value = false;
};

template <class... T>
constexpr inline bool dependent_false_v = dependent_false<T...>::value;

//
// Argument helper
//

template <std::size_t ArgNum, typename Arg, typename... Rest>
struct argument_helper_2 : argument_helper_2<ArgNum - 1, Rest...> {
};

template <typename Arg, typename... Rest>
struct argument_helper_2<0, Arg, Rest...> {
	using type = Arg;
};

template <std::size_t ArgNum, typename... Args>
using argument_helper_2_t = typename argument_helper_2<ArgNum, Args...>::type;

// Specialization for 'auto' argument type
template <std::size_t ArgNum, typename... T>
typename std::enable_if_t<ArgNum <= sizeof...(T)> argument_helper(T...);

// Specialization for function pointer
template <std::size_t ArgNum, typename Ret, typename... Args>
argument_helper_2_t<ArgNum, Args...> argument_helper(Ret (*)(Args...));

// Specialization for functor and lambda
template <std::size_t ArgNum, typename Ret, typename F, typename... Args>
argument_helper_2_t<ArgNum, Args...> argument_helper(Ret (F::*)(Args...));

// Specialization for functor and lambda
template <std::size_t ArgNum, typename Ret, typename F, typename... Args>
argument_helper_2_t<ArgNum, Args...> argument_helper(Ret (F::*)(Args...) const);

// Specialization for functor and lambda
template <std::size_t ArgNum, typename F>
decltype(argument_helper<ArgNum>(&F::operator())) argument_helper(F);

/*!
 * @brief Get the argument type ArgNum argument of function F.
 *
 */
template <typename F, std::size_t ArgNum>
using argument = decltype(argument_helper<ArgNum>(std::declval<F>()));

//
// Is specialization of
//

template <template <class...> class T, class U>
struct is_specialization_of : std::false_type {
};

template <template <class...> class T, class... Us>
struct is_specialization_of<T, T<Us...>> : std::true_type {
};

template <template <class...> class T, class... Us>
constexpr inline bool is_specialization_of_v = is_specialization_of<T, Us...>::value;

//
// Is pair
//

template <class T>
struct is_pair : is_specialization_of<std::pair, T> {
};

template <class T>
constexpr inline bool is_pair_v = is_pair<T>::value;

//
// Is tuple
//

template <class T>
struct is_tuple : is_specialization_of<std::tuple, T> {
};

template <class T>
constexpr inline bool is_tuple_v = is_tuple<T>::value;

//
// Is unique
//

template <class...>
struct is_unique : std::true_type {
};

template <class T, class... Rest>
struct is_unique<T, Rest...> {
	static constexpr bool value =
	    (!std::is_same_v<T, Rest> && ...) && is_unique<Rest...>::value;
};

template <class T, class... Rest>
constexpr inline bool is_unique_v = is_unique<T, Rest...>::value;

//
// Contains type
//

template <class T, class... Ts>
struct contains_type : std::disjunction<std::is_same<T, Ts>...> {
};

template <class T, class... Ts>
constexpr inline bool contains_type_v = contains_type<T, Ts...>::value;

//
// Contains convertible type
//

template <class T, class... Ts>
struct contains_convertible_type : std::disjunction<std::is_convertible<T, Ts>...> {
};

template <class T, class... Ts>
constexpr inline bool contains_convertible_type_v =
    contains_convertible_type<T, Ts...>::value;

//
// Index type
//

template <std::size_t I, class... Ts>
struct index_type : std::tuple_element<I, std::tuple<Ts...>> {
};

template <std::size_t I, class... Ts>
using index_type_t = typename index_type<I, Ts...>::type;

//
// Remove const, volatile, and reference
//

#if __cplusplus >= 201711L
using std::remove_cvref;
using std::remove_cvref_t;
#else
template <class T>
struct remove_cvref {
	using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;
#endif

//
// Index
//

template <class...>
struct index;

template <class T, class... R>
struct index<T, T, R...> : std::integral_constant<std::size_t, 0> {
};

template <class T, class F, class... R>
struct index<T, F, R...>
    : std::integral_constant<std::size_t, 1 + index<T, R...>::value> {
};

template <class T, class... Types>
constexpr inline std::size_t index_v = index<T, Types...>::value;

//
// String
//

template <class T>
struct is_string
    : public std::disjunction<std::is_same<char*, remove_cvref_t<T>>,
                              std::is_same<char const*, remove_cvref_t<T>>,
                              std::is_same<std::string, remove_cvref_t<T>>,
                              std::is_same<std::string_view, remove_cvref_t<T>>> {
};

template <class T>
constexpr inline bool is_string_v = is_string<T>::value;
}  // namespace ufo

#endif  // UFO_UTILITY_TYPE_TRAITS