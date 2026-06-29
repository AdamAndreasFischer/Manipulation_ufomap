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

#ifndef UFO_UTILITY_MACROS
#define UFO_UTILITY_MACROS

#define UFO_REPEAT_2(M, N)   M(N) M(N + 1)
#define UFO_REPEAT_4(M, N)   UFO_REPEAT_2(M, N) UFO_REPEAT_2(M, N + 2)
#define UFO_REPEAT_8(M, N)   UFO_REPEAT_4(M, N) UFO_REPEAT_4(M, N + 4)
#define UFO_REPEAT_16(M, N)  UFO_REPEAT_8(M, N) UFO_REPEAT_8(M, N + 8)
#define UFO_REPEAT_32(M, N)  UFO_REPEAT_16(M, N) UFO_REPEAT_16(M, N + 16)
#define UFO_REPEAT_64(M, N)  UFO_REPEAT_32(M, N) UFO_REPEAT_32(M, N + 32)
#define UFO_REPEAT_128(M, N) UFO_REPEAT_64(M, N) UFO_REPEAT_64(M, N + 64)

#define UFO_MIN(a, b)            ((a) < (b) ? (a) : (b))
#define UFO_MAX(a, b)            ((a) < (b) ? (b) : (a))
#define UFO_CLAMP(v, lo, hi)     UFO_MAX(lo, UFO_MIN(hi, v))
#define UFO_MIN_PAIR_FIRST(a, b) ((a).first < (b).first ? (a) : (b))
#define UFO_MAX_PAIR_FIRST(a, b) ((a).first < (b).first ? (b) : (a))
#define UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, i, j) \
	{                                                 \
		auto const a = UFO_MIN_PAIR_FIRST(c[i], c[j]);  \
		c[j]         = UFO_MAX_PAIR_FIRST(c[i], c[j]);  \
		c[i]         = a;                               \
	}

#define UFO_MIN_2(c)            \
	{                             \
		c[0] = UFO_MIN(c[0], c[1]); \
	}

#define UFO_MIN_3(c)            \
	{                             \
		c[0] = UFO_MIN(c[0], c[2]); \
		UFO_MIN_2(c);               \
	}

#define UFO_MIN_4(c)            \
	{                             \
		c[0] = UFO_MIN(c[0], c[2]); \
		c[1] = UFO_MIN(c[1], c[3]); \
		UFO_MIN_2(c);               \
	}

#define UFO_MIN_8(c)            \
	{                             \
		c[0] = UFO_MIN(c[0], c[4]); \
		c[1] = UFO_MIN(c[1], c[5]); \
		c[2] = UFO_MIN(c[2], c[6]); \
		c[3] = UFO_MIN(c[3], c[7]); \
		UFO_MIN_4(c);               \
	}

#define UFO_MIN_16(c)            \
	{                              \
		c[0] = UFO_MIN(c[0], c[8]);  \
		c[1] = UFO_MIN(c[1], c[9]);  \
		c[2] = UFO_MIN(c[2], c[10]); \
		c[3] = UFO_MIN(c[3], c[11]); \
		c[4] = UFO_MIN(c[4], c[12]); \
		c[5] = UFO_MIN(c[5], c[13]); \
		c[6] = UFO_MIN(c[6], c[14]); \
		c[7] = UFO_MIN(c[7], c[15]); \
		UFO_MIN_8(c);                \
	}

#define UFO_MIN_PAIR_FIRST_2(c)            \
	{                                        \
		c[0] = UFO_MIN_PAIR_FIRST(c[0], c[1]); \
	}

#define UFO_MIN_PAIR_FIRST_4(c)            \
	{                                        \
		c[0] = UFO_MIN_PAIR_FIRST(c[0], c[2]); \
		c[1] = UFO_MIN_PAIR_FIRST(c[1], c[3]); \
		UFO_MIN_PAIR_FIRST_2(c);               \
	}

#define UFO_MIN_PAIR_FIRST_8(c)            \
	{                                        \
		c[0] = UFO_MIN_PAIR_FIRST(c[0], c[4]); \
		c[1] = UFO_MIN_PAIR_FIRST(c[1], c[5]); \
		c[2] = UFO_MIN_PAIR_FIRST(c[2], c[6]); \
		c[3] = UFO_MIN_PAIR_FIRST(c[3], c[7]); \
		UFO_MIN_PAIR_FIRST_4(c);               \
	}

#define UFO_MIN_PAIR_FIRST_16(c)            \
	{                                         \
		c[0] = UFO_MIN_PAIR_FIRST(c[0], c[8]);  \
		c[1] = UFO_MIN_PAIR_FIRST(c[1], c[9]);  \
		c[2] = UFO_MIN_PAIR_FIRST(c[2], c[10]); \
		c[3] = UFO_MIN_PAIR_FIRST(c[3], c[11]); \
		c[4] = UFO_MIN_PAIR_FIRST(c[4], c[12]); \
		c[5] = UFO_MIN_PAIR_FIRST(c[5], c[13]); \
		c[6] = UFO_MIN_PAIR_FIRST(c[6], c[14]); \
		c[7] = UFO_MIN_PAIR_FIRST(c[7], c[15]); \
		UFO_MIN_PAIR_FIRST_8(c);                \
	}

#define UFO_SORT_ASCENDING_PAIR_FIRST_2(c)       \
	{                                              \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 0, 1); \
	}

#define UFO_SORT_ASCENDING_PAIR_FIRST_4(c)       \
	{                                              \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 0, 2); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 1, 3); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 0, 1); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 2, 3); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 1, 2); \
	}

#define UFO_SORT_ASCENDING_PAIR_FIRST_8(c)       \
	{                                              \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 0, 2); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 1, 3); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 4, 6); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 5, 7); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 0, 4); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 1, 5); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 2, 6); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 3, 7); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 0, 1); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 2, 3); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 4, 5); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 6, 7); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 2, 4); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 3, 5); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 1, 4); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 3, 6); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 1, 2); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 3, 4); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 5, 6); \
	}

#define UFO_SORT_ASCENDING_PAIR_FIRST_16(c)        \
	{                                                \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 0, 13);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 1, 12);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 2, 15);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 3, 14);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 4, 8);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 5, 6);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 7, 11);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 9, 10);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 0, 5);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 1, 7);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 2, 9);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 3, 4);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 6, 13);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 8, 14);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 10, 15); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 11, 12); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 0, 1);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 2, 3);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 4, 5);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 6, 8);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 7, 9);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 10, 11); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 12, 13); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 14, 15); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 0, 2);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 1, 3);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 4, 10);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 5, 11);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 6, 7);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 8, 9);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 12, 14); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 13, 15); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 1, 2);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 3, 12);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 4, 6);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 5, 7);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 8, 10);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 9, 11);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 13, 14); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 1, 4);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 2, 6);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 5, 8);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 7, 10);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 9, 13);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 11, 14); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 2, 4);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 3, 6);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 9, 12);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 11, 13); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 3, 5);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 6, 8);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 7, 9);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 10, 12); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 3, 4);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 5, 6);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 7, 8);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 9, 10);  \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 11, 12); \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 6, 7);   \
		UFO_SORT_ASCENDING_PAIR_FIRST_SWAP(c, 8, 9);   \
	}

#endif  // UFO_UTILITY_MACROS