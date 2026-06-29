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

#ifndef UFO_CONTAINER_TREE_CODE_HPP
#define UFO_CONTAINER_TREE_CODE_HPP

// UFO
#include <ufo/container/tree/key.hpp>
#include <ufo/math/math.hpp>
#include <ufo/morton/morton.hpp>

// STL
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ufo
{
template <std::size_t Dim>
class TreeCode
{
 private:
	//
	// Friends
	//

	friend class std::hash<TreeCode>;

 public:
	using code_t    = std::uint32_t;
	using key_t     = typename TreeKey<Dim>::key_t;
	using depth_t   = std::uint32_t;
	using size_type = std::size_t;

 private:
	static constexpr code_t const DEPTHS_PER_IDX = Morton<Dim>::LEVELS_32;
	static constexpr code_t const OFFSET_MASK    = ~((~code_t(0)) << Dim);

	static constexpr std::array<bool, 3> const ACTIVE{
	    true, std::numeric_limits<key_t>::digits > DEPTHS_PER_IDX,
	    std::numeric_limits<key_t>::digits > 2 * DEPTHS_PER_IDX};

 public:
	/**************************************************************************************
	|                                                                                     |
	|                                    Constructors                                     |
	|                                                                                     |
	**************************************************************************************/

	constexpr TreeCode() noexcept                = default;
	constexpr TreeCode(TreeCode const&) noexcept = default;

	constexpr TreeCode(std::array<code_t, 3> const& code, depth_t const& depth)
	    : code_(code), depth_(depth)
	{
		assert(valid());
	}

	constexpr explicit TreeCode(std::array<code_t, 3> const& code) : TreeCode(code, 0) {}

	constexpr explicit TreeCode(TreeKey<Dim> const& key) : depth_(key.depth())
	{
		auto k = key << key.depth();

		code_[0] = Morton<Dim>::encode32(k);
		if constexpr (ACTIVE[1]) {
			code_[1] = Morton<Dim>::encode32(k >> DEPTHS_PER_IDX);
		}
		if constexpr (ACTIVE[2]) {
			code_[2] = Morton<Dim>::encode32(k >> (2 * DEPTHS_PER_IDX));
		}
	}

	[[nodiscard]] static constexpr TreeCode invalid() noexcept
	{
		TreeCode code;
		code.invalidate();
		return code;
	}

	/**************************************************************************************
	|                                                                                     |
	|                                 Assignment operator                                 |
	|                                                                                     |
	**************************************************************************************/

	constexpr TreeCode& operator=(TreeCode const&) noexcept = default;

	/**************************************************************************************
	|                                                                                     |
	|                                 Conversion operator                                 |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] constexpr explicit operator TreeKey<Dim>() const noexcept
	{
		typename TreeKey<Dim>::Key key = Morton<Dim>::decode32(code_[0]);

		if constexpr (ACTIVE[1]) {
			auto k = Morton<Dim>::decode32(code_[1]) << DEPTHS_PER_IDX;
			for (std::size_t j{}; Dim > j; ++j) {
				key[j] |= k[j];
			}
		}
		if constexpr (ACTIVE[2]) {
			auto k = Morton<Dim>::decode32(code_[2]) << (2 * DEPTHS_PER_IDX);
			for (std::size_t j{}; Dim > j; ++j) {
				key[j] |= k[j];
			}
		}

		return TreeKey<Dim>(key >> depth_, depth_);
	}

	/**************************************************************************************
	|                                                                                     |
	|                                   Element access                                    |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] constexpr key_t operator[](size_type pos) const noexcept
	{
		assert(size() > pos);

		key_t k = Morton<Dim>::decode32(code_[0], pos);

		if constexpr (ACTIVE[1]) {
			k |= Morton<Dim>::decode32(code_[1], pos) << DEPTHS_PER_IDX;
		}
		if constexpr (ACTIVE[2]) {
			k |= Morton<Dim>::decode32(code_[2], pos) << (2 * DEPTHS_PER_IDX);
		}

		return k >> depth_;
	}

	/**************************************************************************************
	|                                                                                     |
	|                                      Capacity                                       |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] static constexpr size_type size() noexcept { return Dim; }

	/**************************************************************************************
	|                                                                                     |
	|                                     Operations                                      |
	|                                                                                     |
	**************************************************************************************/

	[[nodiscard]] constexpr bool valid() const noexcept
	{
		bool a = maxDepth() >= depth();
		// TODO: Implement
		// // Check if unused upper bits are set to zero
		// bool b = 0 == (code() >> (Dim * maxDepth()));
		// // Check if unused lower bits are set to zero
		// bool c = code() == ((code() >> (Dim * depth())) << (Dim * depth()));

		// return a && b && c;
		return a;
	}

	constexpr void invalidate() noexcept { depth_ = maxDepth() + 1; }

	[[nodiscard]] static constexpr std::size_t branchingFactor() noexcept
	{
		return ipow(std::size_t(2), static_cast<int>(Dim));
	}

	// [[nodiscard]] constexpr code_t code() const noexcept { return code_; }

	[[nodiscard]] static constexpr depth_t maxDepth() noexcept
	{
		depth_t max_depth = DEPTHS_PER_IDX;
		if constexpr (ACTIVE[1]) {
			max_depth += DEPTHS_PER_IDX;
		}
		if constexpr (ACTIVE[2]) {
			max_depth += DEPTHS_PER_IDX;
		}
		return max_depth;
	}

	[[nodiscard]] constexpr depth_t depth() const noexcept { return depth_; }

	[[nodiscard]] static constexpr bool equalAtDepth(TreeCode const& lhs,
	                                                 TreeCode const& rhs,
	                                                 depth_t         depth) noexcept
	{
		assert(maxDepth() >= depth);
		assert(lhs.valid() && rhs.valid());

		auto i = depth / DEPTHS_PER_IDX;
		auto d = depth % DEPTHS_PER_IDX;

		std::array<code_t, 3> m{0 >= i ? ~code_t(0) : code_t(0),
		                        1 >= i ? ~code_t(0) : code_t(0),
		                        2 >= i ? ~code_t(0) : code_t(0)};

		m[i] <<= Dim * d;

		return (lhs.code_[0] & m[0]) == (rhs.code_[0] & m[0]) &&
		       (lhs.code_[1] & m[1]) == (rhs.code_[1] & m[1]) &&
		       (lhs.code_[2] & m[2]) == (rhs.code_[2] & m[2]);
	}

	[[nodiscard]] static constexpr depth_t depthWhereEqual(TreeCode const& lhs,
	                                                       TreeCode const& rhs) noexcept
	{
		assert(lhs.valid() && rhs.valid());

		auto depth =
		    lhs.code_[2] != rhs.code_[2]
		        ? 2 * DEPTHS_PER_IDX
		        : static_cast<depth_t>(lhs.code_[1] != rhs.code_[1]) * DEPTHS_PER_IDX;
		depth = std::max(depth, std::max(lhs.depth(), rhs.depth()));

		auto   i = depth / DEPTHS_PER_IDX;
		auto   d = depth % DEPTHS_PER_IDX;
		code_t c = (lhs.code_[i] ^ rhs.code_[i]) >> (Dim * d);
		for (; c; c >>= Dim, ++depth) {
		}

		return depth;
	}

	[[nodiscard]] constexpr TreeCode toDepth(depth_t depth) const
	{
		assert(maxDepth() >= depth);

		TreeCode ret = *this;
		ret.setDepth(depth);
		return ret;
	}

	constexpr void setDepth(depth_t depth)
	{
		assert(maxDepth() >= depth);

		auto i = depth / DEPTHS_PER_IDX;
		auto d = depth % DEPTHS_PER_IDX;

		std::array<code_t, 3> m{0 >= i ? ~code_t(0) : code_t(0),
		                        1 >= i ? ~code_t(0) : code_t(0),
		                        2 >= i ? ~code_t(0) : code_t(0)};

		m[i] <<= Dim * d;

		code_[0] &= m[0];
		code_[1] &= m[1];
		code_[2] &= m[2];
		depth_ = depth;
	}

	/*!
	 * @brief Get the offset at the current depth (same as `c.offset(c.depth())`).
	 *
	 * @return The offset at the current depth.
	 */
	[[nodiscard]] constexpr code_t offset() const { return offset(depth_); }

	/*!
	 * @brief Get the offset at a specific depth for this code.
	 *
	 * @param depth The depth the index is requested for.
	 * @return The offset at the specified depth.
	 */
	[[nodiscard]] constexpr code_t offset(depth_t depth) const
	{
		assert(maxDepth() >= depth);

		auto i = depth / DEPTHS_PER_IDX;
		auto d = depth % DEPTHS_PER_IDX;
		return (code_[i] >> (Dim * d)) & OFFSET_MASK;
	}

	[[nodiscard]] constexpr TreeCode parent() const { return toDepth(depth_ + 1); }

	/*!
	 * @brief Get the code of a specific child of this code
	 *
	 * @param idx The index of the child
	 * @return TreeCode The child code
	 */
	[[nodiscard]] constexpr TreeCode child(std::size_t idx) const
	{
		assert(0 < depth_);
		assert(branchingFactor() > idx);

		return firstborn().firstbornSibling(idx);
	}

	/*!
	 * @brief Get the code of the firstborn child of this code (same as child(0)).
	 *
	 * @return TreeCode The firstborn child code
	 */
	[[nodiscard]] constexpr TreeCode firstborn() const
	{
		assert(0 < depth_);
		return TreeCode(code_, depth_ - 1);
	}

	/*!
	 * @brief Get the code of a specific sibling of this code
	 *
	 * @param idx The index of the sibling
	 * @return TreeCode The sibling code
	 */
	[[nodiscard]] constexpr TreeCode sibling(std::size_t idx) const
	{
		assert(branchingFactor() > idx);

		TreeCode ret = *this;

		auto i = depth_ / DEPTHS_PER_IDX;
		auto d = depth_ % DEPTHS_PER_IDX;

		// NOTE: Two shifts to prevent shifting by more bits than code_t contains
		ret.code_[i] &= ((~code_t(0)) << Dim) << (Dim * d);
		ret.code_[i] |= static_cast<code_t>(idx) << (Dim * d);

		return ret;
	}

	/*!
	 * @brief Get the code of a specific sibling of this firstborn code
	 *
	 * @note Should only be called on firstborn codes (i.e., `offset(depth())` should be
	 * `0`)
	 *
	 * @param idx The index of the sibling
	 * @return TreeCode The sibling code
	 */
	[[nodiscard]] constexpr TreeCode firstbornSibling(std::size_t idx) const
	{
		assert(0 == offset(depth_));
		assert(branchingFactor() > idx);

		auto i = depth_ / DEPTHS_PER_IDX;
		auto d = depth_ % DEPTHS_PER_IDX;

		TreeCode ret = *this;
		ret.code_[i] |= static_cast<code_t>(idx) << (Dim * d);
		return ret;
	}

	// void append(code_t code, depth_t num_depths)
	// {
	// 	// TODO: Fix asserts
	// 	assert(depth() >= num_depths);

	// 	// TODO: Implement
	// }

	// code_t remove(depth_t num_depths)
	// {
	// 	// TODO: Fix asserts
	// 	assert(maxDepth() - depth() >= num_depths);
	// 	assert(DEPTHS_PER_IDX >= num_depths);

	// 	// TODO: Implement
	// }

	// [[nodiscard]] code_t lowestOffsets(depth_t num_depths) const
	// {
	// 	// // TODO: Fix asserts
	// 	// assert(maxDepth() - depth() >= num_depths);
	// 	// assert(DEPTHS_PER_IDX >= num_depths);

	// 	// // TODO: Implement
	// 	// return 0;

	// 	assert(DEPTHS_PER_IDX >= num_depths);

	// 	return code_[0] &
	// 	       (~code_t(0) >> (std::numeric_limits<code_t>::digits - Dim * num_depths));
	// }

	// void lowestOffsets(depth_t num_depths, code_t lowest_offsets)
	// {
	// 	// // TODO: Fix asserts
	// 	// assert(maxDepth() - depth() >= num_depths);
	// 	// assert(DEPTHS_PER_IDX >= num_depths);

	// 	// // TODO: Implement
	// 	// return 0;

	// 	assert(DEPTHS_PER_IDX >= num_depths);

	// 	// TODO: Fix
	// 	code_[0] |= lowest_offsets;

	// 	// return code_[0] &
	// 	//        (~code_t(0) >> (std::numeric_limits<code_t>::digits - Dim * num_depths));
	// }

	[[nodiscard]] code_t lowestOffsets() const { return code_[0]; }

	void lowestOffsets(code_t lowest_offsets, depth_t depth = 0)
	{
		code_[0] = lowest_offsets;
		depth_   = depth;
	}

	/**************************************************************************************
	|                                                                                     |
	|                                       Compare                                       |
	|                                                                                     |
	**************************************************************************************/

	friend constexpr bool operator==(TreeCode const& lhs, TreeCode const& rhs) noexcept
	{
		return lhs.code_ == rhs.code_ && lhs.depth_ == rhs.depth_;
	}

	friend constexpr bool operator!=(TreeCode const& lhs, TreeCode const& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	friend constexpr bool operator<(TreeCode const& lhs, TreeCode const& rhs) noexcept
	{
		return lhs.code_ < rhs.code_;
	}

	friend constexpr bool operator<=(TreeCode const& lhs, TreeCode const& rhs) noexcept
	{
		return lhs.code_ <= rhs.code_;
	}

	friend constexpr bool operator>(TreeCode const& lhs, TreeCode const& rhs) noexcept
	{
		return lhs.code_ > rhs.code_;
	}

	friend constexpr bool operator>=(TreeCode const& lhs, TreeCode const& rhs) noexcept
	{
		return lhs.code_ >= rhs.code_;
	}

	friend std::ostream& operator<<(std::ostream& os, TreeCode const& code)
	{
		os << "code: " << code.code_[0] << " ";
		if constexpr (ACTIVE[1]) {
			os << code.code_[1] << " ";
		}
		if constexpr (ACTIVE[2]) {
			os << code.code_[2] << " ";
		}
		return os << "depth: " << code.depth();
	}

 private:
	std::array<code_t, 3> code_{};
	depth_t               depth_ = maxDepth() + 1;
};

using BinaryCode = TreeCode<1>;
using QuadCode   = TreeCode<2>;
using OctCode    = TreeCode<3>;
using HexCode    = TreeCode<4>;

}  // namespace ufo

namespace std
{
template <std::size_t Dim>
struct hash<ufo::TreeCode<Dim>> {
	std::size_t operator()(ufo::TreeCode<Dim> code) const
	{
		// Code size
		static constexpr std::size_t const CS =
		    std::numeric_limits<typename ufo::TreeCode<Dim>::code_t>::digits;
		// Hash size
		static constexpr std::size_t const HS = std::numeric_limits<std::size_t>::digits;

		std::size_t h = static_cast<std::size_t>(code.code_[0]);
		if constexpr (ufo::TreeCode<Dim>::ACTIVE[1] && HS > CS) {
			h |= static_cast<std::size_t>(code.code_[1]) << CS;
		}
		if constexpr (ufo::TreeCode<Dim>::ACTIVE[2] && HS > 2 * CS) {
			h |= static_cast<std::size_t>(code.code_[2]) << (2 * CS);
		}
		return h;
	}
};
}  // namespace std

#endif  // UFO_CONTAINER_TREE_CODE_HPP