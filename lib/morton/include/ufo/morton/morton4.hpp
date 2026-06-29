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

#ifndef UFO_MORTON4_HPP
#define UFO_MORTON4_HPP

// UFO
#include <ufo/math/vec4.hpp>
#include <ufo/morton/detail/morton.hpp>

// STL
#include <cassert>
#include <cstddef>
#include <cstdint>

#if defined(UFO_MORTON_BMI2) && (defined(__BMI2__) || defined(__AVX2__))
#define UFO_MORTON_CONSTEXPR
#include <immintrin.h>
#else
#define UFO_MORTON_CONSTEXPR constexpr
#endif

namespace ufo
{
template <>
struct Morton<4> {
	static constexpr std::uint32_t const X_M_32 = 0x11111111;
	static constexpr std::uint32_t const Y_M_32 = X_M_32 << 1;
	static constexpr std::uint32_t const Z_M_32 = X_M_32 << 2;
	static constexpr std::uint32_t const W_M_32 = X_M_32 << 3;

	static constexpr std::uint64_t const X_M_64 = 0x1111111111111111;
	static constexpr std::uint64_t const Y_M_64 = X_M_64 << 1;
	static constexpr std::uint64_t const Z_M_64 = X_M_64 << 2;
	static constexpr std::uint64_t const W_M_64 = X_M_64 << 3;

	static constexpr std::size_t const LEVELS_32 = 8;   // floor(32 / 4)
	static constexpr std::size_t const LEVELS_64 = 16;  // floor(64 / 4)

	[[nodiscard]] static UFO_MORTON_CONSTEXPR std::uint32_t encode32(std::uint32_t x,
	                                                                 std::uint32_t y,
	                                                                 std::uint32_t z,
	                                                                 std::uint32_t w)
	{
#if defined(UFO_MORTON_BMI2) && (defined(__BMI2__) || defined(__AVX2__))
		return _pdep_u32(x, X_M_32) | _pdep_u32(y, Y_M_32) | _pdep_u32(z, Z_M_32) |
		       _pdep_u32(w, W_M_32);
#else
		return spread32(x) | (spread32(y) << 1) | (spread32(z) << 2) | (spread32(w) << 3);
#endif
	}

	[[nodiscard]] static UFO_MORTON_CONSTEXPR std::uint32_t encode32(Vec4u const& v)
	{
		return encode32(v.x, v.y, v.z, v.w);
	}

	[[nodiscard]] static UFO_MORTON_CONSTEXPR std::uint64_t encode64(std::uint32_t x,
	                                                                 std::uint32_t y,
	                                                                 std::uint32_t z,
	                                                                 std::uint32_t w)
	{
#if defined(UFO_MORTON_BMI2) && (defined(__BMI2__) || defined(__AVX2__))
		return _pdep_u64(x, X_M_64) | _pdep_u64(y, Y_M_64) | _pdep_u64(z, Z_M_64) |
		       _pdep_u64(w, W_M_64);
#else
		return spread64(x) | (spread64(y) << 1) | (spread64(z) << 2) | (spread64(w) << 3);
#endif
	}

	[[nodiscard]] static UFO_MORTON_CONSTEXPR std::uint64_t encode64(Vec4u const& v)
	{
		return encode64(v.x, v.y, v.z, v.w);
	}

	[[nodiscard]] static UFO_MORTON_CONSTEXPR Vec4u decode32(std::uint32_t m)
	{
#if defined(UFO_MORTON_BMI2) && (defined(__BMI2__) || defined(__AVX2__))
		return Vec4u(_pext_u32(m, X_M_32), _pext_u32(m, Y_M_32), _pext_u32(m, Z_M_32),
		             _pext_u32(m, W_M_32));
#else
		return Vec4u(compact32(m), compact32(m >> 1), compact32(m >> 2), compact32(m >> 3));
#endif
	}

	[[nodiscard]] static UFO_MORTON_CONSTEXPR std::uint32_t decode32(std::uint32_t m,
	                                                                 std::size_t   pos)
	{
		assert(4 > pos);
		return compact32(m >> pos);
	}

