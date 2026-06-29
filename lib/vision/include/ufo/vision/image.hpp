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

#ifndef UFO_VISION_IMAGE_HPP
#define UFO_VISION_IMAGE_HPP

// std
#include <cstddef>
#include <vector>

namespace ufo
{
template <class T>
class Image
{
 public:
	using value_type = T;

	Image() : rows_(0), cols_(0) {}

	Image(std::size_t rows, std::size_t cols, T const& value = T())
	    : rows_(rows), cols_(cols), data_(rows * cols, value)
	{
	}

	[[nodiscard]] T& operator[](std::size_t index) { return data_[index]; }

	[[nodiscard]] T operator[](std::size_t index) const { return data_[index]; }

	[[nodiscard]] T& operator()(std::size_t row, std::size_t col)
	{
		return data_[col + row * cols()];
	}

	[[nodiscard]] T const& operator()(std::size_t row, std::size_t col) const
	{
		return data_[col + row * cols()];
	}

	[[nodiscard]] T& at(std::size_t row, std::size_t col)
	{
		// TODO: Add bounds check
		return this->operator()(row, col);
	}

	[[nodiscard]] T const& at(std::size_t row, std::size_t col) const
	{
		// TODO: Add bounds check
		return this->operator()(row, col);
	}

	[[nodiscard]] auto begin() { return std::begin(data_); }

	[[nodiscard]] auto begin() const { return std::begin(data_); }

	[[nodiscard]] auto cbegin() const { return std::cbegin(data_); }

	[[nodiscard]] auto rbegin() { return std::rbegin(data_); }

	[[nodiscard]] auto rbegin() const { return std::rbegin(data_); }

	[[nodiscard]] auto crbegin() const { return std::crbegin(data_); }

	[[nodiscard]] auto end() { return std::end(data_); }

	[[nodiscard]] auto end() const { return std::end(data_); }

	[[nodiscard]] auto cend() const { return std::cend(data_); }

	[[nodiscard]] auto rend() { return std::rend(data_); }

	[[nodiscard]] auto rend() const { return std::rend(data_); }

	[[nodiscard]] auto crend() const { return std::crend(data_); }

	[[nodiscard]] T* data() { return data_.data(); }

	[[nodiscard]] T const* data() const { return data_.data(); }

	[[nodiscard]] bool empty() const { return data_.empty(); }

	[[nodiscard]] constexpr std::size_t rows() const noexcept { return rows_; }

	[[nodiscard]] constexpr std::size_t cols() const noexcept { return cols_; }

	[[nodiscard]] constexpr std::size_t size() const noexcept { return data_.size(); }

 private:
	// TODO: Make it work with bool. std::vector has specialization for bool.
	std::vector<T> data_;
	std::size_t    rows_;
	std::size_t    cols_;
};
}  // namespace ufo

#endif  // UFO_VISION_IMAGE_HPP