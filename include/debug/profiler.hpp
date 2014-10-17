#ifndef __PROFILER_HPP__
#define __PROFILER_HPP__

#include <map>
#include <string>
#include <vector>
#include "core/hash.hpp"
#include "core/singleton.hpp"
#include "utils/sysutils.hpp"

namespace mhe {

class Profiler;
class ProfilerElement;

class Profiler
{
public:
	struct Data
	{
		std::string name;
		std::string data;
		size_t count;
		float interval;

		Data() : count(0), interval(0.0f) {}
	};
public:
	void start(ProfilerElement* element);
	void finish(ProfilerElement* element);

	void clear();

	std::vector<Profiler::Data> data() const;
private:
	void prepare_output_data(std::vector<Profiler::Data>& data) const;

	std::vector<Profiler::Data> data_;
#ifdef PROFILER_SAVE_LAST_FRAME_DATA
	std::vector<Profiler::Data> last_frame_data_;
#endif
};

class MainProfiler : public Profiler, public Singleton<MainProfiler>
{
	friend class Singleton<MainProfiler>;
private:
	MainProfiler() {}
	~MainProfiler() {}
};

class ProfilerElement
{
public:
	enum
	{
		single,
		add
	};
public:
	ProfilerElement(const char* name, int mode = add, Profiler& profiler = MainProfiler::instance(), const char* data = 0);
	~ProfilerElement();

	const char* name() const
	{
		return name_;
	}

	float result() const
	{
		return result_;
	}

	int mode() const
	{
		return mode_;
	}

	const char* data() const
	{
		return data_;
	}
private:
	utils::Timer timer_;
	float result_;
	const char* name_;
	Profiler& profiler_;
	int mode_;
	const char* data_;
};

#define PROFILE(name) ProfilerElement pe##__LINE__(name)

}

#endif
