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

#ifndef UFO_EXECUTION_EXECUTION_HPP
#define UFO_EXECUTION_EXECUTION_HPP

// UFO
#include <ufo/utility/enum.hpp>
#include <ufo/utility/type_traits.hpp>

// STL
#include <type_traits>

#if defined(UFO_PAR_STL)
#include <execution>
#endif

// GCD (Grand Central Dispatch)
#if defined(UFO_PAR_GCD)
#include <dispatch/dispatch.h>
#else
#define __block
#endif

// oneTBB (Threading Building Blocks)
#if defined(UFO_PAR_TBB)
#include <oneapi/tbb.h>
#endif

// OMP (OpenMP)
#if defined(UFO_PAR_OMP)
#include <omp.h>
#endif

namespace ufo::execution
{
namespace detail
{
enum class ExecutionPolicy : unsigned {
	NONE      = 0u,
	SEQ       = 1u << 0,
	UNSEQ     = 1u << 1,
	PAR       = 1u << 2,
	PAR_UNSEQ = 1u << 3,
};

enum class ExecutionBackend : unsigned {
	NONE = 0u,

#if defined(UFO_PAR_STL)
	STL = 1u << 0,
#else
	STL = NONE,
#endif

#if defined(UFO_PAR_GCD)
	GCD = 1u << 1,
#else
	GCD = NONE,
#endif

#if defined(UFO_PAR_TBB)
	TBB = 1u << 2,
#else
	TBB = NONE,
#endif

#if defined(UFO_PAR_OMP)
	OMP = 1u << 3,
#else
	OMP = NONE,
#endif

