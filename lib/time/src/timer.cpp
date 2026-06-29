// UFO
#include <ufo/time/timer.hpp>

namespace ufo
{
//
// Public functions
//

void Timer::start() { start(std::chrono::high_resolution_clock::now()); }

void Timer::pause()
{
	current_ += std::chrono::high_resolution_clock::now() - start_;
	start_ = {};
}

void Timer::resume() { start(); }

void Timer::reset()
{
	start_           = {};
	current_         = std::chrono::high_resolution_clock::duration::zero();
	samples_         = 0;
	last_time_point_ = {};
	last_            = std::chrono::high_resolution_clock::duration::zero();
	total_           = std::chrono::high_resolution_clock::duration::zero();
	mean_ =
	    std::chrono::duration<double, std::chrono::high_resolution_clock::period>::zero();
	sum_squares_diffs_ = 0.0;
	min_               = std::chrono::high_resolution_clock::duration::max();
	max_               = std::chrono::high_resolution_clock::duration::min();
}

void Timer::resetCurrent()
{
	start_   = {};
	current_ = std::chrono::high_resolution_clock::duration::zero();
}

void Timer::stop() { stop(std::chrono::high_resolution_clock::now()); }

Timer& Timer::operator+=(Timer rhs)
{
	auto now = std::chrono::high_resolution_clock::now();
	if (rhs.running() || rhs.paused()) {
		rhs.stop(now);
	}

	if (last_time_point_ < rhs.last_time_point_) {
		last_time_point_ = rhs.last_time_point_;
		last_            = rhs.last_;
	}

	mean_ = 0 == samples_ + rhs.samples_
	            ? std::chrono::duration<double,
	                                    std::chrono::high_resolution_clock::period>::zero()
	            : (total_ + rhs.total_) / static_cast<double>(samples_ + rhs.samples_);

	samples_ += rhs.samples_;
	total_ += rhs.total_;
	sum_squares_diffs_ += rhs.sum_squares_diffs_;
	min_ = std::min(min_, rhs.min_);
	max_ = std::max(max_, rhs.max_);

	return *this;
}

Timer operator+(Timer lhs, Timer rhs)
{
	auto now = std::chrono::high_resolution_clock::now();
	if (lhs.running() || lhs.paused()) {
		lhs.stop(now);
	}
	if (rhs.running() || rhs.paused()) {
		rhs.stop(now);
	}

	lhs += rhs;
	return lhs;
}

Timer& Timer::operator-=(Timer rhs)
{
	auto now = std::chrono::high_resolution_clock::now();
	if (rhs.running() || rhs.paused()) {
		rhs.stop(now);
	}

	samples_ -= rhs.samples_;
	total_ -= rhs.total_;
	mean_ -= rhs.mean_;
	sum_squares_diffs_ -= rhs.sum_squares_diffs_;
	min_ -= std::min(min_, rhs.min_);
	max_ -= std::max(max_, rhs.max_);

	return *this;
}

Timer operator-(Timer lhs, Timer rhs)
{
	auto now = std::chrono::high_resolution_clock::now();
	if (lhs.running() || lhs.paused()) {
		lhs.stop(now);
	}
	if (rhs.running() || rhs.paused()) {
		rhs.stop(now);
	}

	lhs -= rhs;
	return lhs;
}

bool Timer::running() const
{
	return std::chrono::time_point<std::chrono::high_resolution_clock>{} != start_;
}

bool Timer::paused() const
{
	return !running() && std::chrono::high_resolution_clock::duration::zero() != current_;
}

double Timer::currentSeconds() const { return current<std::chrono::seconds::period>(); }

double Timer::currentMilliseconds() const
{
	return current<std::chrono::milliseconds::period>();
}

double Timer::currentMicroseconds() const
{
	return current<std::chrono::microseconds::period>();
}

double Timer::currentNanoseconds() const
{
	return current<std::chrono::nanoseconds::period>();
}

double Timer::lastSeconds() const { return last<std::chrono::seconds::period>(); }

double Timer::lastMilliseconds() const
{
	return last<std::chrono::milliseconds::period>();
}

double Timer::lastMicroseconds() const
{
	return last<std::chrono::microseconds::period>();
}

double Timer::lastNanoseconds() const { return last<std::chrono::nanoseconds::period>(); }

double Timer::totalSeconds() const { return total<std::chrono::seconds::period>(); }

double Timer::totalMilliseconds() const
{
	return total<std::chrono::milliseconds::period>();
}

double Timer::totalMicroseconds() const
{
	return total<std::chrono::microseconds::period>();
}

double Timer::totalNanoseconds() const
{
	return total<std::chrono::nanoseconds::period>();
}

double Timer::minSeconds() const { return min<std::chrono::seconds::period>(); }

double Timer::minMilliseconds() const { return min<std::chrono::milliseconds::period>(); }

double Timer::minMicroseconds() const { return min<std::chrono::microseconds::period>(); }

double Timer::minNanoseconds() const { return min<std::chrono::nanoseconds::period>(); }

double Timer::maxSeconds() const { return max<std::chrono::seconds::period>(); }

double Timer::maxMilliseconds() const { return max<std::chrono::milliseconds::period>(); }

double Timer::maxMicroseconds() const { return max<std::chrono::microseconds::period>(); }

double Timer::maxNanoseconds() const { return max<std::chrono::nanoseconds::period>(); }

double Timer::meanSeconds() const { return mean<std::chrono::seconds::period>(); }

double Timer::meanMilliseconds() const
{
	return mean<std::chrono::milliseconds::period>();
}

double Timer::meanMicroseconds() const
{
	return mean<std::chrono::microseconds::period>();
}

double Timer::meanNanoseconds() const { return mean<std::chrono::nanoseconds::period>(); }

double Timer::varianceSeconds() const { return variance<std::chrono::seconds::period>(); }

double Timer::varianceMilliseconds() const
{
	return variance<std::chrono::milliseconds::period>();
}

double Timer::varianceMicroseconds() const
{
	return variance<std::chrono::microseconds::period>();
}

double Timer::varianceNanoseconds() const
{
	return variance<std::chrono::nanoseconds::period>();
}

double Timer::stdSeconds() const { return std<std::chrono::seconds::period>(); }

double Timer::stdMilliseconds() const { return std<std::chrono::milliseconds::period>(); }

double Timer::stdMicroseconds() const { return std<std::chrono::microseconds::period>(); }

double Timer::stdNanoseconds() const { return std<std::chrono::nanoseconds::period>(); }

double Timer::sampleVarianceSeconds() const
{
	return sampleVariance<std::chrono::seconds::period>();
}

double Timer::sampleVarianceMilliseconds() const
{
	return sampleVariance<std::chrono::milliseconds::period>();
}

double Timer::sampleVarianceMicroseconds() const
{
	return sampleVariance<std::chrono::microseconds::period>();
}

double Timer::sampleVarianceNanoseconds() const
{
	return sampleVariance<std::chrono::nanoseconds::period>();
}

double Timer::populationVarianceSeconds() const
{
	return populationVariance<std::chrono::seconds::period>();
}

double Timer::populationVarianceMilliseconds() const
{
	return populationVariance<std::chrono::milliseconds::period>();
}

double Timer::populationVarianceMicroseconds() const
{
	return populationVariance<std::chrono::microseconds::period>();
}

double Timer::populationVarianceNanoseconds() const
{
	return populationVariance<std::chrono::nanoseconds::period>();
}

int Timer::numSamples() const { return samples_; }

//
// Private functions
//

void Timer::start(std::chrono::time_point<std::chrono::high_resolution_clock> time)
{
	start_ = time;
}

void Timer::stop(std::chrono::time_point<std::chrono::high_resolution_clock> time)
{
	last_time_point_ = time;

	last_ = paused() ? current_ : current_ + (time - start_);

	start_   = {};
	current_ = std::chrono::high_resolution_clock::duration::zero();

	++samples_;

	auto delta_1 = std::chrono::duration<double>(last_ - mean_);
	mean_ += delta_1 / samples_;
	auto delta_2 = std::chrono::duration<double>(last_ - mean_);
	sum_squares_diffs_ += toDouble<std::chrono::seconds::period>(delta_1) *
	                      toDouble<std::chrono::seconds::period>(delta_2);

	total_ += last_;
	min_ = std::min(min_, last_);
	max_ = std::max(max_, last_);
}

void Timer::addSample(std::chrono::time_point<std::chrono::high_resolution_clock> start,
                      std::chrono::time_point<std::chrono::high_resolution_clock> stop)
{
	auto elapsed = stop - start;

	if (last_time_point_ < stop) {
		last_time_point_ = stop;
		last_            = elapsed;
	}

	++samples_;

	auto delta_1 = std::chrono::duration<double>(elapsed - mean_);
	mean_ += delta_1 / samples_;
	auto delta_2 = std::chrono::duration<double>(elapsed - mean_);
	sum_squares_diffs_ += toDouble<std::chrono::seconds::period>(delta_1) *
	                      toDouble<std::chrono::seconds::period>(delta_2);

	total_ += elapsed;
	min_ = std::min(min_, elapsed);
	max_ = std::max(max_, elapsed);
}
}  // namespace ufo