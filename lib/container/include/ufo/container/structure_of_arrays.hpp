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

#ifndef UFO_CONTAINER_STRUCTURE_OF_ARRAYS_HPP
#define UFO_CONTAINER_STRUCTURE_OF_ARRAYS_HPP

// UFO
#include <ufo/container/structure_of_arrays/element.hpp>
#include <ufo/container/structure_of_arrays/value.hpp>
#include <ufo/container/structure_of_arrays/view.hpp>
#include <ufo/utility/proxy_arrow_result.hpp>
#include <ufo/utility/type_traits.hpp>

// STL
#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

namespace ufo
{
template <class... Ts>
class SoA
{
	static_assert(0 < sizeof...(Ts));

	friend SoAElement<Ts...>;

 public:
	/**************************************************************************************
	|                                                                                     |
	|                                        Tags                                         |
	|                                                                                     |
	**************************************************************************************/

	using data_type              = std::tuple<std::vector<Ts>...>;
	using value_type             = SoAElement<Ts...>;
	using size_type              = std::size_t;
	using difference_type        = std::ptrdiff_t;
	using reference              = value_type&;
	using const_reference        = value_type const&;
	using pointer                = value_type*;
	using const_pointer          = value_type const*;
	using iterator               = pointer;
	using const_iterator         = const_pointer;
	using reverse_iterator       = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	/**************************************************************************************
	|                                                                                     |
	|                                    Constructors                                     |
	|                                                                                     |
	**************************************************************************************/

	SoA() = default;

	explicit SoA(size_type count) { resize(count); }

	SoA(size_type count, value_type const& value) { resize(count, value); }

	SoA(size_type count, SoAValue<Ts...> const& value) { resize(count, value); }

	template <class InputIt>
	SoA(InputIt first, InputIt last)
	{
		insert(begin(), first, last);
	}

	SoA(SoA const& other) : data_(other.data_) { create_elements(); }

	SoA(SoA&& other) noexcept = default;

	SoA(std::initializer_list<value_type> init) : SoA(init.begin(), init.end()) {}

	SoA(std::initializer_list<SoAValue<Ts...>> init) : SoA(init.begin(), init.end()) {}

	/**************************************************************************************
	|                                                                                     |
	|                                     Destructor                                      |
	|                                                                                     |
	**************************************************************************************/

	~SoA() = default;

	/**************************************************************************************
	|                                                                                     |
	|                                 Assignment operator                                 |
	|                                                                                     |
	**************************************************************************************/

	SoA& operator=(SoA const& rhs)
	{
		data_ = rhs.data_;
		create_elements();
		return *this;
	}

	SoA& operator=(SoA&& rhs) = default;

	SoA& operator=(std::initializer_list<value_type> ilist)
	{
		clear();
		insert(begin(), ilist.begin(), ilist.end());
		return *this;
	}

	SoA& operator=(std::initializer_list<SoAValue<Ts...>> ilist)
	{
		clear();
		insert(begin(), ilist.begin(), ilist.end());
		return *this;
	}

	/**************************************************************************************
	|                                                                                     |
	|                                       Assign                                        |
	|                                                                                     |
	**************************************************************************************/

	// void assign(size_type count, value_type const& value)
	// {
	// 	// TODO: Implement
	// 	clear();
	// 	reserve(count);
	// 	for (std::size_t i{}; count > i; ++i) {
	// 		push_back(value);
	// 	}
	// }

	// void assign(size_type count, SoAValue<Ts...> const& value)
	// {
	// 	// TODO: Implement
	// 	clear();
	// 	reserve(count);
	// 	for (std::size_t i{}; count > i; ++i) {
	// 		push_back(value);
	// 	}
	// }

	// template <class InputIt>
	// void assign(InputIt first, InputIt last)
	// {
	// 	// TODO: Implement
	// 	clear();
	// 	reserve(std::distance(first, last));
	// 	for (; first != last; ++first) {
	// 		push_back(*first);
	// 	}
	// }

	// void assign(std::initializer_list<value_type> ilist)
	// {
	// 	assign(std::begin(ilist), std::end(ilist));
	// }

	// void assign(std::initializer_list<SoAValue<Ts...>> ilist)
	// {
	// 	assign(std::begin(ilist), std::end(ilist));
	// }

	/**************************************************************************************
	|                                                                                     |
	|                                        View                                         |
	|                                                                                     |
	**************************************************************************************/

	template <class T>
	[[nodiscard]] SoAView<T> view()
	{
		return SoAView<T>(std::get<std::vector<T>>(data_));
	}

	template <class T>
	[[nodiscard]] SoAView<T const> view() const
	{
		return SoAView<T const>(std::get<std::vector<T>>(data_));
	}

	template <std::size_t I>
	[[nodiscard]] SoAView<index_type_t<I, Ts...>> view()
	{
		return SoAView<index_type_t<I, Ts...>>(std::get<I>(data_));
	}