	[[nodiscard]] static UFO_MORTON_CONSTEXPR Vec4u decode64(std::uint64_t m)
	{
#if defined(UFO_MORTON_BMI2) && (defined(__BMI2__) || defined(__AVX2__))
		return Vec4u(_pext_u64(m, X_M_64), _pext_u64(m, Y_M_64), _pext_u64(m, Z_M_64),
		             _pext_u64(m, W_M_64));
#else
		return Vec4u(compact64(m), compact64(m >> 1), compact64(m >> 2), compact64(m >> 3));
#endif
	}

	[[nodiscard]] static UFO_MORTON_CONSTEXPR std::uint32_t decode64(std::uint64_t m,
	                                                                 std::size_t   pos)
	{
		assert(4 > pos);
		return compact64(m >> pos);
	}

	[[nodiscard]] static UFO_MORTON_CONSTEXPR std::uint32_t spread32(std::uint32_t x)
	{
#if defined(UFO_MORTON_BMI2) && (defined(__BMI2__) || defined(__AVX2__))
		return _pdep_u32(x, X_M_32);
#else
		std::uint32_t m(x);
		m &= 0xFF;
		m = (m | (m << 16)) & 0xC0003F;
		m = (m | (m << 8)) & 0xC03807;
		m = (m | (m << 4)) & 0x8430843;
		m = (m | (m << 2)) & 0x9090909;
		m = (m | (m << 1)) & X_M_32;
		return m;
#endif
	}

	[[nodiscard]] static UFO_MORTON_CONSTEXPR std::uint64_t spread64(std::uint32_t x)
	{
#if defined(UFO_MORTON_BMI2) && (defined(__BMI2__) || defined(__AVX2__))
		return _pdep_u64(x, X_M_64);
#else
		std::uint64_t m(x);
		m &= 0xFFFF;
		m = (m | (m << 32)) & 0xF800000007FF;
		m = (m | (m << 16)) & 0xF80007C0003F;
		m = (m | (m << 8)) & 0xC0380700C03807;
		m = (m | (m << 4)) & 0x843084308430843;
		m = (m | (m << 2)) & 0x909090909090909;
		m = (m | (m << 1)) & X_M_64;
		return m;
#endif
	}

	[[nodiscard]] static UFO_MORTON_CONSTEXPR std::uint32_t compact32(std::uint32_t m)
	{
#if defined(UFO_MORTON_BMI2) && (defined(__BMI2__) || defined(__AVX2__))
		return _pext_u32(m, X_M_32);
#else
		std::uint32_t x(m);
		x &= X_M_32;
		x = (x ^ (x >> 1)) & 0x9090909;
		x = (x ^ (x >> 2)) & 0x8430843;
		x = (x ^ (x >> 4)) & 0xC03807;
		x = (x ^ (x >> 8)) & 0xC0003F;
		x = (x ^ (x >> 16)) & 0xFF;
		return x;
#endif
	}

	[[nodiscard]] static UFO_MORTON_CONSTEXPR std::uint32_t compact64(std::uint64_t m)
	{
#if defined(UFO_MORTON_BMI2) && (defined(__BMI2__) || defined(__AVX2__))
		return static_cast<std::uint32_t>(_pext_u64(m, X_M_64));
#else
		std::uint64_t x(m);
		x &= X_M_64;
		x = (x ^ (x >> 1)) & 0x909090909090909;
		x = (x ^ (x >> 2)) & 0x843084308430843;
		x = (x ^ (x >> 4)) & 0xC0380700C03807;
		x = (x ^ (x >> 8)) & 0xF80007C0003F;
		x = (x ^ (x >> 16)) & 0xF800000007FF;
		x = (x ^ (x >> 32)) & 0xFFFF;
		return static_cast<std::uint32_t>(x);
#endif
	}
};
}  // namespace ufo

#endif  // UFO_MORTON4_HPP