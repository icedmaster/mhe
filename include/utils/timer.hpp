#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include "sysutils.hpp"

namespace mhe {
namespace utils {

class Timer
{
public:
	Timer(cmn::uint interval) :
		interval_(interval), start_tick_(0)
	{}

	void start()
	{
		start_tick_ = get_current_tick();
	}

	void stop()
	{
		start_tick_ = 0;
	}

	bool elapsed() const
	{
		if (!start_tick_) return false;
		return (get_current_tick() >= (start_tick_ + interval_));
	}
private:
	cmn::uint interval_;
	cmn::uint start_tick_;
};

}}

#endif
