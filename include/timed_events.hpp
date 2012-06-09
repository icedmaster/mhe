#ifndef __TIMED_EVENTS_HPP__
#define __TIMED_EVENTS_HPP__

#include "types.hpp"
#include "mhe_utils.hpp"
#include <list>
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace mhe
{

class TimedEventBase
{
public:
	TimedEventBase(cmn::uint interval, bool start = true, bool repeat = false) :
		interval_(interval), repeat_(repeat)
	{
		start_tick_ = (start) ? utils::get_current_tick() : 0; 
	}
	
	virtual ~TimedEventBase() {}

	void start(cmn::uint tick)
	{
		start_tick_ = tick;
	}

	bool try_execute(cmn::uint tick)
	{
		if (!need_execute(tick)) return false;
		if (repeat_) start_tick_ = tick;
		else start_tick_ = 0;
		execute_impl();
		return true;
	}

	bool is_running() const
	{
		return start_tick_ != 0;
	}
private:
	bool need_execute(cmn::uint tick) const
	{
		if ( start_tick_ && (tick >= (start_tick_ + interval_)) )
			return true;
		return false;
	}

	virtual void execute_impl() = 0;

	cmn::uint interval_;
	cmn::uint start_tick_;
	bool repeat_;
};

// manager class
class TimedEventsManager
{
	typedef boost::shared_ptr<TimedEventBase> timed_event_ptr;
public:
	void add(timed_event_ptr event)
	{
		events_.push_back(event);
	}

	void add(TimedEventBase* event)
	{
		events_.push_back(timed_event_ptr(event));
	}

	void update(cmn::uint tick);
private:
	std::list<timed_event_ptr> events_;
};

}

#endif
