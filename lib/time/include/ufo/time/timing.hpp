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

#ifndef UFO_TIME_TIMING_HPP
#define UFO_TIME_TIMING_HPP

// UFO
#include <ufo/time/timer.hpp>

// STL
#include <array>
#include <codecvt>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <mutex>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace ufo
{
class Timing
{
	using Mutex = std::mutex;

 public:
	Timing(std::string const& tag = "Total", char const* color = "");

	Timing(char const* tag, char const* color = "");

	template <class InputIt>
	Timing(std::string const& tag, InputIt first, InputIt last)
	    : Timing(tag, "", first, last)
	{
	}

	template <class InputIt>
	Timing(char const* tag, InputIt first, InputIt last)
	    : Timing(std::string(tag), first, last)
	{
	}

	template <class InputIt>
	Timing(std::string const& tag, char const* color, InputIt first, InputIt last)
	    : Timing(tag, color)
	{
		extend(first, last);
	}

	template <class InputIt>
	Timing(char const* tag, char const* color, InputIt first, InputIt last)
	    : Timing(std::string(tag), color, first, last)
	{
	}

	Timing(std::string const& tag, std::initializer_list<Timing> init);

	Timing(char const* tag, std::initializer_list<Timing> init);

	Timing(std::string const& tag, char const* color, std::initializer_list<Timing> init);

	Timing(char const* tag, char const* color, std::initializer_list<Timing> init);

	Timing& start();

	Timing& start(std::string const& tag);

	Timing& start(std::string const& tag, char const* color);

	bool stop();

	std::size_t stop(std::size_t levels);

	void stopAll();

	Timing const& operator[](std::string const& tag) const;

	Timing& operator[](std::string const& tag);

	void extend(Timing const& source);

	void extend(Timing&& source);

	template <class InputIt>
	void extend(InputIt first, InputIt last)
	{
		std::lock_guard lock(mutex_);
		for (; first != last; ++first) {
			extendImpl(*first);
		}
	}

	void extend(std::initializer_list<Timing> ilist);

	void merge(Timing const& source);

	void merge(Timing&& source);

	template <class InputIt>
	void merge(InputIt first, InputIt last)
	{
		std::lock_guard lock(mutex_);
		for (; first != last; ++first) {
			mergeImpl(*first);
		}
	}

	void merge(std::initializer_list<Timing> ilist);

	std::string const& tag() const;

	std::string const& color() const;

	void setColor(std::string const& color);

	static constexpr char const* resetColor() { return "\033[0m"; }
	static constexpr char const* blackColor() { return "\033[30m"; }
	static constexpr char const* redColor() { return "\033[31m"; }
	static constexpr char const* greenColor() { return "\033[32m"; }
	static constexpr char const* yellowColor() { return "\033[33m"; }
	static constexpr char const* blueColor() { return "\033[34m"; }
	static constexpr char const* magentaColor() { return "\033[35m"; }
	static constexpr char const* cyanColor() { return "\033[36m"; }
	static constexpr char const* whiteColor() { return "\033[37m"; }
	static constexpr char const* boldBlackColor() { return "\033[1m\033[30m"; }
	static constexpr char const* boldRedColor() { return "\033[1m\033[31m"; }
	static constexpr char const* boldGreenColor() { return "\033[1m\033[32m"; }
	static constexpr char const* boldYellowColor() { return "\033[1m\033[33m"; }
	static constexpr char const* boldBlueColor() { return "\033[1m\033[34m"; }
	static constexpr char const* boldMagentaColor() { return "\033[1m\033[35m"; }
	static constexpr char const* boldCyanColor() { return "\033[1m\033[36m"; }
	static constexpr char const* boldWhiteColor() { return "\033[1m\033[37m"; }

	template <class Period = std::chrono::seconds::period>
	void print(bool random_colors = false, bool bold = false, bool info = true,
	           int group_colors_level = std::numeric_limits<int>::max(),
	           int precision          = 4) const
	{
		print("", random_colors, bold, info, group_colors_level, precision);
	}

	template <class Period = std::chrono::seconds::period>
	void print(std::string const& name, bool random_colors = false, bool bold = false,
	           bool info = true, int group_colors_level = std::numeric_limits<int>::max(),
	           int precision = 4) const
	{
		using namespace std::string_literals;

		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		static constexpr std::array const RC{redColor(),  greenColor(),   yellowColor(),
		                                     blueColor(), magentaColor(), cyanColor(),
		                                     whiteColor()};

		std::wstring header_left =
		    L" " + (name.empty() ? L"Timings" : converter.from_bytes(name) + L" timings") +
		    L" in " + unit<Period>() + L" ";
		std::wstring header_right = L" UFO 🛸 ";

		// Left + right + seperator
		std::size_t header_length = header_left.length() + header_right.length() + 1;

		auto timers = timings();

		std::vector<std::pair<std::wstring, std::wstring>> component{{L" Component ", L""}};
		addTags(component, timers);

		std::size_t component_length{};
		for (auto const& [prefix, tag] : component) {
			component_length = std::max(component_length, prefix.length() + tag.length());
		}

		std::array data{
		    std::vector<std::string>{" Total "s}, std::vector<std::string>{" Last "s},
		    std::vector<std::string>{" Mean "s},  std::vector<std::string>{" Std dev "s},
		    std::vector<std::string>{" Min "s},   std::vector<std::string>{" Max "s}};
		std::array<std::function<double(Timing const&)>, data.size()> fun{
		    [](Timing const& t) { return t.timer_.total<Period>(); },
		    [](Timing const& t) { return t.timer_.last<Period>(); },
		    [](Timing const& t) { return t.timer_.mean<Period>(); },
		    [](Timing const& t) { return t.timer_.std<Period>(); },
		    [](Timing const& t) { return t.timer_.min<Period>(); },
		    [](Timing const& t) { return t.timer_.max<Period>(); }};

		for (std::size_t i{}; fun.size() > i; ++i) {
			addFloating<Period>(data[i], timers, precision, fun[i]);
		}

		std::array<std::size_t, data.size()> data_length;
		for (std::size_t i{}; data.size() > i; ++i) {
			data_length[i] = maxLength(data[i]);
		}

		std::vector<std::wstring> samples{L" Samples "};
		addNumSamples(samples, timers);
		std::size_t samples_length = maxLength(samples);

		std::vector<std::wstring> threads{L" Threads "};
		addNumThreads(threads, timers);
		std::size_t threads_length = maxLength(threads);

		std::size_t total_length =
		    std::accumulate(std::begin(data_length), std::end(data_length),
		                    component_length + 1 + samples_length + threads_length);
		total_length = std::max(total_length, header_length);

		{
			// Header
			std::size_t header_sep_pos = std::max(header_left.length(), total_length / 2);

			auto t_1 = converter.to_bytes(std::wstring(header_sep_pos, L'─'));
			auto t_2 =
			    converter.to_bytes(std::wstring(total_length - header_sep_pos - 1, L'─'));
			std::printf("╭%s┬%s╮\n", t_1.c_str(), t_2.c_str());
			t_1     = converter.to_bytes(header_left);
			t_2     = converter.to_bytes(header_right);
			int s_1 = static_cast<int>(header_sep_pos);
			int s_2 = total_length - header_sep_pos + 1;
			std::printf("│%-*s│%*s│\n", s_1, t_1.c_str(), s_2, t_2.c_str());
			if (component_length == header_sep_pos) {
				t_1 = converter.to_bytes(std::wstring(header_sep_pos, L'─'));
				t_2 = converter.to_bytes(std::wstring(total_length - header_sep_pos - 1, L'─'));
				std::printf("├%s┼%s┤\n", t_1.c_str(), t_2.c_str());
			} else if (component_length < header_sep_pos) {
				t_1 = converter.to_bytes(std::wstring(component_length, L'─'));
				t_2 =
				    converter.to_bytes(std::wstring(header_sep_pos - component_length - 1, L'─'));
				auto t_3 =
				    converter.to_bytes(std::wstring(total_length - header_sep_pos - 1, L'─'));
				std::printf("├%s┬%s┴%s┤\n", t_1.c_str(), t_2.c_str(), t_3.c_str());
			} else {
				t_1 = converter.to_bytes(std::wstring(header_sep_pos, L'─'));
				t_2 =
				    converter.to_bytes(std::wstring(component_length - header_sep_pos - 1, L'─'));
				auto t_3 =
				    converter.to_bytes(std::wstring(total_length - component_length - 1, L'─'));
				std::printf("├%s┴%s┬%s┤\n", t_1.c_str(), t_2.c_str(), t_3.c_str());
			}
		}

		{
			// Labels
			auto [left_pad, right_pad] = centeringPadding(component[0].first, component_length);
			std::printf("│%*s%s%*s│", left_pad, "",
			            converter.to_bytes(component[0].first).c_str(), right_pad, "");
			for (std::size_t i{0}; data.size() != i; ++i) {
				auto [left_pad, right_pad] = centeringPadding(data[i][0], data_length[i]);
				std::printf("%*s%s%*s", left_pad, "", data[i][0].c_str(), right_pad, "");
			}
			std::tie(left_pad, right_pad) = centeringPadding(samples[0], samples_length);
			std::printf("%*s%s%*s", left_pad, "", converter.to_bytes(samples[0]).c_str(),
			            right_pad, "");
			std::tie(left_pad, right_pad) = centeringPadding(threads[0], threads_length);
			std::printf("%*s%s%*s", left_pad, "", converter.to_bytes(threads[0]).c_str(),
			            right_pad, "");
			std::printf("│\n");
			auto t_1 = converter.to_bytes(std::wstring(component_length, L'─'));
			auto t_2 =
			    converter.to_bytes(std::wstring(total_length - component_length - 1, L'─'));
			std::printf("├%s┼%s┤\n", t_1.c_str(), t_2.c_str());
		}

		{
			// Data
			int rng_color = 0;
			for (std::size_t i{1}; component.size() > i; ++i) {
				rng_color += timers[i - 1].level <= group_colors_level;
				std::string color = bold ? "\033[1m" : "";
				color += random_colors ? RC[rng_color % RC.size()]
				                       : timers[i - 1].timing->color().c_str();

				std::printf("│");
				{
					// Component
					if (1 == i) {
						auto [left_pad, right_pad] =
						    centeringPadding(component[i].second, component_length);
						std::printf("%*s%s%s%*s", left_pad, "", color.c_str(),
						            converter.to_bytes(component[i].second).c_str(), right_pad, "");
					} else {
						auto prefix = converter.to_bytes(component[i].first);
						auto tag    = converter.to_bytes(component[i].second);
						int  s      = component_length - component[i].first.length() -
						        component[i].second.length();
						std::printf("%s%s%s%*s", prefix.c_str(), color.c_str(), tag.c_str(), s, "");
					}
					std::printf("%s│%s", resetColor(), color.c_str());
				}

				{
					// Other data
					for (std::size_t j{0}; data.size() > j; ++j) {
						auto [left_pad, right_pad] = centeringPadding(data[j][i], data_length[j]);
						if (" nan " == data[j][i]) {
							// Center aligned
							std::printf("%*s%s%*s", left_pad, "", data[j][i].c_str(), right_pad, "");
						} else {
							// Left aligned
							std::printf("%s%*s", data[j][i].c_str(), left_pad + right_pad, "");
						}
					}
					int s = static_cast<int>(samples_length - samples[i].length());
					std::printf("%s%*s", converter.to_bytes(samples[i]).c_str(), s, "");
					int t = static_cast<int>(threads_length - threads[i].length());
					std::printf("%s%*s", converter.to_bytes(threads[i]).c_str(), t, "");
				}

				// Reset color
				std::printf("%s│\n", resetColor());

				{
					// First seperator
					if (1 == i && component.size() > 2) {
						auto t_1 = converter.to_bytes(std::wstring(component_length, L'╌'));
						auto t_2 = converter.to_bytes(
						    std::wstring(total_length - component_length - 1, L'╌'));
						std::printf("├%s┼%s┤\n", t_1.c_str(), t_2.c_str());
					}
				}
			}
		}

		bool running    = false;
		bool paused     = false;
		bool concurrent = false;
		if (info) {
			// Info
			for (auto const& s : samples) {
				running = running || (2 <= s.length() && L'¹' == s[s.length() - 2]) ||
				          (3 <= s.length() && L'¹' == s[s.length() - 3]);
				paused = paused || (2 <= s.length() && L'²' == s[s.length() - 2]);
				if (running && paused) {
					break;
				}
			}

			for (auto const& [_, tag] : component) {
				if (2 <= tag.length() && L'³' == tag[tag.length() - 2]) {
					concurrent = true;
					break;
				}
			}

			if (running || concurrent) {
				auto t_1 = converter.to_bytes(std::wstring(component_length, L'─'));
				auto t_2 =
				    converter.to_bytes(std::wstring(total_length - component_length - 1, L'─'));
				std::printf("├%s┴%s┤\n", t_1.c_str(), t_2.c_str());
				if (running) {
					std::wstring info = L" ¹ # running threads that are not accounted for ";
					int          s    = static_cast<int>(total_length - info.length());
					std::printf("│%s%*s│\n", converter.to_bytes(info).c_str(), s, "");
				}
				if (paused) {
					std::wstring info = L" ² Indicates that the timer is paused ";
					int          s    = static_cast<int>(total_length - info.length());
					std::printf("│%s%*s│\n", converter.to_bytes(info).c_str(), s, "");
				}
				if (concurrent) {
					std::wstring info = L" ³ Indicates that the timer has run concurrently ";
					int          s    = static_cast<int>(total_length - info.length());
					std::printf("│%s%*s│\n", converter.to_bytes(info).c_str(), s, "");
				}
			}
		}

		{
			// Footer
			if (running || concurrent) {
				auto t = converter.to_bytes(std::wstring(total_length, L'─'));
				std::printf("╰%s╯\n", t.c_str());
			} else {
				auto t_1 = converter.to_bytes(std::wstring(component_length, L'─'));
				auto t_2 =
				    converter.to_bytes(std::wstring(total_length - component_length - 1, L'─'));
				std::printf("╰%s┴%s╯\n", t_1.c_str(), t_2.c_str());
			}
		}
	}

	void printSeconds(bool random_colors = false, bool bold = false, bool info = true,
	                  int group_colors_level = std::numeric_limits<int>::max(),
	                  int precision          = 4) const;

	void printSeconds(std::string const& name, bool random_colors = false,
	                  bool bold = false, bool info = true,
	                  int group_colors_level = std::numeric_limits<int>::max(),
	                  int precision          = 4) const;

	void printMilliseconds(bool random_colors = false, bool bold = false, bool info = true,
	                       int group_colors_level = std::numeric_limits<int>::max(),
	                       int precision          = 4) const;

	void printMilliseconds(std::string const& name, bool random_colors = false,
	                       bool bold = false, bool info = true,
	                       int group_colors_level = std::numeric_limits<int>::max(),
	                       int precision          = 4) const;

	void printMicroseconds(bool random_colors = false, bool bold = false, bool info = true,
	                       int group_colors_level = std::numeric_limits<int>::max(),
	                       int precision          = 4) const;

	void printMicroseconds(std::string const& name, bool random_colors = false,
	                       bool bold = false, bool info = true,
	                       int group_colors_level = std::numeric_limits<int>::max(),
	                       int precision          = 4) const;

	void printNanoseconds(bool random_colors = false, bool bold = false, bool info = true,
	                      int group_colors_level = std::numeric_limits<int>::max(),
	                      int precision          = 4) const;

	void printNanoseconds(std::string const& name, bool random_colors = false,
	                      bool bold = false, bool info = true,
	                      int group_colors_level = std::numeric_limits<int>::max(),
	                      int precision          = 4) const;

 private:
	Timing(Timing* parent, std::string const& tag);

	Timing(Timing* parent, std::string const& tag, std::string const& color);

	// Timing(Timing const& other);

	// Timing(Timing const& other, std::unique_lock<Mutex> rhs_lk);

	// Timing(Timing&& other);

	// Timing(Timing&& other, std::unique_lock<Mutex> rhs_lk);

	// Timing& operator=(Timing const& rhs);

	// Timing& operator=(Timing&& rhs);

	// friend void swap(Timing& a, Timing& b)
	// {
	// 	if (&a != &b) {
	// 		std::scoped_lock lock(a.mutex_, b.mutex_);
	// 		using std::swap;
	// 		swap(a.timings_, b.timings_);
	// 		swap(a.tag_, b.tag_);
	// 		swap(a.color_, b.color_);
	// 		swap(a.started_id_, b.started_id_);
	// 		swap(a.has_run_concurrent_, b.has_run_concurrent_);
	// 	}
	// }

	void lockParents();

	void unlockParents();

	Timing* findDeepest(std::thread::id id);

	std::size_t stop(std::chrono::time_point<std::chrono::high_resolution_clock> time,
	                 std::size_t                                                 levels);

	std::pair<std::size_t, std::chrono::high_resolution_clock::duration> stopRecurs(
	    std::thread::id                                             id,
	    std::chrono::time_point<std::chrono::high_resolution_clock> time,
	    std::size_t                                                 levels);

	struct TimingNL {
		Timing const* timing;
		std::size_t   num;
		int           level;

		TimingNL(Timing const* timing, std::size_t num, int level)
		    : timing(timing), num(num), level(level)
		{
		}
	};

	void extendImpl(Timing const& source);

	void extendImpl(Timing&& source);

	void mergeImpl(Timing const& source);

	void mergeImpl(Timing&& source);

	std::vector<TimingNL> timings() const;

	void timingsRecurs(std::vector<TimingNL>& data, std::size_t num, int level) const;

	int maxTagLength(std::vector<TimingNL> const& timers) const;

	void addTags(std::vector<std::pair<std::wstring, std::wstring>>& data,
	             std::vector<TimingNL> const&                        timers) const;

	template <class Period, class Fun>
	void addFloating(std::vector<std::string>& data, std::vector<TimingNL> const& timers,
	                 int precision, Fun f) const
	{
		std::stringstream ss;
		for (auto const& t : timers) {
			ss << std::fixed << std::setprecision(precision);
			ss << ' ' << f(*t.timing) << ' ';
			data.push_back(ss.str());
			ss = {};
		}
	}

	void addNumSamples(std::vector<std::wstring>&   data,
	                   std::vector<TimingNL> const& timers) const;

	void addNumThreads(std::vector<std::wstring>&   data,
	                   std::vector<TimingNL> const& timers) const;

	std::size_t maxLength(std::vector<std::string> const& data) const;

	std::size_t maxLength(std::vector<std::wstring> const& data) const;

	std::pair<int, int> centeringPadding(std::string const& str, int max_width) const;

	std::pair<int, int> centeringPadding(std::wstring const& str, int max_width) const;

	template <class Period>
	static std::wstring unit()
	{
		if constexpr (std::is_same_v<Period, std::chrono::nanoseconds::period>) {
			return L"nanoseconds (ns)";
		} else if constexpr (std::is_same_v<Period, std::chrono::microseconds::period>) {
			return L"microseconds (µs)";
		} else if constexpr (std::is_same_v<Period, std::chrono::milliseconds::period>) {
			return L"milliseconds (ms)";
		} else if constexpr (std::is_same_v<Period, std::chrono::seconds::period>) {
			return L"seconds (s)";
		} else if constexpr (std::is_same_v<Period, std::chrono::minutes::period>) {
			return L"minutes (min)";
		} else if constexpr (std::is_same_v<Period, std::chrono::hours::period>) {
			return L"hours (h)";
		} else {
			return L"[PERIOD/UNIT NOT SUPPORTED]";
		}
	}

	[[nodiscard]] int numSamples() const;

	void updateMaxConcurrent();

	[[nodiscard]] std::size_t numRunningThreads() const;

 private:
	struct SingleTimer {
		bool                                                        independent;
		std::chrono::time_point<std::chrono::high_resolution_clock> start;
		std::chrono::high_resolution_clock::duration                extra_time;
	};

	mutable Mutex                   mutex_;
	mutable std::unique_lock<Mutex> lock_;

	Timer                                  timer_;
	std::map<std::thread::id, SingleTimer> thread_;

	std::string tag_;
	std::string color_;

	Timing*                       parent_ = nullptr;
	std::map<std::string, Timing> children_;

	std::size_t max_concurrent_threads_ = 0;
};
}  // namespace ufo

#endif  // UFO_TIME_TIMING_HPP