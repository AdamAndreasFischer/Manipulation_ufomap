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

#ifndef UFO_VISION_DETAIL_COLOR_TYPE_TRAITS_HPP
#define UFO_VISION_DETAIL_COLOR_TYPE_TRAITS_HPP

// UFO
#include <ufo/vision/color_type.hpp>
#include <ufo/vision/detail/color.hpp>

// STL
#include <type_traits>

namespace ufo::color
{
namespace detail
{
template <ColorType CT, ColorType... CTs>
struct is_color_type : std::bool_constant<((CT == CTs) || ...)> {
};
}  // namespace detail

template <class T>
struct is_color : std::false_type {
};

template <ColorType CT>
struct is_color<Color<CT>> : std::true_type {
};

template <class T>
constexpr inline bool is_color_v = is_color<T>::value;

template <class T>
struct color_type;

template <ColorType CT>
struct color_type<Color<CT>> : std::integral_constant<ColorType, CT> {
};

template <class T>
constexpr inline ColorType color_type_v = color_type<T>::value;

// NOTE: Might be problems if they are volatile/const/reference.

template <class... Ts>
struct contains_color : std::disjunction<is_color<Ts>...> {
};

template <class... Ts>
constexpr inline bool contains_color_v = contains_color<Ts...>::value;

// NOTE: Might be problems if they are volatile/const/reference.

template <class T, class... Ts>
struct first_color_type : first_color_type<Ts...> {
};

template <ColorType CT, class... Ts>
struct first_color_type<Color<CT>, Ts...> : std::integral_constant<ColorType, CT> {
};

template <class... Ts>
constexpr inline ColorType first_color_type_v = first_color_type<Ts...>::value;

// NOTE: Might be problems if they are volatile/const/reference.

namespace detail
{
template <std::size_t I, class T, class... Ts>
struct first_color_index : first_color_index<I + 1, Ts...> {
};

template <ColorType CT, std::size_t I, class... Ts>
struct first_color_index<I, Color<CT>, Ts...> : std::integral_constant<std::size_t, I> {
};
}  // namespace detail

template <class... Ts>
struct first_color_index : detail::first_color_index<0, Ts...> {
};

template <class... Ts>
constexpr inline std::size_t first_color_index_v = first_color_index<Ts...>::value;

template <ColorType CT>
struct is_rgb
    : detail::is_color_type<CT, ColorType::RGB8U, ColorType::RGBA8U, ColorType::RGB32F,
                            ColorType::RGBA32F, ColorType::RGBW32F, ColorType::RGBAW32F> {
};

template <ColorType CT>
constexpr inline bool is_rgb_v = is_rgb<CT>::value;

template <ColorType CT>
struct is_lab
    : detail::is_color_type<CT, ColorType::LAB32F, ColorType::LABA32F, ColorType::LABW32F,
                            ColorType::LABAW32F> {
};

template <ColorType CT>
constexpr inline bool is_lab_v = is_lab<CT>::value;

template <ColorType CT>
struct is_lch
    : detail::is_color_type<CT, ColorType::LCH32F, ColorType::LCHA32F, ColorType::LCHW32F,
                            ColorType::LCHAW32F> {
};

template <ColorType CT>
constexpr inline bool is_lch_v = is_lch<CT>::value;

template <ColorType CT>
struct has_alpha
    : detail::is_color_type<CT, ColorType::RGBA8U, ColorType::RGBA32F,
                            ColorType::RGBAW32F, ColorType::LABA32F, ColorType::LABAW32F,
                            ColorType::LCHA32F, ColorType::LCHAW32F> {
};

template <ColorType CT>
constexpr inline bool has_alpha_v = has_alpha<CT>::value;

template <ColorType CT>
struct has_weight
    : detail::is_color_type<CT, ColorType::RGBW32F, ColorType::RGBAW32F,
                            ColorType::LABW32F, ColorType::LABAW32F, ColorType::LCHW32F,
                            ColorType::LCHAW32F> {
};

template <ColorType CT>
constexpr inline bool has_weight_v = has_weight<CT>::value;

template <ColorType CT>
struct add_alpha {
	static_assert(has_alpha_v<CT>, "Missing specialization for this type.");
	static constexpr ColorType value = CT;
};

template <ColorType CT>
constexpr inline ColorType add_alpha_v = add_alpha<CT>::value;

template <>
struct add_alpha<ColorType::RGB8U> {
	static constexpr ColorType value = ColorType::RGBA8U;
};

template <>
struct add_alpha<ColorType::RGB32F> {
	static constexpr ColorType value = ColorType::RGBA32F;
};

template <>
struct add_alpha<ColorType::RGBW32F> {
	static constexpr ColorType value = ColorType::RGBAW32F;
};

template <>
struct add_alpha<ColorType::LAB32F> {
	static constexpr ColorType value = ColorType::LABA32F;
};

template <>
struct add_alpha<ColorType::LABW32F> {
	static constexpr ColorType value = ColorType::LABAW32F;
};

template <>
struct add_alpha<ColorType::LCH32F> {
	static constexpr ColorType value = ColorType::LCHA32F;
};

template <>
struct add_alpha<ColorType::LCHW32F> {
	static constexpr ColorType value = ColorType::LCHAW32F;
};

template <ColorType CT>
struct remove_alpha {
	static_assert(!has_alpha_v<CT>, "Missing specialization for this type.");
	static constexpr ColorType value = CT;
};

template <ColorType CT>
constexpr inline ColorType remove_alpha_v = remove_alpha<CT>::value;

template <>
struct remove_alpha<ColorType::RGBA8U> {
	static constexpr ColorType value = ColorType::RGB8U;
};

template <>
struct remove_alpha<ColorType::RGBA32F> {
	static constexpr ColorType value = ColorType::RGB32F;
};

template <>
struct remove_alpha<ColorType::RGBAW32F> {
	static constexpr ColorType value = ColorType::RGBW32F;
};

template <>
struct remove_alpha<ColorType::LABA32F> {
	static constexpr ColorType value = ColorType::LAB32F;
};

template <>
struct remove_alpha<ColorType::LABAW32F> {
	static constexpr ColorType value = ColorType::LABW32F;
};

template <>
struct remove_alpha<ColorType::LCHA32F> {
	static constexpr ColorType value = ColorType::LCH32F;
};

template <>
struct remove_alpha<ColorType::LCHAW32F> {
	static constexpr ColorType value = ColorType::LCHW32F;
};

template <ColorType CT>
struct add_weight {
	static_assert(has_weight_v<CT>, "Missing specialization for this type.");
	static constexpr ColorType value = CT;
};

template <ColorType CT>
constexpr inline ColorType add_weight_v = add_weight<CT>::value;

template <>
struct add_weight<ColorType::RGB32F> {
	static constexpr ColorType value = ColorType::RGBW32F;
};

template <>
struct add_weight<ColorType::RGBA32F> {
	static constexpr ColorType value = ColorType::RGBAW32F;
};

template <>
struct add_weight<ColorType::LAB32F> {
	static constexpr ColorType value = ColorType::LABW32F;
};

template <>
struct add_weight<ColorType::LABA32F> {
	static constexpr ColorType value = ColorType::LABAW32F;
};

template <>
struct add_weight<ColorType::LCH32F> {
	static constexpr ColorType value = ColorType::LCHW32F;
};

template <>
struct add_weight<ColorType::LCHA32F> {
	static constexpr ColorType value = ColorType::LCHAW32F;
};

template <ColorType CT>
struct remove_weight {
	static_assert(!has_weight_v<CT>, "Missing specialization for this type.");
	static constexpr ColorType value = CT;
};

template <ColorType CT>
constexpr inline ColorType remove_weight_v = remove_weight<CT>::value;

template <>
struct remove_weight<ColorType::RGBW32F> {
	static constexpr ColorType value = ColorType::RGB32F;
};

template <>
struct remove_weight<ColorType::RGBAW32F> {
	static constexpr ColorType value = ColorType::RGBA32F;
};

template <>
struct remove_weight<ColorType::LABW32F> {
	static constexpr ColorType value = ColorType::LAB32F;
};

template <>
struct remove_weight<ColorType::LABAW32F> {
	static constexpr ColorType value = ColorType::LABA32F;
};

template <>
struct remove_weight<ColorType::LCHW32F> {
	static constexpr ColorType value = ColorType::LCH32F;
};

template <>
struct remove_weight<ColorType::LCHAW32F> {
	static constexpr ColorType value = ColorType::LCHA32F;
};

template <ColorType CT>
struct add_alpha_weight {
	static constexpr ColorType value = add_weight_v<add_alpha_v<CT>>;
};

template <ColorType CT>
constexpr inline ColorType add_alpha_weight_v = add_alpha_weight<CT>::value;

template <ColorType CT>
struct remove_alpha_weight {
	static constexpr ColorType value = remove_weight_v<remove_alpha_v<CT>>;
};

template <ColorType CT>
constexpr inline ColorType remove_alpha_weight_v = remove_alpha_weight<CT>::value;

// template <ColorType CT>
// struct make_float {
// 	static constexpr ColorType value = CT;
// };

// template <ColorType CT>
// constexpr inline ColorType make_float_v = make_float<CT>::value;

// template <>
// struct make_float<ColorType::RGB8U> {
// 	static constexpr ColorType value = ColorType::RGB32F;
// };

// template <>
// struct make_float<ColorType::RGBA8U> {
// 	static constexpr ColorType value = ColorType::RGBA32F;
// };
}  // namespace ufo::color

#endif  // UFO_VISION_DETAIL_COLOR_TYPE_TRAITS_HPP