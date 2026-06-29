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

#ifndef UFO_TIME_TIMER_HPP
#define UFO_TIME_TIMER_HPP

// STL
#include <chrono>
#include <cmath>

namespace ufo
{
class Timer
{
 public:
	void start();

	void pause();

	void resume();

	void reset();

	void resetCurrent();

	void stop();

	/*!
	 * @brief
	 *
	 * @note This will stop `rhs` (if it is running or paused) to add another sample.
	 *
	 * @param rhs
	 * @return Timer&
	 */
	Timer& operator+=(Timer rhs);

	/*!
	 * @brief
	 *
	 * @note This will stop both `lhs` and `rhs` (if running or paused) to add another
	 * sample.
	 *
	 * @param rhs
	 * @return Timer&
	 */
	friend Timer operator+(Timer lhs, Timer rhs);

	/*!
	 * @brief
	 *
	 * @note This will stop the `rhs` timer (if it is running or paused) to add another
	 * sample.
	 *
	 * @param rhs
	 * @return Timer&
	 */
	Timer& operator-=(Timer rhs);

	/*!
	 * @brief
	 *
	 * @note This will stop both `lhs` and `rhs` (if running or paused) to add another
	 * sample.
	 *
	 * @param rhs
	 * @return Timer&
	 */
	friend Timer operator-(Timer lhs, Timer rhs);

	[[nodiscard]] bool running() const;

	[[nodiscard]] bool paused() const;

	template <class Period = std::chrono::seconds::period>
	[[nodiscard]] double current() const
	{
		return toDouble<Period>(
		    running() ? current_ + (std::chrono::high_resolution_clock::now() - start_)
		              : current_);
	}

	[[nodiscard]] double currentSeconds() const;

	[[nodiscard]] double currentMilliseconds() const;

	[[nodiscard]] double currentMicroseconds() const;

	[[nodiscard]] double currentNanoseconds() const;

	template <class Period = std::chrono::seconds::period>
	[[nodiscard]] double last() const
	{
		return toDouble<Period>(last_);
	}

	[[nodiscard]] double lastSeconds() const;

	[[nodiscard]] double lastMilliseconds() const;

	[[nodiscard]] double lastMicroseconds() const;

	[[nodiscard]] double lastNanoseconds() const;

	template <class Period = std::chrono::seconds::period>
	[[nodiscard]] double total() const
	{
		return current<Period>() + toDouble<Period>(total_);
	}

	[[nodiscard]] double totalSeconds() const;

	[[nodiscard]] double totalMilliseconds() const;

	[[nodiscard]] double totalMicroseconds() const;

	[[nodiscard]] double totalNanoseconds() const;

	template <class Period = std::chrono::seconds::period>
	[[nodiscard]] double min() const
	{
		return 0 < numSamples() ? toDouble<Period>(min_)
		                        : std::numeric_limits<double>::quiet_NaN();
	}

	[[nodiscard]] double minSeconds() const;

	[[nodiscard]] double minMilliseconds() const;

	[[nodiscard]] double minMicroseconds() const;

	[[nodiscard]] double minNanoseconds() const;

	template <class Period = std::chrono::seconds::period>
	[[nodiscard]] double max() const
	{
		return 0 < numSamples() ? toDouble<Period>(max_)
		                        : std::numeric_limits<double>::quiet_NaN();
	}

	[[nodiscard]] double maxSeconds() const;

	[[nodiscard]] double maxMilliseconds() const;

	[[nodiscard]] double maxMicroseconds() const;

	[[nodiscard]] double maxNanoseconds() const;

	template <class Period = std::chrono::seconds::period>
	[[nodiscard]] double mean() const
	{
		return toDouble<Period>(mean_);
	}

	[[nodiscard]] double meanSeconds() const;

	[[nodiscard]] double meanMilliseconds() const;

	[[nodiscard]] double meanMicroseconds() const;

	[[nodiscard]] double meanNanoseconds() const;

	template <class Period = std::chrono::seconds::period>
	[[nodiscard]] double variance() const
	{
		return populationVariance<Period>();
	}

	[[nodiscard]] double varianceSeconds() const;

	[[nodiscard]] double varianceMilliseconds() const;

	[[nodiscard]] double varianceMicroseconds() const;

	[[nodiscard]] double varianceNanoseconds() const;

	template <class Period = std::chrono::seconds::period>
	[[nodiscard]] double std() const
	{
		return std::sqrt(variance<Period>());
	}

	[[nodiscard]] double stdSeconds() const;

	[[nodiscard]] double stdMilliseconds() const;

	[[nodiscard]] double stdMicroseconds() const;

	[[nodiscard]] double stdNanoseconds() const;

	template <class Period = std::chrono::seconds::period>
	[[nodiscard]] double sampleVariance() const
	{
		constexpr long double s =
		    static_cast<long double>(Period::den) / static_cast<long double>(Period::num);
		return 1 < numSamples()
		           ? static_cast<double>(s * s * (sum_squares_diffs_ / (numSamples() - 1)))
		           : std::numeric_limits<double>::quiet_NaN();
	}

	[[nodiscard]] double sampleVarianceSeconds() const;

	[[nodiscard]] double sampleVarianceMilliseconds() const;

	[[nodiscard]] double sampleVarianceMicroseconds() const;

	[[nodiscard]] double sampleVarianceNanoseconds() const;

	template <class Period = std::chrono::seconds::period>
	[[nodiscard]] double populationVariance() const
	{
		constexpr long double s =
		    static_cast<long double>(Period::den) / static_cast<long double>(Period::num);
		return 0 < numSamples()
		           ? static_cast<double>(s * s * (sum_squares_diffs_ / numSamples()))
		           : std::numeric_limits<double>::quiet_NaN();
	}

	[[nodiscard]] double populationVarianceSeconds() const;

	[[nodiscard]] double populationVarianceMilliseconds() const;

	[[nodiscard]] double populationVarianceMicroseconds() const;

	[[nodiscard]] double populationVarianceNanoseconds() const;

	[[nodiscard]] int numSamples() const;

 protected:
	void start(std::chrono::time_point<std::chrono::high_resolution_clock> time);

	void stop(std::chrono::time_point<std::chrono::high_resolution_clock> time);

	void addSample(std::chrono::time_point<std::chrono::high_resolution_clock> start,
	               std::chrono::time_point<std::chrono::high_resolution_clock> stop);

 private:
	template <class Period, class Duration>
	[[nodiscard]] static constexpr double toDouble(Duration dur)
	{
		return std::chrono::duration<double, Period>(dur).count();
	}

 protected:
	std::chrono::time_point<std::chrono::high_resolution_clock> start_ = {};
	// Used for pause/resume
	std::chrono::high_resolution_clock::duration current_ =
	    std::chrono::high_resolution_clock::duration::zero();

 private:
	int                                                         samples_         = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_time_point_ = {};
	std::chrono::high_resolution_clock::duration                last_ =
	    std::chrono::high_resolution_clock::duration::zero();
	std::chrono::high_resolution_clock::duration total_ =
	    std::chrono::high_resolution_clock::duration::zero();
	std::chrono::duration<double, std::chrono::high_resolution_clock::period> mean_ =
	    std::chrono::duration<double, std::chrono::high_resolution_clock::period>::zero();
	double                                       sum_squares_diffs_ = 0.0;
	std::chrono::high_resolution_clock::duration min_ =
	    std::chrono::high_resolution_clock::duration::max();
	std::chrono::high_resolution_clock::duration max_ =
	    std::chrono::high_resolution_clock::duration::min();

	friend class Timing;
};
}  // namespace ufo

#endif  // UFO_TIME_TIMER_HPP