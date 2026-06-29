// UFO
#include <ufo/time/timing.hpp>

// STL
#include <cassert>
#include <cmath>
#include <stack>

namespace ufo
{
//
// Public functions
//

Timing::Timing(std::string const& tag, char const* color) : Timing(nullptr, tag, color) {}

Timing::Timing(char const* tag, char const* color) : Timing(std::string(tag), color) {}

Timing::Timing(std::string const& tag, std::initializer_list<Timing> init)
    : Timing(tag, std::begin(init), std::end(init))
{
}

Timing::Timing(char const* tag, std::initializer_list<Timing> init)
    : Timing(std::string(tag), init)
{
}

Timing::Timing(std::string const& tag, char const* color,
               std::initializer_list<Timing> init)
    : Timing(tag, color, std::begin(init), std::end(init))
{
}

Timing::Timing(char const* tag, char const* color, std::initializer_list<Timing> init)
    : Timing(std::string(tag), color, init)
{
}

Timing& Timing::start()
{
	// TODO: Implement

	// auto start = std::chrono::high_resolution_clock::now();

	// lockParents();
	// lock_.lock();

	// auto id = std::this_thread::get_id();

	// std::vector<Timer*> timers;
	// timers.push_back(&threads_[id]);
	// if (auto it = running_ids_.find(id); std::end(running_ids_) != it) {
	// 	assert(!timers[0]->running());
	// 	timers[0]->start_   = timer_.start_;
	// 	timers[0]->current_ = timer_.current_;
	// 	running_ids_.erase(it);
	// 	if (running_ids_.empty()) {
	// 		timer_.resetCurrent();
	// 	}
	// }

	// updateMaxConcurrent();

	// for (auto p = parent_; nullptr != p; p = p->parent_) {
	// 	if (auto it = p->threads_.find(id); std::end(p->threads_) != it) {
	// 		timers.push_back(&(it->second));
	// 		continue;
	// 	} else {
	// 		p->running_ids_.insert(id);
	// 		if (1 == p->running_ids_.size()) {
	// 			timers.push_back(&(p->timer_));
	// 		}
	// 		p->updateMaxConcurrent();
	// 	}
	// }

	// auto finish  = std::chrono::high_resolution_clock::now();
	// auto elapsed = finish - start;

	// for (auto t : timers) {
	// 	if (t->running()) {
	// 		t->current_ -= elapsed;
	// 	} else {
	// 		t->start(finish);
	// 	}
	// }

	// lock_.unlock();
	// unlockParents();

	return *this;
}

Timing& Timing::start(std::string const& tag)
{
	auto start = std::chrono::high_resolution_clock::now();
	auto id    = std::this_thread::get_id();

	for (auto p = this; nullptr != p; p = p->parent_) {
		std::lock_guard lock(p->mutex_);
		auto [it, added] = thread_.try_emplace(id);
		if (added) {
			it->second.independent = false;
			it->second.start       = start;
			it->second.extra_time  = std::chrono::high_resolution_clock::duration::zero();
		}
		p->updateMaxConcurrent();
	}

	Timing* parent = this;
	if (0 < thread_.count(id)) {
		parent = findDeepest(id);
	}

	parent->lock_.lock();

	auto& new_timing = parent->children_.try_emplace(tag, tag).first->second;
	new_timing.lock_.lock();
	new_timing.parent_ = parent;

	parent->lock_.unlock();

	auto& st = new_timing.thread_[id];
	new_timing.updateMaxConcurrent();
	new_timing.lock_.unlock();

	st.independent = true;
	st.start       = start;
	st.extra_time  = std::chrono::high_resolution_clock::now() - start;

	return new_timing;
}

Timing& Timing::start(std::string const& tag, char const* color)
{
	auto& ret  = start(tag);
	ret.color_ = color;  // TODO: Should be counted in extra time
	return ret;
}

bool Timing::stop()
{
	// TODO: Implement correct

	auto time = std::chrono::high_resolution_clock::now();
	auto id   = std::this_thread::get_id();

	lock_.lock();
	if (0 == thread_.count(id)) {
		lock_.unlock();
		return false;
	}

	Timing* current = findDeepest(id);

	lock_.unlock();

	current->lock_.lock();

	auto& st = current->thread_[id];
	auto  et = st.extra_time;
	current->timer_.addSample(st.start + et, time);
	current->thread_.erase(id);

	Timing* parent = current->parent_;

	current->lock_.unlock();

	if (nullptr != parent) {
		parent->lock_.lock();
		auto& st = parent->thread_[id];
		parent->lock_.unlock();
		time -= st.extra_time + et;
		st.extra_time = std::chrono::high_resolution_clock::now() - time;
	}

	return true;

	// auto time = std::chrono::high_resolution_clock::now();
	// return 1 == stop(time, 1);
}

std::size_t Timing::stop(std::size_t levels)
{
	auto time = std::chrono::high_resolution_clock::now();
	return stop(time, levels);
}

void Timing::stopAll()
{
	auto time = std::chrono::high_resolution_clock::now();
	stop(time, std::numeric_limits<std::size_t>::max());
}

Timing const& Timing::operator[](std::string const& tag) const
{
	std::lock_guard lock(mutex_);
	return children_.at(tag);
}

Timing& Timing::operator[](std::string const& tag)
{
	std::lock_guard lock(mutex_);
	return children_[tag];
}

void Timing::extend(Timing const& source)
{
	std::lock_guard lock(mutex_);
	extendImpl(source);
}

void Timing::extend(Timing&& source)
{
	std::lock_guard lock(mutex_);
	extendImpl(std::move(source));
}

void Timing::extend(std::initializer_list<Timing> ilist)
{
	extend(std::begin(ilist), std::end(ilist));
}

void Timing::merge(Timing const& source)
{
	std::lock_guard lock(mutex_);
	mergeImpl(source);
}

void Timing::merge(Timing&& source)
{
	std::lock_guard lock(mutex_);
	mergeImpl(std::move(source));
}

void Timing::merge(std::initializer_list<Timing> ilist)
{
	merge(std::begin(ilist), std::end(ilist));
}

std::string const& Timing::tag() const { return tag_; }

std::string const& Timing::color() const { return color_; }

void Timing::setColor(std::string const& color) { color_ = color; }

void Timing::printSeconds(bool random_colors, bool bold, bool info,
                          int group_colors_level, int precision) const
{
	printSeconds("", random_colors, bold, info, group_colors_level, precision);
}

void Timing::printSeconds(std::string const& name, bool random_colors, bool bold,
                          bool info, int group_colors_level, int precision) const
{
	print<std::chrono::seconds::period>(name, random_colors, bold, info, group_colors_level,
	                                    precision);
}

void Timing::printMilliseconds(bool random_colors, bool bold, bool info,
                               int group_colors_level, int precision) const
{
	printMilliseconds("", random_colors, bold, info, group_colors_level, precision);
}

void Timing::printMilliseconds(std::string const& name, bool random_colors, bool bold,
                               bool info, int group_colors_level, int precision) const
{
	print<std::chrono::milliseconds::period>(name, random_colors, bold, info,
	                                         group_colors_level, precision);
}

void Timing::printMicroseconds(bool random_colors, bool bold, bool info,
                               int group_colors_level, int precision) const
{
	printMicroseconds("", random_colors, bold, info, group_colors_level, precision);
}

void Timing::printMicroseconds(std::string const& name, bool random_colors, bool bold,
                               bool info, int group_colors_level, int precision) const
{
	print<std::chrono::microseconds::period>(name, random_colors, bold, info,
	                                         group_colors_level, precision);
}

void Timing::printNanoseconds(bool random_colors, bool bold, bool info,
                              int group_colors_level, int precision) const
{
	printNanoseconds("", random_colors, bold, info, group_colors_level, precision);
}

void Timing::printNanoseconds(std::string const& name, bool random_colors, bool bold,
                              bool info, int group_colors_level, int precision) const
{
	print<std::chrono::nanoseconds::period>(name, random_colors, bold, info,
	                                        group_colors_level, precision);
}

//
// Private functions
//

Timing::Timing(Timing* parent, std::string const& tag)
    : lock_(mutex_, std::defer_lock), tag_(tag), color_(""), parent_(parent)
{
}

Timing::Timing(Timing* parent, std::string const& tag, std::string const& color)
    : lock_(mutex_, std::defer_lock), tag_(tag), color_(color), parent_(parent)
{
}

// Timing::Timing(Timing const& other) : Timing(other,
// std::unique_lock<Mutex>(other.mutex_))
// {
// }

// Timing::Timing(Timing const& other, std::unique_lock<Mutex> /* rhs_lk */)
//     : timings_(other.timings_)
//     , tag_(other.tag_)
//     , color_(other.color_)
//     , started_id_(other.started_id_)
//     , has_run_concurrent_(other.has_run_concurrent_)
// {
// }

// Timing::Timing(Timing&& other)
//     : Timing(std::move(other), std::unique_lock<Mutex>(other.mutex_))
// {
// }

// Timing::Timing(Timing&& other, std::unique_lock<Mutex> /* rhs_lk */)
//     : timings_(std::move(other.timings_))
//     , tag_(std::move(other.tag_))
//     , color_(std::move(other.color_))
//     , started_id_(std::move(other.started_id_))
//     , has_run_concurrent_(std::move(other.has_run_concurrent_))
// {
// }

// Timing& Timing::operator=(Timing const& rhs)
// {
// 	if (this != &rhs) {
// 		std::scoped_lock lock(mutex_, rhs.mutex_);
// 		timings_            = rhs.timings_;
// 		tag_                = rhs.tag_;
// 		color_              = rhs.color_;
// 		started_id_         = rhs.started_id_;
// 		has_run_concurrent_ = rhs.has_run_concurrent_;
// 	}
// 	return *this;
// }

// Timing& Timing::operator=(Timing&& rhs)
// {
// 	if (this != &rhs) {
// 		std::scoped_lock lock(mutex_, rhs.mutex_);
// 		timings_            = std::move(rhs.timings_);
// 		tag_                = std::move(rhs.tag_);
// 		color_              = std::move(rhs.color_);
// 		started_id_         = std::move(rhs.started_id_);
// 		has_run_concurrent_ = std::move(rhs.has_run_concurrent_);
// 	}
// 	return *this;
// }

void Timing::lockParents()
{
	std::stack<Timing*> parents;
	for (auto p = parent_; nullptr != p; p = p->parent_) {
		parents.push(p);
	}

	while (!parents.empty()) {
		auto p = parents.top();
		parents.pop();
		p->lock_.lock();
	}
}

void Timing::unlockParents()
{
	for (auto p = parent_; nullptr != p; p = p->parent_) {
		p->lock_.unlock();
	}
}

Timing* Timing::findDeepest(std::thread::id id)
{
	for (auto& [_, child] : children_) {
		std::lock_guard lock(child.mutex_);
		if (0 < child.thread_.count(id)) {
			return child.findDeepest(id);
		}
	}

	return this;
}

std::size_t Timing::stop(std::chrono::time_point<std::chrono::high_resolution_clock> time,
                         std::size_t levels)
{
	if (0 == levels) {
		return 0;
	}

	// lockParents();

	auto [stopped_levels, _] = stopRecurs(std::this_thread::get_id(), time, levels);

	// unlockParents();

	return stopped_levels;
}

std::pair<std::size_t, std::chrono::high_resolution_clock::duration> Timing::stopRecurs(
    std::thread::id /* id */,
    std::chrono::time_point<std::chrono::high_resolution_clock> /* time */,
    std::size_t /* levels */)
{
	return {};
	// std::lock_guard lock(mutex_);

	// std::size_t stopped_levels{};
	// for (auto& [_, child] : children_) {
	// 	if (levels <= stopped_levels) {
	// 		return {stopped_levels, {}};
	// 	}
	// 	if (0 < child.running_ids_.count(id) || 0 < child.start_.count(id)) {
	// 		auto [sl, et] = child.stopRecurs(id, time, levels - stopped_levels);
	// 		stopped_levels += sl;
	// 		extra_time_[id] += et;
	// 	}
	// }

	// std::chrono::high_resolution_clock::duration extra_time{};
	// if (stopped_levels < levels) {
	// 	if (auto it = running_ids_.find(id); std::end(running_ids_) != it) {
	// 		++stopped_levels;
	// 		running_ids_.erase(it);
	// 		if (running_ids_.empty()) {
	// 			timer_.stop(time);
	// 		}
	// 	} else if (auto it = start_.find(id); std::end(start_) != it) {
	// 		++stopped_levels;
	// 		it->second.current_ -= extra_time_[id];
	// 		extra_time += extra_time_[id];
	// 		extra_time_[id] = {};  // TODO: Remove?
	// 		it->second.stop(time);
	// 		timer_ += it->second;
	// 		start_.erase(it);
	// 	}
	// }

	// return {stopped_levels, extra_time};
}

void Timing::extendImpl(Timing const& /* source */)
{
	// TODO: Implement

	// for (auto& child : children_) {
	// 	if (child.tag() == source.tag()) {
	// 		child.merge(source);
	// 		return;
	// 	}
	// }

	// source.parent_ = this;
	// children_.push_back(source);
}

void Timing::extendImpl(Timing&& /* source */)
{
	// TODO: Implement

	// for (auto& child : children_) {
	// 	if (child.tag() == source.tag()) {
	// 		child.merge(std::move(source));
	// 		return;
	// 	}
	// }

	// source.parent_ = this;
	// children_.push_back(std::move(source));
}

void Timing::mergeImpl(Timing const& source)
{
	if (tag() != source.tag()) {
		extendImpl(source);
		return;
	}

	// TODO: Implement
}

void Timing::mergeImpl(Timing&& source)
{
	if (tag() != source.tag()) {
		extendImpl(std::move(source));
		return;
	}

	// TODO: Implement
}

std::vector<Timing::TimingNL> Timing::timings() const
{
	std::vector<TimingNL> data;
	data.emplace_back(this, 0, 0);

	int         level{};
	std::size_t i{1};
	for (auto& [_, t] : children_) {
		t.timingsRecurs(data, i, level);
		++i;
	}

	return data;
}

void Timing::timingsRecurs(std::vector<TimingNL>& data, std::size_t num, int level) const
{
	data.emplace_back(this, num, level);
	std::size_t i{1};
	for (auto& [_, t] : children_) {
		t.timingsRecurs(data, i, level + 1);
		++i;
	}
}

int Timing::maxTagLength(std::vector<TimingNL> const& timers) const
{
	int max{};
	for (auto const& t : timers) {
		max = std::max(max, 3 * t.level + static_cast<int>(t.timing->tag().length()) + 1);
	}
	return max;
}

void Timing::addTags(std::vector<std::pair<std::wstring, std::wstring>>& data,
                     std::vector<TimingNL> const&                        timers) const
{
	// TODO: Optimized

	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

	for (std::size_t i{}; timers.size() > i; ++i) {
		std::wstring prefix = L" ";
		// TODO: std::wstring tag_postfix = timers[i].timing->has_run_concurrent_ ? L"² " : L"
		// ";
		std::wstring tag_postfix = L" ";

		if (0 == timers[i].level) {
			data.emplace_back(prefix,
			                  converter.from_bytes(timers[i].timing->tag()) + tag_postfix);
			continue;
		}

		for (int level{}; timers[i].level - 1 > level; ++level) {
			bool found = false;
			for (std::size_t j = i + 1; timers.size() > j; ++j) {
				if (level == timers[j].level) {
					found = true;
					break;
				} else if (level > timers[j].level) {
					break;
				}
			}
			if (found) {
				prefix += L"   "; // L"│  "
			} else {
				prefix += L"   ";
			}
		}

		bool found = false;
		for (std::size_t j = i + 1; timers.size() > j; ++j) {
			if (timers[i].level == timers[j].level) {
				found = true;
				break;
			} else if (timers[i].level > timers[j].level) {
				break;
			}
		}
		if (found) {
			prefix += L"├─ ";
		} else {
			prefix += L"└─ ";  // L"╰─ "
		}

		data.emplace_back(prefix,
		                  converter.from_bytes(timers[i].timing->tag()) + tag_postfix);
	}
}

void Timing::addNumSamples(std::vector<std::wstring>&   data,
                           std::vector<TimingNL> const& timers) const
{
	for (auto const& t : timers) {
		auto nc = t.timing->numRunningThreads();
		if (0 == nc) {
			data.push_back(L" " + std::to_wstring(t.timing->numSamples()) + L" ");
		} else {
			data.push_back(L" " + std::to_wstring(t.timing->numSamples()) + L"+" +
			               std::to_wstring(nc) + L"¹ ");
		}
	}
}

void Timing::addNumThreads(std::vector<std::wstring>&   data,
                           std::vector<TimingNL> const& timers) const
{
	for (auto const& t : timers) {
		data.push_back(L" " + std::to_wstring(t.timing->numRunningThreads()) + L"/" +
		               std::to_wstring(t.timing->max_concurrent_threads_) + L" ");
	}
}

std::size_t Timing::maxLength(std::vector<std::string> const& data) const
{
	std::size_t max{};
	for (std::string const& s : data) {
		max = std::max(max, s.length());
	}
	return max;
}

std::size_t Timing::maxLength(std::vector<std::wstring> const& data) const
{
	std::size_t max{};
	for (std::wstring const& s : data) {
		max = std::max(max, s.length());
	}
	return max;
}

std::pair<int, int> Timing::centeringPadding(std::string const& str, int max_width) const
{
	int left_pad  = std::floor((max_width - static_cast<int>(str.length())) / 2.0);
	int right_pad = max_width - (left_pad + static_cast<int>(str.length()));
	return {left_pad, right_pad};
}

std::pair<int, int> Timing::centeringPadding(std::wstring const& str, int max_width) const
{
	int left_pad  = std::floor((max_width - static_cast<int>(str.length())) / 2.0);
	int right_pad = max_width - (left_pad + static_cast<int>(str.length()));
	return {left_pad, right_pad};
}

int Timing::numSamples() const { return timer_.numSamples(); }

void Timing::updateMaxConcurrent()
{
	max_concurrent_threads_ = std::max(max_concurrent_threads_, numRunningThreads());
}

std::size_t Timing::numRunningThreads() const { return thread_.size(); }
}  // namespace ufo