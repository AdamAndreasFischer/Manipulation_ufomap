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

#ifndef UFO_CONTAINER_STRUCTURE_OF_ARRAYS_VALUE_HPP
#define UFO_CONTAINER_STRUCTURE_OF_ARRAYS_VALUE_HPP

// UFO
#include <ufo/utility/type_traits.hpp>

// STL
#include <cstddef>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ufo
{
template <class... Ts>
class SoAValue
{
 public:
	SoAValue() = default;

	SoAValue(Ts const&... ts) : data_(ts...) {}

	template <class T, std::enable_if_t<contains_type_v<T, Ts...>, bool> = true>
	operator T&()
	{
		return get<T>();
	}

	template <class T, std::enable_if_t<contains_type_v<T, Ts...>, bool> = true>
	operator T const&() const
	{
		return get<T>();
	}

	template <class T, std::enable_if_t<contains_type_v<T, Ts...>, bool> = true>
	SoAValue& operator=(T const& rhs)
	{
		get<T>() = rhs;
		return *this;
	}

	template <class T, std::enable_if_t<contains_type_v<T, Ts...>, bool> = true>
	[[nodiscard]] T& get()
	{
		return std::get<T>(data_);
	}

	template <class T, std::enable_if_t<contains_type_v<T, Ts...>, bool> = true>
	[[nodiscard]] T const& get() const
	{
		return std::get<T>(data_);
	}

	template <std::size_t I>
	auto&& get() &
	{
		return get_helper<I>(*this);
	}

	template <std::size_t I>
	auto&& get() &&
	{
		return get_helper<I>(*this);
	}

	template <std::size_t I>
	auto&& get() const&
	{
		return get_helper<I>(*this);
	}

	template <std::size_t I>
	auto&& get() const&&
	{
		return get_helper<I>(*this);
	}

 private:
	template <std::size_t I, class T>
	auto&& get_helper(T&& t)
	{
		return std::get<I>(std::forward<T>(t).data_);
	}

 private:
	std::tuple<Ts...> data_;
};

template <class T, class... Ts>
std::ostream& operator<<(std::ostream& os, SoAValue<T, Ts...> const& value)
{
	os << value.template get<T>();
	((os << ", " << value.template get<Ts>()), ...);
	return os;
}

//
// Type traits
//

template <class T, class... Ts>
struct contains_type<T, SoAValue<Ts...>> : contains_type<T, Ts...> {
};
}  // namespace ufo

namespace std
{
template <class... Ts>
struct tuple_size<ufo::SoAValue<Ts...>> : integral_constant<size_t, sizeof...(Ts)> {
};

template <size_t Index, class... Ts>
struct tuple_element<Index, ufo::SoAValue<Ts...>> : tuple_element<Index, tuple<Ts...>> {
};
}  // namespace std

#endif  // UFO_CONTAINER_STRUCTURE_OF_ARRAYS_VALUE_HPP