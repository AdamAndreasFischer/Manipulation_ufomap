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

#ifndef UFO_DYNAMIC_GEOMETRY_HPP
#define UFO_DYNAMIC_GEOMETRY_HPP

// STL
#include <memory>

namespace ufo
{
namespace detail
{
class DynamicGeometryBase
{
 public:
	virtual ~DynamicGeometryBase() {}

	[[nodiscard]] virtual DynamicGeometryBase* clone() const = 0;
};

template <class Geometry>
class DynamicGeometry
    : public DynamicGeometryBase
    , public Geometry
{
 public:
	DynamicGeometry(Geometry const& geometry) : Geometry(geometry) {}

	DynamicGeometry(Geometry&& geometry) : Geometry(std::move(geometry)) {}

	virtual ~DynamicGeometry() {}

 protected:
	[[nodiscard]] DynamicGeometry* clone() const override
	{
		return new DynamicGeometry(*this);
	}
};
}  // namespace detail

class DynamicGeometry
{
 public:
	DynamicGeometry() = default;

	template <class Geometry>
	DynamicGeometry(Geometry&& geometry)
	    : geometry_(std::make_unique<detail::DynamicGeometry<Geometry>>(
	          std::forward<Geometry>(geometry)))
	{
	}

	DynamicGeometry(DynamicGeometry const& other)
	{
		if (other.hasGeometry()) {
			geometry_.reset(other.geometry_->clone());
		}
	}

	DynamicGeometry(DynamicGeometry&&) = default;

	DynamicGeometry(detail::DynamicGeometryBase const& geometry)
	    : geometry_(geometry.clone())
	{
	}

	DynamicGeometry& operator=(DynamicGeometry const& rhs)
	{
		if (rhs.hasGeometry()) {
			geometry_.reset(rhs.geometry_->clone());
		} else {
			geometry_.reset();
		}
		return *this;
	}

	DynamicGeometry& operator=(DynamicGeometry&&) = default;

	DynamicGeometry& operator=(detail::DynamicGeometryBase const& rhs)
	{
		geometry_.reset(rhs.clone());
		return *this;
	}

	template <class Geometry>
	DynamicGeometry& operator=(Geometry&& geometry)
	{
		geometry_ = std::make_unique<detail::DynamicGeometry<Geometry>>(
		    std::forward<Geometry>(geometry));
		return *this;
	}

	template <class Geometry>
	[[nodiscard]] constexpr bool contains(Geometry const& geometry) const
	{
		// TODO: Implement
		return true;
	}

	template <class Geometry>
	[[nodiscard]] constexpr bool disjoint(Geometry const& geometry) const
	{
		// TODO: Implement
		return true;
	}

	template <class Geometry>
	[[nodiscard]] constexpr bool inside(Geometry const& geometry) const
	{
		// TODO: Implement
		return true;
	}

	template <class Geometry>
	[[nodiscard]] constexpr bool intersects(Geometry const& geometry) const
	{
		// TODO: Implement
		return true;
	}

	template <class Geometry>
	[[nodiscard]] constexpr double distance(Geometry const& geometry) const
	{
		// TODO: Should this also be able to return double?
		// TODO: Implement
		return 0.0;
	}

	[[nodiscard]] bool hasGeometry() const { return !!geometry_; }

 private:
	std::unique_ptr<detail::DynamicGeometryBase> geometry_;
};
}  // namespace ufo

#endif  // UFO_DYNAMIC_GEOMETRY_HPP