	template <std::size_t I>
	[[nodiscard]] SoAView<index_type_t<I, Ts...> const> view() const
	{
		return SoAView<index_type_t<I, Ts...> const>(std::get<I>(data_));
	}

	/**************************************************************************************
	|                                                                                     |
	|                                   Element access                                    |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] reference at(size_type pos)
	{
		if (size() <= pos) {
			// FIXME: Improve message
			std::out_of_range("Out of range");
		}
		return operator[](pos);
	}

	[[nodiscard]] const_reference at(size_type pos) const
	{
		if (size() <= pos) {
			// FIXME: Improve message
			std::out_of_range("Out of range");
		}
		return operator[](pos);
	}

	[[nodiscard]] reference operator[](size_type pos) { return elements_[pos]; }

	[[nodiscard]] const_reference operator[](size_type pos) const { return elements_[pos]; }

	[[nodiscard]] reference front() { return operator[](0u); }

	[[nodiscard]] const_reference front() const { return operator[](0u); }

	[[nodiscard]] reference back() { return operator[](size() - 1); }

	[[nodiscard]] const_reference back() const { return operator[](size() - 1); }

	[[nodiscard]] data_type* data() { return &data_; }

	[[nodiscard]] data_type const* data() const { return &data_; }

	/**************************************************************************************
	|                                                                                     |
	|                                      Iterators                                      |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] iterator begin() { return elements_.get(); }

	[[nodiscard]] const_iterator begin() const { return elements_.get(); }

	[[nodiscard]] const_iterator cbegin() const { return begin(); }

	[[nodiscard]] iterator end() { return begin() + size(); }

	[[nodiscard]] const_iterator end() const { return begin() + size(); }

	[[nodiscard]] const_iterator cend() const { end(); }

	// TODO: Implement below

	// [[nodiscard]] reverse_iterator rbegin() { return  elements_.rbegin(); }

	// [[nodiscard]] const_reverse_iterator rbegin() const { return elements_.rbegin(); }

	// [[nodiscard]] const_reverse_iterator crbegin() const { return rbegin(); }

	// [[nodiscard]] reverse_iterator rend() { return elements_.rend(); }

	// [[nodiscard]] const_reverse_iterator rend() const { return elements_.rend(); }

	// [[nodiscard]] const_reverse_iterator crend() const { rend(); }

	/**************************************************************************************
	|                                                                                     |
	|                                      Capacity                                       |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] bool empty() const noexcept { return std::get<0>(data_).empty(); }

	[[nodiscard]] size_type size() const noexcept { return std::get<0>(data_).size(); }

	[[nodiscard]] size_type max_size() const noexcept
	{
		return std::get<0>(data_).max_size();
	}

	void reserve(size_type new_cap)
	{
		(std::get<std::vector<Ts>>(data_).reserve(new_cap), ...);
		create_elements();
	}

	[[nodiscard]] size_type capacity() const noexcept
	{
		std::array cap{std::get<std::vector<Ts>>(data_).capacity()...};
		return *std::min_element(cap.begin(), cap.end());
	}

	void shrink_to_fit()
	{
		(std::get<std::vector<Ts>>(data_).shrink_to_fit(), ...);
		create_elements();
	}

	/**************************************************************************************
	|                                                                                     |
	|                                      Modifiers                                      |
	|                                                                                     |
	**************************************************************************************/

	void clear()
	{
		(std::get<std::vector<Ts>>(data_).clear(), ...);
		create_elements();
	}

	// iterator insert(const_iterator pos, value_type const& value)
	// {
	//   // TODO: Implement
	// }

	// iterator insert(const_iterator pos, SoAValue<Ts...> const& value)
	// {
	// 	// TODO: Implement
	// }

	// iterator insert(const_iterator pos, value_type&& value)
	// {
	// 	// TODO: Implement
	// }

	// iterator insert(const_iterator pos, SoAValue<Ts...>&& value)
	// {
	// 	// TODO: Implement
	// }

	// iterator insert(const_iterator pos, size_type count, value_type const& value)
	// {
	// 	// TODO: Implement
	// }

	// iterator insert(const_iterator pos, size_type count, SoAValue<Ts...> const& value)
	// {
	// 	// TODO: Implement
	// }

	// template <class InputIt>
	// iterator insert(const_iterator pos, InputIt first, InputIt last)
	// {
	// 	// TODO: Implement
	// }

	// iterator insert(const_iterator pos, std::initializer_list<value_type> ilist)
	// {
	// 	// TODO: Implement
	// }

	// iterator insert(const_iterator pos, std::initializer_list<SoAValue<Ts...>> ilist)
	// {
	// 	// TODO: Implement
	// }

	// iterator emplace(const_iterator pos, Ts const&... ts)
	// {
	// 	// TODO: Implement
	// }

