/*!
 * UFOMap: An Efficient Probabilistic 3D Mapping Framework That Embraces the
 * Unknown
 *
 * @author Daniel Duberg (dduberg@kth.se)
 * @see https://github.com/UnknownFreeOccupied/ufomap
 * @version 1.0
 * @date 2022-05-13
 *
 * @copyright Copyright (c) 2022, Daniel Duberg, KTH Royal Institute of
 * Technology
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Daniel Duberg, KTH Royal Institute of Technology
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
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
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UFO_MORTON1_HPP
#define UFO_MORTON1_HPP

// UFO
#include <ufo/math/vec1.hpp>
#include <ufo/morton/detail/morton.hpp>

// STL
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace ufo
{
template <>
struct Morton<1> {
	static constexpr std::uint32_t const X_M_32 = 0xffffffff;
	static constexpr std::uint64_t const X_M_64 = 0xffffffffffffffff;

	static constexpr std::size_t const LEVELS_32 = 32;  // floor(32 / 1)
	static constexpr std::size_t const LEVELS_64 = 64;  // floor(64 / 1)

	[[nodiscard]] static constexpr std::uint32_t encode32(std::uint32_t x) { return x; }

	[[nodiscard]] static constexpr std::uint32_t encode32(Vec1u v) { return encode32(v.x); }

	[[nodiscard]] static constexpr std::uint64_t encode64(std::uint64_t x) { return x; }

	[[nodiscard]] static constexpr std::uint64_t encode64(Vec1u v) { return encode64(v.x); }

	[[nodiscard]] static constexpr Vec1u decode32(std::uint32_t m) { return Vec1u(m); }

	[[nodiscard]] static constexpr std::uint32_t decode32(std::uint32_t m, std::size_t pos)
	{
		assert(1 > pos);
		return compact32(m >> pos);
	}

	[[nodiscard]] static constexpr Vec1u decode64(std::uint64_t m) { return Vec1u(m); }

	[[nodiscard]] static constexpr std::uint64_t decode64(std::uint64_t m, std::size_t pos)
	{
		assert(1 > pos);
		return compact64(m >> pos);
	}

	[[nodiscard]] static constexpr std::uint32_t spread32(std::uint32_t x) { return x; }

	[[nodiscard]] static constexpr std::uint64_t spread64(std::uint64_t x) { return x; }

	[[nodiscard]] static constexpr std::uint32_t compact32(std::uint32_t m) { return m; }

	[[nodiscard]] static constexpr std::uint64_t compact64(std::uint64_t m) { return m; }
};
}  // namespace ufo

#endif  // UFO_MORTON1_HPP