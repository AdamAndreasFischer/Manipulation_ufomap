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

#ifndef UFO_VISION_COLOR_HPP
#define UFO_VISION_COLOR_HPP

// UFO
#include <ufo/math/math.hpp>
#include <ufo/vision/color_delta_e.hpp>
#include <ufo/vision/color_space.hpp>
#include <ufo/vision/color_type.hpp>
#include <ufo/vision/detail/color.hpp>
#include <ufo/vision/detail/color_type_traits.hpp>
#include <ufo/vision/lab.hpp>
#include <ufo/vision/lch.hpp>
#include <ufo/vision/rgb.hpp>

// STL
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <tuple>

namespace ufo
{
template <ColorType CT, std::enable_if_t<color::is_rgb_v<CT>, bool> = true>
[[nodiscard]] Color<CT> toLinearRGB(Color<CT> color)
{
	using T = decltype(Color<CT>::red);

	float red   = detail::convertRGBComponent<float>(color.red);
	float green = detail::convertRGBComponent<float>(color.green);
	float blue  = detail::convertRGBComponent<float>(color.blue);

	if constexpr (color::has_weight_v<CT>) {
		red /= color.weight;
		green /= color.weight;
		blue /= color.weight;
	}

	red   = detail::toLinearRGBComponent(red);
	green = detail::toLinearRGBComponent(green);
	blue  = detail::toLinearRGBComponent(blue);

	if constexpr (color::has_weight_v<CT>) {
		red *= color.weight;
		green *= color.weight;
		blue *= color.weight;
	}

	color.red   = detail::convertRGBComponent<T>(red);
	color.green = detail::convertRGBComponent<T>(green);
	color.blue  = detail::convertRGBComponent<T>(blue);

	return color;
}

template <ColorType CT, std::enable_if_t<color::is_rgb_v<CT>, bool> = true>
[[nodiscard]] Color<CT> fromLinearRGB(Color<CT> color)
{
	using T = decltype(Color<CT>::red);

	float red   = detail::convertRGBComponent<float>(color.red);
	float green = detail::convertRGBComponent<float>(color.green);
	float blue  = detail::convertRGBComponent<float>(color.blue);

	if constexpr (color::has_weight_v<CT>) {
		red /= color.weight;
		green /= color.weight;
		blue /= color.weight;
	}

	red   = detail::fromLinearRGBComponent(red);
	green = detail::fromLinearRGBComponent(green);
	blue  = detail::fromLinearRGBComponent(blue);

	if constexpr (color::has_weight_v<CT>) {
		red *= color.weight;
		green *= color.weight;
		blue *= color.weight;
	}

	color.red   = detail::convertRGBComponent<T>(red);
	color.green = detail::convertRGBComponent<T>(green);
	color.blue  = detail::convertRGBComponent<T>(blue);

	return color;
}

template <ColorType CT, std::enable_if_t<!color::has_alpha_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<color::add_alpha_v<CT>> addAlpha(Color<CT> const& color)
{
	Color<color::add_alpha_v<CT>> res;

	if constexpr (color::is_rgb_v<CT>) {
		res.red   = color.red;
		res.green = color.green;
		res.blue  = color.blue;
	} else if constexpr (color::is_lab_v<CT>) {
		res.lightness = color.lightness;
		res.a         = color.a;
		res.b         = color.b;
	} else if constexpr (color::is_lch_v<CT>) {
		res.lightness = color.lightness;
		res.chroma    = color.chroma;
		res.hue       = color.hue;
	}

	res.alpha = detail::maxAlpha(res.alpha);

	if constexpr (color::has_weight_v<CT>) {
		res.weight = color.weight;
		res.alpha *= res.weight;
	}

	return res;
}

template <ColorType CT, std::enable_if_t<color::has_alpha_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<CT> addAlpha(Color<CT> color)
{
	return color;
}

template <ColorType CT, std::enable_if_t<color::has_alpha_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<color::remove_alpha_v<CT>> removeAlpha(
    Color<CT> const& color)
{
	Color<color::remove_alpha_v<CT>> res;

	if constexpr (color::is_rgb_v<CT>) {
		res.red   = color.red;
		res.green = color.green;
		res.blue  = color.blue;
	} else if constexpr (color::is_lab_v<CT>) {
		res.lightness = color.lightness;
		res.a         = color.a;
		res.b         = color.b;
	} else if constexpr (color::is_lch_v<CT>) {
		res.lightness = color.lightness;
		res.chroma    = color.chroma;
		res.hue       = color.hue;
	}

	if constexpr (color::has_weight_v<CT>) {
		res.weight = color.weight;
	}

	return res;
}

template <ColorType CT, std::enable_if_t<!color::has_alpha_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<CT> removeAlpha(Color<CT> color)
{
	return color;
}

template <ColorType CT, std::enable_if_t<!color::has_weight_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<color::add_weight_v<CT>> addWeight(Color<CT> const& color)
{
	Color<color::add_weight_v<CT>> res;

	if constexpr (color::is_rgb_v<CT>) {
		res.red   = color.red;
		res.green = color.green;
		res.blue  = color.blue;
	} else if constexpr (color::is_lab_v<CT>) {
		res.lightness = color.lightness;
		res.a         = color.a;
		res.b         = color.b;
	} else if constexpr (color::is_lch_v<CT>) {
		res.lightness = color.lightness;
		res.chroma    = color.chroma;
		res.hue       = color.hue;
	}

	if constexpr (color::has_alpha_v<CT>) {
		res.alpha = color.alpha;
	}

	res.weight = detail::initWeight(res.weight);

	return res;
}

template <ColorType CT, std::enable_if_t<color::has_weight_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<CT> addWeight(Color<CT> color)
{
	return color;
}

template <ColorType CT, std::enable_if_t<color::has_weight_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<color::remove_weight_v<CT>> removeWeight(
    Color<CT> const& color)
{
	Color<color::remove_weight_v<CT>> res;

	if constexpr (color::is_rgb_v<CT>) {
		res.red   = color.red / color.weight;
		res.green = color.green / color.weight;
		res.blue  = color.blue / color.weight;
	} else if constexpr (color::is_lab_v<CT>) {
		res.lightness = color.lightness / color.weight;
		res.a         = color.a / color.weight;
		res.b         = color.b / color.weight;
	} else if constexpr (color::is_lch_v<CT>) {
		res.lightness = color.lightness / color.weight;
		res.chroma    = color.chroma / color.weight;
		res.hue       = color.hue / color.weight;
	}

	if constexpr (color::has_alpha_v<CT>) {
		res.alpha = color.alpha / color.weight;
	}

	return res;
}

template <ColorType CT, std::enable_if_t<!color::has_weight_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<CT> removeWeight(Color<CT> color)
{
	return color;
}

template <ColorType CT>
[[nodiscard]] constexpr auto alpha(Color<CT> const& color)
{
	if constexpr (color::has_alpha_v<CT>) {
		return color.alpha;
	} else if constexpr (color::is_rgb_v<CT>) {
		return detail::maxAlpha(color.red);
	} else if constexpr (color::is_lab_v<CT>) {
		return detail::maxAlpha(color.lightness);
	} else if constexpr (color::is_lch_v<CT>) {
		return detail::maxAlpha(color.lightness);
	} else {
		static_assert(dependent_false_v<Color<CT>>, "Not supported.");
	}
}

template <ColorType CT>
[[nodiscard]] constexpr float weight(Color<CT> const& color)
{
	if constexpr (color::has_weight_v<CT>) {
		return color.weight;
	} else {
		return 1.0f;
	}
}

template <ColorType CT>
[[nodiscard]] constexpr Color<CT> convert(Color<CT> color)
{
	return color;
}

template <ColorType To, ColorType From, std::enable_if_t<From != To, bool> = true>
[[nodiscard]] constexpr Color<To> convert(Color<From> const& color)
{
	if constexpr (color::has_weight_v<From>) {
		if (0.0f == color.weight) {
			return Color<To>{};
		}
	}

	Color<To> res;

	if constexpr (color::has_weight_v<From> && color::has_weight_v<To>) {
		res.weight = color.weight;
	} else if constexpr (color::has_weight_v<To>) {
		res.weight = detail::initWeight(res.weight);
	}

	auto const color_nw = removeWeight(color);

	if constexpr (color::is_rgb_v<From> && color::is_rgb_v<To>) {
		using T = decltype(res.red);

		res.red   = detail::convertRGBComponent<T>(color_nw.red);
		res.green = detail::convertRGBComponent<T>(color_nw.green);
		res.blue  = detail::convertRGBComponent<T>(color_nw.blue);
	} else if constexpr (color::is_rgb_v<From> && color::is_lab_v<To>) {
		// Make sure it is float
		float red   = detail::convertRGBComponent<float>(color_nw.red);
		float green = detail::convertRGBComponent<float>(color_nw.green);
		float blue  = detail::convertRGBComponent<float>(color_nw.blue);

		// Taken from: https://bottosson.github.io/posts/oklab/

		// clang-format off
		float l = 0.4122214708f * red + 0.5363325363f * green + 0.0514459929f * blue;
		float m = 0.2119034982f * red + 0.6806995451f * green + 0.1073969566f * blue;
		float s = 0.0883024619f * red + 0.2817188376f * green + 0.6299787005f * blue;

		l = std::cbrtf(l);
		m = std::cbrtf(m);
		s = std::cbrtf(s);

		res.lightness = 0.2104542553f * l + 0.7936177850f * m - 0.0040720468f * s;
		res.a         = 1.9779984951f * l - 2.4285922050f * m + 0.4505937099f * s;
		res.b         = 0.0259040371f * l + 0.7827717662f * m - 0.8086757660f * s;
		// clang-format on
	} else if constexpr (color::is_rgb_v<From> && color::is_lch_v<To>) {
		auto const lch =
		    convert<color::remove_alpha_weight_v<To>>(convert<ColorType::LAB32F>(color_nw));
		res.lightness = lch.lightness;
		res.chroma    = lch.chroma;
		res.hue       = lch.hue;
	} else if constexpr (color::is_lab_v<From> && color::is_lab_v<To>) {
		res.lightness = color_nw.lightness;
		res.a         = color_nw.a;
		res.b         = color_nw.b;
	} else if constexpr (color::is_lab_v<From> && color::is_rgb_v<To>) {
		// Taken from: https://bottosson.github.io/posts/oklab/

		// clang-format off
		float l = color_nw.lightness + 0.3963377774f * color_nw.a + 0.2158037573f * color_nw.b;
		float m = color_nw.lightness - 0.1055613458f * color_nw.a - 0.0638541728f * color_nw.b;
		float s = color_nw.lightness - 0.0894841775f * color_nw.a - 1.2914855480f * color_nw.b;

		l = l * l * l;
		m = m * m * m;
		s = s * s * s;

		float red   = +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
		float green = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
		float blue  = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;
		// clang-format on

		using T = decltype(Color<To>::red);

		res.red   = detail::convertRGBComponent<T>(red);
		res.green = detail::convertRGBComponent<T>(green);
		res.blue  = detail::convertRGBComponent<T>(blue);
	} else if constexpr (color::is_lab_v<From> && color::is_lch_v<To>) {
		res.lightness = color_nw.lightness;
		res.chroma    = std::sqrt(color_nw.a * color_nw.a + color_nw.b * color_nw.b);
		res.hue       = std::atan2(color_nw.b, color_nw.a);
	} else if constexpr (color::is_lch_v<From> && color::is_lch_v<To>) {
		res.lightness = color_nw.lightness;
		res.chroma    = color_nw.chroma;
		res.hue       = color_nw.hue;
	} else if constexpr (color::is_lch_v<From> && color::is_rgb_v<To>) {
		auto const rgb =
		    convert<color::remove_alpha_weight_v<To>>(convert<ColorType::LAB32F>(color_nw));
		res.red   = rgb.red;
		res.green = rgb.green;
		res.blue  = rgb.blue;
	} else if constexpr (color::is_lch_v<From> && color::is_lab_v<To>) {
		res.lightness = color_nw.lightness;
		res.a         = color_nw.chroma * std::cos(color_nw.hue);
		res.b         = color_nw.chroma * std::sin(color_nw.hue);
	} else {
		static_assert(dependent_false_v<Color<To>>, "Not supported.");
	}

	if constexpr (color::has_alpha_v<From> && color::has_alpha_v<To>) {
		using T = decltype(Color<To>::alpha);

		res.alpha = detail::convertAlpha<T>(color_nw.alpha);
	} else if constexpr (color::has_alpha_v<To>) {
		res.alpha = detail::maxAlpha(res.alpha);
	}

	if constexpr (color::has_weight_v<To>) {
		// Apply weight

		if constexpr (color::is_rgb_v<To>) {
			res.red *= res.weight;
			res.green *= res.weight;
			res.blue *= res.weight;
		} else if constexpr (color::is_lab_v<To>) {
			res.lightness *= res.weight;
			res.a *= res.weight;
			res.b *= res.weight;
		} else if constexpr (color::is_lch_v<To>) {
			res.lightness *= res.weight;
			res.chroma *= res.weight;
			res.hue *= res.weight;
		}

		if constexpr (color::has_alpha_v<To>) {
			res.alpha *= res.weight;
		}
	}

	return res;
}

template <class To, ColorType From, std::enable_if_t<color::is_color_v<To>, bool> = true>
[[nodiscard]] constexpr To convert(Color<From> const& color)
{
	return convert<color::color_type_v<To>>(color);
}

// Returns `a+t(b-a)`
template <ColorType CT>
[[nodiscard]] constexpr Color<CT> blend(Color<CT> a, Color<CT> const& b, float t)
{
	if constexpr (color::is_rgb_v<CT>) {
		using T = decltype(Color<CT>::red);

		a.red = static_cast<T>(lerp(static_cast<float>(a.red), static_cast<float>(b.red), t));
		a.green =
		    static_cast<T>(lerp(static_cast<float>(a.green), static_cast<float>(b.green), t));
		a.blue =
		    static_cast<T>(lerp(static_cast<float>(a.blue), static_cast<float>(b.blue), t));
	} else if constexpr (color::is_lab_v<CT>) {
		// Always float
		a.lightness = lerp(a.lightness, b.lightness, t);
		a.a         = lerp(a.a, b.a, t);
		a.b         = lerp(a.b, b.b, t);
	} else if constexpr (color::is_lch_v<CT>) {
		// Always float
		a.lightness = lerp(a.lightness, b.lightness, t);
		a.chroma    = lerp(a.chroma, b.chroma, t);
		a.hue       = lerp(a.hue, b.hue, t);
	}

	if constexpr (color::has_alpha_v<CT>) {
		using T = decltype(Color<CT>::alpha);

		a.alpha =
		    static_cast<T>(lerp(static_cast<float>(a.alpha), static_cast<float>(b.alpha), t));
	}

	if constexpr (color::has_weight_v<CT>) {
		// Always float
		a.weight = lerp(a.weight, b.weight, t);
	}

	return a;
}

template <class InputIt1, class InputIt2>
[[nodiscard]] constexpr auto blend(InputIt1 first_color, InputIt1 last_color,
                                   InputIt2 first_weight)
{
	using T                = typename std::iterator_traits<InputIt1>::value_type;
	constexpr ColorType CT = color::color_type_v<T>;

	if constexpr (color::is_rgb_v<CT>) {
		Color<ColorType::RGBAW32F> ret{};

		for (; last_color != first_color; ++first_color, first_weight) {
			float const w = static_cast<float>(*first_weight);
			auto const  c = convert<ColorType::RGBAW32F>(*first_color);
			ret.red += c.red * w;
			ret.green += c.green * w;
			ret.blue += c.blue * w;
			ret.alpha += c.alpha * w;
			ret.weight += c.weight * w;
		}

		return convert<CT>(ret);
	} else if constexpr (color::is_lab_v<CT>) {
		Color<color::add_weight_v<CT>> ret{};

		for (; last_color != first_color; ++first_color, first_weight) {
			float const w = static_cast<float>(*first_weight);
			auto const& c = *first_color;
			ret.lightness += c.lightness * w;
			ret.a += c.a * w;
			ret.b += c.b * w;
			if constexpr (color::has_alpha_v<CT>) {
				ret.alpha += c.alpha * w;
			}
			if constexpr (color::has_weight_v<CT>) {
				ret.weight += c.weight * w;
			} else {
				ret.weight += w;
			}
		}

		return convert<CT>(ret);
	} else if constexpr (color::is_lch_v<CT>) {
		Color<color::add_weight_v<CT>> ret{};

		for (; last_color != first_color; ++first_color, first_weight) {
			float const w = static_cast<float>(*first_weight);
			auto const& c = *first_color;
			ret.lightness += c.lightness * w;
			ret.chroma += c.chroma * w;
			ret.hue += c.hue * w;
			if constexpr (color::has_alpha_v<CT>) {
				ret.alpha += c.alpha * w;
			}
			if constexpr (color::has_weight_v<CT>) {
				ret.weight += c.weight * w;
			} else {
				ret.weight += w;
			}
		}

		return convert<CT>(ret);
	} else {
		static_assert(dependent_false_v<Color<CT>>, "Not supported.");
	}
}

template <ColorType CT>
[[nodiscard]] constexpr Color<CT> average(Color<CT> a, Color<CT> const& b)
{
	if constexpr (color::has_weight_v<CT>) {
		return a + b;
	} else {
		if constexpr (color::is_rgb_v<CT>) {
			using T = decltype(Color<CT>::red);

			a.red =
			    static_cast<T>((static_cast<float>(a.red) + static_cast<float>(b.red)) * 0.5f);
			a.green = static_cast<T>(
			    (static_cast<float>(a.green) + static_cast<float>(b.green)) * 0.5f);
			a.blue = static_cast<T>((static_cast<float>(a.blue) + static_cast<float>(b.blue)) *
			                        0.5f);
		} else if constexpr (color::is_lab_v<CT>) {
			// Always float
			a.lightness = (a.lightness + b.lightness) * 0.5f;
			a.a         = (a.a + b.a) * 0.5f;
			a.b         = (a.b + b.b) * 0.5f;
		} else if constexpr (color::is_lch_v<CT>) {
			// Always float
			a.lightness = (a.lightness + b.lightness) * 0.5f;
			a.chroma    = (a.chroma + b.chroma) * 0.5f;
			a.hue       = (a.hue + b.hue) * 0.5f;
		}

		if constexpr (color::has_alpha_v<CT>) {
			using T = decltype(Color<CT>::alpha);

			a.alpha = static_cast<T>(
			    (static_cast<float>(a.alpha) + static_cast<float>(b.alpha)) * 0.5f);
		}
	}

	return a;
}

template <class InputIt>
[[nodiscard]] constexpr auto average(InputIt first, InputIt last)
{
	using T                = typename std::iterator_traits<InputIt>::value_type;
	constexpr ColorType CT = color::color_type_v<T>;

	T res{};

	if constexpr (color::has_weight_v<CT>) {
		for (; last != first; ++first) {
			res += *first;
		}
	} else if constexpr (ColorType::RGB8U == CT) {
		Color<ColorType::RGB32F> tmp{};
		unsigned                 count{};
		for (; last != first; ++first, ++count) {
			tmp.red += static_cast<float>(first->red);
			tmp.green += static_cast<float>(first->green);
			tmp.blue += static_cast<float>(first->blue);
		}

		res.red   = static_cast<std::uint8_t>(tmp.red / static_cast<float>(count));
		res.green = static_cast<std::uint8_t>(tmp.green / static_cast<float>(count));
		res.blue  = static_cast<std::uint8_t>(tmp.blue / static_cast<float>(count));
	} else if constexpr (ColorType::RGBA8U == CT) {
		Color<ColorType::RGBA32F> tmp{};
		unsigned                  count{};
		for (; last != first; ++first, ++count) {
			tmp.red += static_cast<float>(first->red);
			tmp.green += static_cast<float>(first->green);
			tmp.blue += static_cast<float>(first->blue);
			tmp.alpha += static_cast<float>(first->alpha);
		}

		res.red   = static_cast<std::uint8_t>(tmp.red / static_cast<float>(count));
		res.green = static_cast<std::uint8_t>(tmp.green / static_cast<float>(count));
		res.blue  = static_cast<std::uint8_t>(tmp.blue / static_cast<float>(count));
		res.alpha = static_cast<std::uint8_t>(tmp.alpha / static_cast<float>(count));
	} else if constexpr (color::is_rgb_v<CT>) {
		unsigned count{};
		for (; last != first; ++first, ++count) {
			res.red += first->red;
			res.green += first->green;
			res.blue += first->blue;
			if constexpr (color::has_alpha_v<CT>) {
				res.alpha += static_cast<float>(first->alpha);
			}
		}

		res.red /= static_cast<float>(count);
		res.green /= static_cast<float>(count);
		res.blue /= static_cast<float>(count);
		if constexpr (color::has_alpha_v<CT>) {
			res.alpha /= static_cast<float>(count);
		}
	} else if constexpr (color::is_lab_v<CT>) {
		unsigned count{};
		for (; last != first; ++first, ++count) {
			res.lightness += first->lightness;
			res.a += first->a;
			res.b += first->b;
			if constexpr (color::has_alpha_v<CT>) {
				res.alpha += static_cast<float>(first->alpha);
			}
		}

		res.lightness /= static_cast<float>(count);
		res.a /= static_cast<float>(count);
		res.b /= static_cast<float>(count);
		if constexpr (color::has_alpha_v<CT>) {
			res.alpha /= static_cast<float>(count);
		}
	} else if constexpr (color::is_lch_v<CT>) {
		unsigned count{};
		for (; last != first; ++first, ++count) {
			res.lightness += first->lightness;
			res.chroma += first->chroma;
			res.hue += first->hue;
			if constexpr (color::has_alpha_v<CT>) {
				res.alpha += static_cast<float>(first->alpha);
			}
		}

		res.lightness /= static_cast<float>(count);
		res.chroma /= static_cast<float>(count);
		res.hue /= static_cast<float>(count);
		if constexpr (color::has_alpha_v<CT>) {
			res.alpha /= static_cast<float>(count);
		}
	} else {
		static_assert(dependent_false_v<T>, "Not supported.");
		;
	}

	return res;
}

template <class Container>
[[nodiscard]] constexpr auto average(Container const& c)
{
	using std::begin;
	using std::end;
	return average(begin(c), end(c));
}

template <ColorType CT>
[[nodiscard]] constexpr Color<CT> average(std::initializer_list<Color<CT>> ilist)
{
	using std::begin;
	using std::end;
	return average(begin(ilist), end(ilist));
}

template <ColorType CT>
[[nodiscard]] constexpr float deltaEEuclideanSquared(Color<CT> const& color,
                                                     Color<CT> const& sample)
{
	if constexpr (color::has_weight_v<CT>) {
		return deltaEEuclideanSquared(removeWeight(color), removeWeight(color));
	}

	if constexpr (color::is_rgb_v<CT>) {
		float const dr = static_cast<float>(color.red) - static_cast<float>(sample.red);
		float const dg = static_cast<float>(color.green) - static_cast<float>(sample.green);
		float const db = static_cast<float>(color.blue) - static_cast<float>(sample.blue);
		return dr * dr + dg * dg + db * db;
	} else if constexpr (color::is_lab_v<CT>) {
		float const dl = color.lightness - sample.lightness;
		float const da = color.a - sample.a;
		float const db = color.b - sample.b;
		return dl * dl + da * da + db * db;
	} else if constexpr (color::is_lch_v<CT>) {
		float const dl = color.lightness - sample.lightness;
		float const dc = color.chroma - sample.chroma;
		float const dh = color.hue - sample.hue;
		return dl * dl + dc * dc + dh * dh;
	} else {
		static_assert(dependent_false_v<Color<CT>>, "Not supported.");
	}
}

template <ColorType CT>
[[nodiscard]] constexpr float deltaEEuclidean(Color<CT> const& color,
                                              Color<CT> const& sample)
{
	return std::sqrt(deltaEEuclideanSquared(color, sample));
}

template <ColorType CT>
[[nodiscard]] constexpr float deltaEOkSquared(Color<CT> const& color,
                                              Color<CT> const& sample,
                                              float const      ab_scale = 2.0f)
{
	auto const c = convert<ColorType::LAB32F>(color);
	auto const s = convert<ColorType::LAB32F>(sample);

	// Original ab_scale was 1 but it was found later that slightly greater than 2 was
	// better by the OkLab author.
	// See: https://github.com/w3c/csswg-drafts/issues/6642#issuecomment-945714988

	float const dl = c.lightness - s.lightness;
	float const da = ab_scale * (c.a - s.a);
	float const db = ab_scale * (c.b - s.b);
	return dl * dl + da * da + db * db;
}

template <ColorType CT>
[[nodiscard]] constexpr float deltaEOk(Color<CT> const& color, Color<CT> const& sample,
                                       float const ab_scale = 2.0f)
{
	return std::sqrt(deltaESquared(color, sample, ab_scale));
}

template <ColorType CT>
[[nodiscard]] constexpr float deltaESquared(Color<CT> const& color,
                                            Color<CT> const& sample,
                                            ColorDeltaE method = ColorDeltaE::EUCLIDEAN,
                                            ColorSpace  space  = ColorSpace::NATIVE)
{
	switch (method) {
		case ColorDeltaE::EUCLIDEAN: {
			switch (space) {
				case ColorSpace::NATIVE: return deltaEEuclideanSquared(color, sample);
				case ColorSpace::RGB:
					return deltaEEuclideanSquared(convert<ColorType::RGB32F>(color),
					                              convert<ColorType::RGB32F>(sample));
				case ColorSpace::OKLAB:
					return deltaEEuclideanSquared(convert<ColorType::LAB32F>(color),
					                              convert<ColorType::LAB32F>(sample));
				case ColorSpace::OKLAB2: return deltaEOkSquared(color, sample);
				case ColorSpace::OKLCH:
					return deltaEEuclideanSquared(convert<ColorType::LCH32F>(color),
					                              convert<ColorType::LCH32F>(sample));
			}
		}
		case ColorDeltaE::OK: {
			return deltaEOkSquared(color, sample);
		}
	}
}

template <ColorType CT>
[[nodiscard]] constexpr float deltaE(Color<CT> const& color, Color<CT> const& sample,
                                     ColorDeltaE method = ColorDeltaE::EUCLIDEAN,
                                     ColorSpace  space  = ColorSpace::NATIVE)
{
	return std::sqrt(deltaESquared(color, sample, method, space));
}

template <ColorType CT, std::enable_if_t<color::has_weight_v<CT>, bool> = true>
constexpr Color<CT>& operator+=(Color<CT>& lhs, Color<CT> const& rhs)
{
	if constexpr (color::is_rgb_v<CT>) {
		lhs.red += rhs.red;
		lhs.green += rhs.green;
		lhs.blue += rhs.blue;
	} else if constexpr (color::is_lab_v<CT>) {
		lhs.lightness += rhs.lightness;
		lhs.a += rhs.a;
		lhs.b += rhs.b;
	} else if constexpr (color::is_lch_v<CT>) {
		lhs.lightness += rhs.lightness;
		lhs.chroma += rhs.chroma;
		lhs.hue += rhs.hue;
	}

	if constexpr (color::has_alpha_v<CT>) {
		lhs.alpha += rhs.alpha;
	}

	lhs.weight += rhs.weight;

	return lhs;
}

template <ColorType CT, std::enable_if_t<color::has_weight_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<CT> operator+(Color<CT> lhs, Color<CT> const& rhs)
{
	lhs += rhs;
	return lhs;
}

template <ColorType CT, std::enable_if_t<color::has_weight_v<CT>, bool> = true>
constexpr Color<CT>& operator-=(Color<CT>& lhs, Color<CT> const& rhs)
{
	if constexpr (color::is_rgb_v<CT>) {
		lhs.red -= rhs.red;
		lhs.green -= rhs.green;
		lhs.blue -= rhs.blue;
	} else if constexpr (color::is_lab_v<CT>) {
		lhs.lightness -= rhs.lightness;
		lhs.a -= rhs.a;
		lhs.b -= rhs.b;
	} else if constexpr (color::is_lch_v<CT>) {
		lhs.lightness -= rhs.lightness;
		lhs.chroma -= rhs.chroma;
		lhs.hue -= rhs.hue;
	}

	if constexpr (color::has_alpha_v<CT>) {
		lhs.alpha -= rhs.alpha;
	}

	lhs.weight -= rhs.weight;

	return lhs;
}

template <ColorType CT, std::enable_if_t<color::has_weight_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<CT> operator-(Color<CT> lhs, Color<CT> const& rhs)
{
	lhs -= rhs;
	return lhs;
}

template <ColorType CT, std::enable_if_t<color::has_weight_v<CT>, bool> = true>
constexpr Color<CT>& operator*=(Color<CT>& lhs, float rhs)
{
	if constexpr (color::is_rgb_v<CT>) {
		lhs.red *= rhs;
		lhs.green *= rhs;
		lhs.blue *= rhs;
	} else if constexpr (color::is_lab_v<CT>) {
		lhs.lightness *= rhs;
		lhs.a *= rhs;
		lhs.b *= rhs;
	} else if constexpr (color::is_lch_v<CT>) {
		lhs.lightness *= rhs;
		lhs.chroma *= rhs;
		lhs.hue *= rhs;
	}

	if constexpr (color::has_alpha_v<CT>) {
		lhs.alpha *= rhs;
	}

	lhs.weight *= rhs;

	return lhs;
}

template <ColorType CT, std::enable_if_t<color::has_weight_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<CT> operator*(Color<CT> lhs, float rhs)
{
	lhs *= rhs;
	return lhs;
}

template <ColorType CT, std::enable_if_t<color::has_weight_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<CT> operator*(float lhs, Color<CT> rhs)
{
	rhs *= lhs;
	return rhs;
}

template <ColorType CT, std::enable_if_t<color::has_weight_v<CT>, bool> = true>
constexpr Color<CT>& operator/=(Color<CT>& lhs, float rhs)
{
	if constexpr (color::is_rgb_v<CT>) {
		lhs.red /= rhs;
		lhs.green /= rhs;
		lhs.blue /= rhs;
	} else if constexpr (color::is_lab_v<CT>) {
		lhs.lightness /= rhs;
		lhs.a /= rhs;
		lhs.b /= rhs;
	} else if constexpr (color::is_lch_v<CT>) {
		lhs.lightness /= rhs;
		lhs.chroma /= rhs;
		lhs.hue /= rhs;
	}

	if constexpr (color::has_alpha_v<CT>) {
		lhs.alpha /= rhs;
	}

	lhs.weight /= rhs;

	return lhs;
}

template <ColorType CT, std::enable_if_t<color::has_weight_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<CT> operator/(Color<CT> lhs, float rhs)
{
	lhs /= rhs;
	return lhs;
}

template <ColorType CT, std::enable_if_t<color::has_weight_v<CT>, bool> = true>
[[nodiscard]] constexpr Color<CT> operator/(float lhs, Color<CT> rhs)
{
	if constexpr (color::is_rgb_v<CT>) {
		rhs.red   = lhs / rhs.red;
		rhs.green = lhs / rhs.green;
		rhs.blue  = lhs / rhs.blue;
	} else if constexpr (color::is_lab_v<CT>) {
		rhs.lightness = lhs / rhs.lightness;
		rhs.a         = lhs / rhs.a;
		rhs.b         = lhs / rhs.b;
	} else if constexpr (color::is_lch_v<CT>) {
		rhs.lightness = lhs / rhs.lightness;
		rhs.chroma    = lhs / rhs.chroma;
		rhs.hue       = lhs / rhs.hue;
	}

	if constexpr (color::has_alpha_v<CT>) {
		rhs.alpha = lhs / rhs.alpha;
	}

	rhs.weight = lhs / rhs.weight;

	return rhs;
}

template <ColorType CT>
constexpr bool operator==(Color<CT> const& lhs, Color<CT> const& rhs)
{
	auto f = [](auto const& x) {
		if constexpr (color::is_rgb_v<CT>) {
			return std::tie(x.red, x.green, x.blue);
		} else if constexpr (color::is_lab_v<CT>) {
			return std::tie(x.lightness, x.a, x.b);
		} else if constexpr (color::is_lch_v<CT>) {
			return std::tie(x.lightness, x.chroma, x.hue);
		}
	};

	bool res = f(lhs) == f(rhs);

	if constexpr (color::has_alpha_v<CT>) {
		res = res && lhs.alpha == rhs.alpha;
	}

	if constexpr (color::has_weight_v<CT>) {
		res = res && lhs.weight == rhs.weight;
	}

	return res;
}

template <ColorType CT>
constexpr bool operator!=(Color<CT> const& lhs, Color<CT> const& rhs)
{
	return !(lhs == rhs);
}
}  // namespace ufo

template <ufo::ColorType CT>
inline std::ostream& operator<<(std::ostream& out, ufo::Color<CT> const& c)
{
	if constexpr (ufo::color::is_rgb_v<CT>) {
		out << "Red: " << +c.red << " Green: " << +c.green << " Blue: " << +c.blue;
	} else if constexpr (ufo::color::is_lab_v<CT>) {
		out << "Lightness: " << c.lightness << " a: " << c.a << " b: " << c.b;
	} else if constexpr (ufo::color::is_lch_v<CT>) {
		out << "Lightness: " << c.lightness << " Chroma: " << c.chroma << " Hue: " << c.hue;
	} else {
		static_assert(ufo::dependent_false_v<ufo::Color<CT>>, "Not supported.");
	}

	if constexpr (ufo::color::has_alpha_v<CT>) {
		out << " Alpha: " << +c.alpha;
	}

	if constexpr (ufo::color::has_weight_v<CT>) {
		out << " Weight: " << c.weight;
	}
	return out;
}

#endif  // UFO_VISION_COLOR_HPP
