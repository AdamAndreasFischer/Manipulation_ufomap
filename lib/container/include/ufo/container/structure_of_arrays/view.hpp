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

#ifndef UFO_CONTAINER_STRUCTURE_OF_ARRAYS_VIEW_HPP
#define UFO_CONTAINER_STRUCTURE_OF_ARRAYS_VIEW_HPP

// STL
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <vector>

namespace ufo
{
// TODO: Make it so you can convert from non-const to const

template <class T>
class SoAView
{
	template <class... Ts>
	friend class SoA;

 public:
	/**************************************************************************************
	|                                                                                     |
	|                                        Tags                                         |
	|                                                                                     |
	**************************************************************************************/

	using element_type           = T;
	using value_type             = std::remove_cv_t<T>;
	using size_type              = std::size_t;
	using difference_type        = std::ptrdiff_t;
	using pointer                = T*;
	using const_pointer          = T const*;
	using reference              = T&;
	using const_reference        = T const&;
	using iterator               = std::conditional_t<std::is_const_v<T>,
	                                                  typename std::vector<value_type>::const_iterator,
	                                                  typename std::vector<value_type>::iterator>;
	using const_iterator         = typename std::vector<value_type>::const_iterator;
	using reverse_iterator       = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	/**************************************************************************************
	|                                                                                     |
	|                                    Constructors                                     |
	|                                                                                     |
	**************************************************************************************/

	SoAView() = default;

	/**************************************************************************************
	|                                                                                     |
	|                                      Iterators                                      |
	|                                                                                     |
	**************************************************************************************/

	iterator begin() const noexcept { return data_->begin(); }

	const_iterator cbegin() const noexcept { return data_->cbegin(); }

	iterator end() const noexcept { return data_->end(); }

	const_iterator cend() const noexcept { return data_->cend(); }

	reverse_iterator rbegin() const noexcept { return data_->rbegin(); }

	const_reverse_iterator crbegin() const noexcept { return data_->crbegin(); }

	reverse_iterator rend() const noexcept { return data_->rend(); }

	const_reverse_iterator crend() const noexcept { return data_->crend(); }

	/**************************************************************************************
	|                                                                                     |
	|                                   Element access                                    |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] reference front() const { return data_->front(); }

	[[nodiscard]] reference back() const { return data_->back(); }

	[[nodiscard]] reference at(size_type pos) const { return data_->at(pos); }

	[[nodiscard]] reference operator[](size_type idx) const { return (*data_)[idx]; }

	[[nodiscard]] pointer data() const noexcept { return data_->data(); }

	/**************************************************************************************
	|                                                                                     |
	|                                      Observers                                      |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] size_type size() const noexcept { return data_->size(); }

	[[nodiscard]] bool empty() const noexcept { return data_->empty(); }

 private:
	/**************************************************************************************
	|                                                                                     |
	|                                    Constructors                                     |
	|                                                                                     |
	**************************************************************************************/

	SoAView(std::conditional_t<std::is_const_v<T>, std::vector<value_type> const&,
	                           std::vector<value_type>&>
	            data)
	    : data_(&data)
	{
	}

 private:
	std::conditional_t<std::is_const_v<T>, std::vector<value_type> const*,
	                   std::vector<value_type>*>
	    data_ = nullptr;
};
}  // namespace ufo

#endif  // UFO_CONTAINER_STRUCTURE_OF_ARRAYS_VIEW_HPP