	// iterator emplace(const_iterator pos, Ts&&... ts)
	// {
	// 	// TODO: Implement
	// }

	iterator erase(const_iterator pos)
	{
		auto const idx = std::distance(cbegin(), pos);
		(std::get<std::vector<Ts>>(data_).erase(std::get<std::vector<Ts>>(data_).begin() +
		                                        idx),
		 ...);
		create_elements();
		return begin() + idx;
	}

	iterator erase(const_iterator first, const_iterator last)
	{
		auto const f_idx = std::distance(cbegin(), first);
		auto const l_idx = std::distance(cbegin(), last);
		(std::get<std::vector<Ts>>(data_).erase(
		     std::get<std::vector<Ts>>(data_).begin() + f_idx,
		     std::get<std::vector<Ts>>(data_).begin() + l_idx),
		 ...);
		create_elements();
		return begin() + f_idx;
	}

	size_type erase(value_type const& value)
	{
		// TODO: Implement
		auto it = std::remove(begin(), end(), value);
		auto r  = end() - it;
		erase(it, end());
		return r;
	}

	size_type erase(SoAValue<Ts...> const& value)
	{
		// TODO: Implement
		auto it = std::remove(begin(), end(), value);
		auto r  = end() - it;
		erase(it, end());
		return r;
	}

	template <class T, std::enable_if_t<contains_type_v<T, Ts...>, bool> = true>
	size_type erase(T const& value)
	{
		// TODO: Implement
		return erase_if(
		    [&value](auto&& element) { return value == element.template get<T>(); });
	}

	template <class Pred>
	size_type erase_if(Pred pred)
	{
		// TODO: Implement
		auto it = std::remove_if(begin(), end(), pred);
		auto r  = end() - it;
		erase(it, end());
		return r;
	}

	void push_back(value_type const& value)
	{
		std::apply(
		    [&value](auto&&... data) {
			    (data.push_back(value.template get<decltype(data)::value_type>()), ...);
		    },
		    data_);
		create_elements();
	}

	void push_back(SoAValue<Ts...> const& value)
	{
		std::apply(
		    [&value](auto&&... data) {
			    (data.push_back(value.template get<decltype(data)::value_type>()), ...);
		    },
		    data_);
		create_elements();
	}

	void push_back(value_type&& value)
	{
		// FIXME: Is it correct?
		std::apply(
		    [&value](auto&&... data) {
			    (data.push_back(std::move(value.template get<decltype(data)::value_type>())),
			     ...);
		    },
		    data_);
		create_elements();
	}

	void push_back(SoAValue<Ts...>&& value)
	{
		// FIXME: Is it correct?
		std::apply(
		    [&value](auto&&... data) {
			    (data.push_back(std::move(value.template get<decltype(data)::value_type>())),
			     ...);
		    },
		    data_);
		create_elements();
	}

	reference emplace_back(Ts const&... ts)
	{
		std::apply([&ts...](auto&&... data) { (data.push_back(ts), ...); }, data_);
		create_elements();
		return back();
	}

	reference emplace_back(Ts&&... ts)
	{
		// FIXME: Is it correct?
		std::apply([&ts...](auto&&... data) { (data.push_back(std::move(ts)), ...); }, data_);
		create_elements();
		return back();
	}

	void pop_back()
	{
		(std::get<std::vector<Ts>>(data_).pop_back(), ...);
		create_elements();
	}

	void resize(size_type count)
	{
		(std::get<std::vector<Ts>>(data_).resize(count), ...);
		create_elements();
	}

	void resize(size_type count, value_type const& value)
	{
		(std::get<std::vector<Ts>>(data_).resize(count, value.template get<Ts>()), ...);
		create_elements();
	}

	void swap(SoA& other)
	{
		(std::get<std::vector<Ts>>(data_).swap(std::get<std::vector<Ts>>(other.data_)), ...);
		create_elements();
		other.create_elements();
	}

 private:
	void create_elements()
	{
		auto const cap = capacity();
		if (0 == cap) {
			elements_     = nullptr;
			elements_cap_ = 0;
		} else if (elements_cap_ != cap) {
			elements_     = std::unique_ptr<value_type[]>(new value_type[cap]);
			elements_cap_ = cap;
			for (std::size_t i{}; cap > i; ++i) {
				elements_[i].reference_    = this;
				elements_[i].is_reference_ = true;
			}
		}
	}

 private:
	std::unique_ptr<value_type[]> elements_;
	std::size_t                   elements_cap_ = {};
	data_type                     data_;
};

//
// Type traits
//

template <class T, class... Ts>
struct contains_type<T, SoA<Ts...>> : contains_type<T, Ts...> {
};
}  // namespace ufo

#endif  // UFO_CONTAINER_STRUCTURE_OF_ARRAYS_HPP