	ALL = STL | GCD | TBB | OMP
};

constexpr ExecutionPolicy operator|(ExecutionPolicy const& lhs,
                                    ExecutionPolicy const& rhs)
{
	return ExecutionPolicy(to_underlying(lhs) | to_underlying(rhs));
}

constexpr ExecutionPolicy operator&(ExecutionPolicy const& lhs,
                                    ExecutionPolicy const& rhs)
{
	return ExecutionPolicy(to_underlying(lhs) & to_underlying(rhs));
}

constexpr ExecutionBackend operator|(ExecutionBackend const& lhs,
                                     ExecutionBackend const& rhs)
{
	return ExecutionBackend(to_underlying(lhs) | to_underlying(rhs));
}

constexpr ExecutionBackend operator&(ExecutionBackend const& lhs,
                                     ExecutionBackend const& rhs)
{
	return ExecutionBackend(to_underlying(lhs) & to_underlying(rhs));
}
}  // namespace detail

template <detail::ExecutionPolicy Policy, detail::ExecutionBackend Backend>
struct execution_policy {
	static constexpr detail::ExecutionPolicy const  policy  = Policy;
	static constexpr detail::ExecutionBackend const backend = Backend;
};

using sequenced_policy =
    execution_policy<detail::ExecutionPolicy::SEQ, detail::ExecutionBackend::ALL>;

using unsequenced_policy =
    execution_policy<detail::ExecutionPolicy::UNSEQ, detail::ExecutionBackend::ALL>;

using parallel_policy =
    execution_policy<detail::ExecutionPolicy::PAR, detail::ExecutionBackend::ALL>;

using parallel_unsequenced_policy =
    execution_policy<detail::ExecutionPolicy::PAR_UNSEQ, detail::ExecutionBackend::ALL>;

using stl_sequenced_policy =
    execution_policy<detail::ExecutionPolicy::SEQ, detail::ExecutionBackend::STL>;

using stl_unsequenced_policy =
    execution_policy<detail::ExecutionPolicy::UNSEQ, detail::ExecutionBackend::STL>;

using stl_parallel_policy =
    execution_policy<detail::ExecutionPolicy::PAR, detail::ExecutionBackend::STL>;

using stl_parallel_unsequenced_policy =
    execution_policy<detail::ExecutionPolicy::PAR_UNSEQ, detail::ExecutionBackend::STL>;

using gcd_sequenced_policy =
    execution_policy<detail::ExecutionPolicy::SEQ, detail::ExecutionBackend::GCD>;

using gcd_unsequenced_policy =
    execution_policy<detail::ExecutionPolicy::UNSEQ, detail::ExecutionBackend::GCD>;

using gcd_parallel_policy =
    execution_policy<detail::ExecutionPolicy::PAR, detail::ExecutionBackend::GCD>;

using gcd_parallel_unsequenced_policy =
    execution_policy<detail::ExecutionPolicy::PAR_UNSEQ, detail::ExecutionBackend::GCD>;

using tbb_sequenced_policy =
    execution_policy<detail::ExecutionPolicy::SEQ, detail::ExecutionBackend::TBB>;

using tbb_unsequenced_policy =
    execution_policy<detail::ExecutionPolicy::UNSEQ, detail::ExecutionBackend::TBB>;

using tbb_parallel_policy =
    execution_policy<detail::ExecutionPolicy::PAR, detail::ExecutionBackend::TBB>;

using tbb_parallel_unsequenced_policy =
    execution_policy<detail::ExecutionPolicy::PAR_UNSEQ, detail::ExecutionBackend::TBB>;

using omp_sequenced_policy =
    execution_policy<detail::ExecutionPolicy::SEQ, detail::ExecutionBackend::OMP>;

using omp_unsequenced_policy =
    execution_policy<detail::ExecutionPolicy::UNSEQ, detail::ExecutionBackend::OMP>;

using omp_parallel_policy =
    execution_policy<detail::ExecutionPolicy::PAR, detail::ExecutionBackend::OMP>;

using omp_parallel_unsequenced_policy =
    execution_policy<detail::ExecutionPolicy::PAR_UNSEQ, detail::ExecutionBackend::OMP>;

template <class, class = void>
struct is_execution_policy : std::false_type {
};

template <detail::ExecutionPolicy Policy, detail::ExecutionBackend Backend>
struct is_execution_policy<execution_policy<Policy, Backend>,
                           std::enable_if_t<detail::ExecutionBackend::NONE != Backend &&
                                            detail::ExecutionPolicy::NONE != Policy>>
    : std::true_type {
};

template <class T>
constexpr inline bool is_execution_policy_v =
    is_execution_policy<remove_cvref_t<T>>::value;

template <class T>
constexpr inline bool is_seq_v =
    detail::ExecutionPolicy::NONE !=
    (detail::ExecutionPolicy::SEQ & remove_cvref_t<T>::policy);

template <class T>
constexpr inline bool is_unseq_v =
    detail::ExecutionPolicy::NONE !=
    (detail::ExecutionPolicy::UNSEQ & remove_cvref_t<T>::policy);

template <class T>
constexpr inline bool is_par_v =
    detail::ExecutionPolicy::NONE !=
    (detail::ExecutionPolicy::PAR & remove_cvref_t<T>::policy);

template <class T>
constexpr inline bool is_par_unseq_v =
    detail::ExecutionPolicy::NONE !=
    (detail::ExecutionPolicy::PAR_UNSEQ & remove_cvref_t<T>::policy);

template <class T>
constexpr inline bool is_stl_v =
    detail::ExecutionBackend::NONE !=
    (detail::ExecutionBackend::STL & remove_cvref_t<T>::backend);

template <class T>
constexpr inline bool is_gcd_v =
    detail::ExecutionBackend::NONE !=
    (detail::ExecutionBackend::GCD & remove_cvref_t<T>::backend);

template <class T>
constexpr inline bool is_tbb_v =
    detail::ExecutionBackend::NONE !=
    (detail::ExecutionBackend::TBB & remove_cvref_t<T>::backend);

template <class T>
constexpr inline bool is_omp_v =
    detail::ExecutionBackend::NONE !=
    (detail::ExecutionBackend::OMP & remove_cvref_t<T>::backend);

constexpr inline sequenced_policy                seq{};
constexpr inline unsequenced_policy              unseq{};
constexpr inline parallel_policy                 par{};
constexpr inline parallel_unsequenced_policy     par_unseq{};
constexpr inline gcd_sequenced_policy            gcd_seq{};
constexpr inline gcd_unsequenced_policy          gcd_unseq{};
constexpr inline gcd_parallel_policy             gcd_par{};
constexpr inline gcd_parallel_unsequenced_policy gcd_par_unseq{};
constexpr inline tbb_sequenced_policy            tbb_seq{};
constexpr inline tbb_unsequenced_policy          tbb_unseq{};
constexpr inline tbb_parallel_policy             tbb_par{};
constexpr inline tbb_parallel_unsequenced_policy tbb_par_unseq{};
constexpr inline omp_sequenced_policy            omp_seq{};
constexpr inline omp_unsequenced_policy          omp_unseq{};
constexpr inline omp_parallel_policy             omp_par{};
constexpr inline omp_parallel_unsequenced_policy omp_par_unseq{};

template <class ExecutionPolicy>
[[nodiscard]] constexpr auto&& toSTL(ExecutionPolicy&& policy)
{
#if defined(UFO_PAR_STL)
	if constexpr (is_stl_v<ExecutionPolicy>) {
		if constexpr (is_seq_v<ExecutionPolicy>) {
			return std::execution::seq;
		} else if constexpr (is_unseq_v<ExecutionPolicy>) {
#if __cplusplus >= 201902L
			return std::execution::unseq;
#else
			return std::execution::seq;
#endif
		} else if constexpr (is_par_v<ExecutionPolicy>) {
			return std::execution::par;
		} else if constexpr (is_par_unseq_v<ExecutionPolicy>) {
			return std::execution::par_unseq;
		}
	}
#endif
}
}  // namespace ufo::execution

#endif  // UFO_EXECUTION_EXECUTION_HPP