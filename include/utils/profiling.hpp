#ifndef __PROFILING_HPP__
#define __PROFILING_HPP__

#include <ostream>
#include "sysutils.hpp"
#include "singleton.hpp"

namespace mhe {
namespace utils {

class ProfilingElement;

class ProfilingHandler
{
public:
	virtual ~ProfilingHandler() {}
	virtual void on_element_completed(const ProfilingElement* element) = 0;
};

class ProfilingElement
{
public:
	ProfilingElement(const std::string& name, ProfilingHandler* handler) :
		name_(name),
		start_time_(get_current_tick()),
		handler_(handler)
	{}

	~ProfilingElement()
	{
		handler_->on_element_completed(this);
	}

	const std::string& name() const
	{
		return name_;
	}

	cmn::uint interval() const
	{
		return get_current_tick() - start_time_;
	}
private:
	std::string name_;
	cmn::uint start_time_;
	ProfilingHandler* handler_;
};

class Profiler : public ProfilingHandler, public Singleton<Profiler>
{
	friend class Singleton<Profiler>;
public:
	struct ProfilingData
	{
		size_t count;
		cmn::uint interval;

		ProfilingData() : count(0), interval(0) {}
	};

	void on_element_completed(const ProfilingElement* element)
	{
		ProfilingData& data = elements_[element->name()]; 
		++data.count;
		data.interval += element->interval();
	}

	void reset()
	{
		elements_.clear();
	}

	std::vector< std::pair<std::string, ProfilingData> > data() const
	{
		std::vector< std::pair<std::string, ProfilingData> > d;
		d.reserve(elements_.size());
		for (std::map<std::string, ProfilingData>::const_iterator it = elements_.begin();
			 it != elements_.end(); ++it)
			d.push_back(std::make_pair(it->first, it->second));
		return d;
	}
private:
	Profiler() {}
	~Profiler() {}
private:
	std::map<std::string, ProfilingData> elements_;
};

#define ADD_PROFILE_ELEMENT(name) mhe::utils::ProfilingElement(name, &(mhe::utils::Profiler::instance()));

inline std::ostream& operator<< (std::ostream& stream, const Profiler& profiler)
{
	const std::vector< std::pair<std::string, Profiler::ProfilingData> >& data = profiler.data();
	stream << "{\n";
	for (size_t i = 0; i < data.size(); ++i)
	{
		float count = data[i].second.count;
		float interval = data[i].second.interval;
		stream << "\t" << data[i].first << " " << interval / count << " " << interval << " " << count << "\n";
	}
	stream << "}\n";
	return stream;
}

}}

#endif
