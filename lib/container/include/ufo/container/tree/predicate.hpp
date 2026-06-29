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

#ifndef UFO_CONTAINER_TREE_PREDICATE_HPP
#define UFO_CONTAINER_TREE_PREDICATE_HPP

// UFO
#include <ufo/container/tree/predicate/and.hpp>
#include <ufo/container/tree/predicate/bool.hpp>
#include <ufo/container/tree/predicate/child_of.hpp>
#include <ufo/container/tree/predicate/coord.hpp>
#include <ufo/container/tree/predicate/depth.hpp>
#include <ufo/container/tree/predicate/depth_interval.hpp>
#include <ufo/container/tree/predicate/exists.hpp>
#include <ufo/container/tree/predicate/false.hpp>
#include <ufo/container/tree/predicate/filter.hpp>
#include <ufo/container/tree/predicate/if_and_only_if.hpp>
#include <ufo/container/tree/predicate/inner.hpp>
#include <ufo/container/tree/predicate/leaf.hpp>
#include <ufo/container/tree/predicate/leaf_or_depth.hpp>
#include <ufo/container/tree/predicate/length.hpp>
#include <ufo/container/tree/predicate/length_interval.hpp>
#include <ufo/container/tree/predicate/modified.hpp>
#include <ufo/container/tree/predicate/offset.hpp>
#include <ufo/container/tree/predicate/or.hpp>
#include <ufo/container/tree/predicate/parent.hpp>
#include <ufo/container/tree/predicate/predicate.hpp>
#include <ufo/container/tree/predicate/predicate_compare.hpp>
#include <ufo/container/tree/predicate/pure_leaf.hpp>
#include <ufo/container/tree/predicate/satisfies.hpp>
#include <ufo/container/tree/predicate/satisfies_inner.hpp>
#include <ufo/container/tree/predicate/spatial.hpp>
#include <ufo/container/tree/predicate/then.hpp>
#include <ufo/container/tree/predicate/true.hpp>

namespace ufo::pred
{
constexpr PureLeaf operator!(Inner) { return {}; }
constexpr Inner    operator!(PureLeaf) { return {}; }
constexpr Parent   operator!(Leaf) { return {}; }
constexpr Leaf     operator!(Parent) { return {}; }
}  // namespace ufo::pred

#endif  // UFO_CONTAINER_TREE_PREDICATE_HPP