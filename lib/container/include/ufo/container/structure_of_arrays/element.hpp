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

#ifndef UFO_CONTAINER_STRUCTURE_OF_ARRAYS_ELEMENT_HPP
#define UFO_CONTAINER_STRUCTURE_OF_ARRAYS_ELEMENT_HPP

// UFO
#include <ufo/container/structure_of_arrays/value.hpp>
#include <ufo/utility/type_traits.hpp>

// STL
#include <cassert>
#include <cstddef>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ufo
{
// Forward declaration
template <class... Ts>
class SoA;

template <class... Ts>
class SoAElement
{
	friend SoA<Ts...>;

 public:
	SoAElement() = default;

	SoAElement(SoAElement const& other)
	{
		if (other.is_reference_) {
			auto other_pos = other.position();
			value_ = new SoAValue<Ts...>(other.reference_->template view<Ts>()[other_pos]...);
		} else if (nullptr != other.value_) {
			value_ = new SoAValue<Ts...>(*other.value_);
		}
	}

	SoAElement(SoAElement&& other)
	{
		if (other.is_reference_) {
			auto other_pos = other.position();
			value_         = new SoAValue<Ts...>(
          std::move(other.reference_->template view<Ts>()[other_pos])...);
		} else {
			value_       = other.value_;
			other.value_ = nullptr;
		}
	}

	SoAElement(Ts const&... ts) : value_(new SoAValue<Ts...>(ts...)), is_reference_(false)
	{
	}

	SoAElement(SoAValue<Ts...> const& value) : value_(new SoAValue<Ts...>(value)) {}

	~SoAElement()
	{
		if (!is_reference_ && nullptr != value_) {
			delete value_;
		}
	}

	SoAElement& operator=(SoAElement const& rhs)
	{
		if (is_reference_ && rhs.is_reference_) {
			auto pos     = position();
			auto rhs_pos = rhs.position();
			((reference_->template view<Ts>()[pos] =
			      rhs.reference_->template view<Ts>()[rhs_pos]),
			 ...);
		} else if (is_reference_) {
			auto pos = position();
			if (nullptr == rhs.value_) {
				((reference_->template view<Ts>()[pos] = {}), ...);
			} else {
				((reference_->template view<Ts>()[pos] = rhs.value_->template get<Ts>()), ...);
			}
		} else if (rhs.is_reference_) {
			auto rhs_pos = rhs.position();
			if (nullptr == value_) {
				value_ = new SoAValue<Ts...>(rhs.reference_->template view<Ts>()[rhs_pos]...);
			} else {
				((value_->template get<Ts>() = rhs.reference_->template view<Ts>()[rhs_pos]),
				 ...);
			}
		} else if (nullptr == value_) {
			if (nullptr != rhs.value_) {
				value_ = new SoAValue<Ts...>(rhs.value_->template get<Ts>()...);
			}
		} else if (nullptr == rhs.value_) {
			*value_ = SoAValue<Ts...>();
		} else {
			*value_ = *rhs.value_;
		}

		return *this;
	}

	SoAElement& operator=(SoAElement&& rhs)
	{
		if (is_reference_ && rhs.is_reference_) {
			auto pos     = position();
			auto rhs_pos = rhs.position();
			((reference_->template view<Ts>()[pos] =
			      std::move(rhs.reference_->template view<Ts>()[rhs_pos])),
			 ...);
		} else if (is_reference_) {
			auto pos = position();
			if (nullptr == rhs.value_) {
				((reference_->template view<Ts>()[pos] = {}), ...);
			} else {
				((reference_->template view<Ts>()[pos] =
				      std::move(rhs.value_->template get<Ts>())),
				 ...);
			}
		} else if (rhs.is_reference_) {
			auto rhs_pos = rhs.position();
			if (nullptr == value_) {
				value_ = new SoAValue(std::move(rhs.reference_->template view<Ts>()[rhs_pos])...);
			} else {
				((value_->template get<Ts>() =
				      std::move(rhs.reference_->template view<Ts>()[rhs_pos])),
				 ...);
			}
		} else if (nullptr == value_) {
			value_     = rhs.value_;
			rhs.value_ = nullptr;
		} else if (nullptr == rhs.value_) {
			*value_ = SoAValue<Ts...>();
		} else {
			delete value_;
			value_     = rhs.value_;
			rhs.value_ = nullptr;
		}

		return *this;
	}

	SoAElement& operator=(SoAValue<Ts...> const& rhs)
	{
		if (is_reference_) {
			auto pos = position();
			((reference_->template view<Ts>()[pos] = rhs.template get<Ts>()), ...);
		} else if (nullptr == value_) {
			value_ = new SoAValue(rhs);
		} else {
			*value_ = rhs;
		}

		return *this;
	}

	SoAElement& operator=(SoAValue<Ts...>&& rhs)
	{
		if (is_reference_) {
			auto pos = position();
			((reference_->template view<Ts>()[pos] = std::move(rhs.template get<Ts>())), ...);
		} else if (nullptr == value_) {
			value_ = new SoAValue(std::move(rhs));
		} else {
			*value_ = std::move(rhs);
		}

		return *this;
	}

	template <class T, std::enable_if_t<contains_type_v<T, Ts...>, bool> = true>
	SoAElement& operator=(T const& rhs)
	{
		get<T>() = rhs;
		return *this;
	}

	operator SoAValue<Ts...>() const
	{
		if (is_reference_) {
			auto pos = position();
			return SoAValue<Ts...>(reference_->template view<Ts>()[pos]...);
		} else if (nullptr == value_) {
			return SoAValue<Ts...>();
		} else {
			return *value_;
		}
	}

	template <class T, std::enable_if_t<contains_type_v<T, Ts...>, bool> = true>
	operator T&()
	{
		return get<T>();
	}

	template <class T, std::enable_if_t<contains_type_v<T, Ts...>, bool> = true>
	operator T const&() const
	{
		return get<T>();
	}

	template <class T, std::enable_if_t<contains_type_v<T, Ts...>, bool> = true>
	[[nodiscard]] T& get()
	{
		if (is_reference_) {
			auto pos = position();
			return reference_->template view<T>()[pos];
		} else {
			if (nullptr == value_) {
				value_ = new SoAValue<Ts...>();
			}
			return value_->template get<T>();
		}
	}

	template <class T, std::enable_if_t<contains_type_v<T, Ts...>, bool> = true>
	[[nodiscard]] T const& get() const
	{
		if (is_reference_) {
			auto pos = position();
			return reference_->template view<T>()[pos];
		} else {
			if (nullptr == value_) {
				value_ = new SoAValue<Ts...>();
			}
			return value_->template get<T>();
		}
	}

	template <std::size_t I>
	[[nodiscard]] auto&& get() &
	{
		return get_helper<I>(*this);
	}

	template <std::size_t I>
	[[nodiscard]] auto&& get() &&
	{
		return get_helper<I>(*this);
	}

	template <std::size_t I>
	[[nodiscard]] auto&& get() const&
	{
		return get_helper<I>(*this);
	}

	template <std::size_t I>
	[[nodiscard]] auto&& get() const&&
	{
		return get_helper<I>(*this);
	}

 private:
	SoAElement(SoA<Ts...>* soa) : reference_(soa), is_reference_(true) {}

	template <std::size_t I, class T>
	[[nodiscard]] auto&& get_helper(T&& t)
	{
		if (is_reference_) {
			auto pos = position();
			return std::forward<T>(t).reference_->template view<I>()[pos];
		} else {
			if (nullptr == value_) {
				value_ = new SoAValue<Ts...>();
			}
			return std::forward<T>(t).value_->template get<I>();
		}
	}

	template <std::size_t I, class T>
	[[nodiscard]] auto&& get_helper(T&& t) const
	{
		if (is_reference_) {
			auto pos = position();
			return std::forward<T>(t).reference_->template view<I>()[pos];
		} else {
			if (nullptr == value_) {
				value_ = new SoAValue<Ts...>();
			}
			return std::forward<T>(t).value_->template get<I>();
		}
	}

	[[nodiscard]] std::size_t position() const
	{
		assert(is_reference_ && nullptr != reference_);
		return std::distance(reference_->cbegin(), this);
	}

 private:
	union {
		SoA<Ts...>*              reference_ = nullptr;
		mutable SoAValue<Ts...>* value_;
	};
	bool is_reference_ = false;
};

template <class... Ts>
bool operator==(SoAElement<Ts...> const& lhs, SoAElement<Ts...> const& rhs)
{
	return ((lhs.template get<Ts>() == rhs.template get<Ts>()) && ...);
}

template <class... Ts>
bool operator==(SoAElement<Ts...> const& lhs, SoAValue<Ts...> const& rhs)
{
	return ((lhs.template get<Ts>() == rhs.template get<Ts>()) && ...);
}

template <class... Ts>
bool operator==(SoAValue<Ts...> const& lhs, SoAElement<Ts...> const& rhs)
{
	return ((lhs.template get<Ts>() == rhs.template get<Ts>()) && ...);
}

template <class... Ts>
bool operator!=(SoAElement<Ts...> const& lhs, SoAElement<Ts...> const& rhs)
{
	return !(lhs == rhs);
}

template <class... Ts>
bool operator!=(SoAElement<Ts...> const& lhs, SoAValue<Ts...> const& rhs)
{
	return !(lhs == rhs);
}

template <class... Ts>
bool operator!=(SoAValue<Ts...> const& lhs, SoAElement<Ts...> const& rhs)
{
	return !(lhs == rhs);
}

template <class T, class... Ts>
std::ostream& operator<<(std::ostream& os, SoAElement<T, Ts...> const& element)
{
	os << element.template get<T>();
	((os << ", " << element.template get<Ts>()), ...);
	return os;
}

//
// Type traits
//

template <class T, class... Ts>
struct contains_type<T, SoAElement<Ts...>> : contains_type<T, Ts...> {
};
}  // namespace ufo

namespace std
{
template <class... Ts>
struct tuple_size<ufo::SoAElement<Ts...>> : integral_constant<size_t, sizeof...(Ts)> {
};

template <size_t Index, class... Ts>
struct tuple_element<Index, ufo::SoAElement<Ts...>> : tuple_element<Index, tuple<Ts...>> {
};
}  // namespace std

#endif  // UFO_CONTAINER_STRUCTURE_OF_ARRAYS_ELEMENT_HPP