#ifndef __SYSTEM_EVENT_HPP__
#define __SYSTEM_EVENT_HPP__

#include "event.hpp"

namespace mhe {

class SystemEvent : public Event
{
public:
enum
{
	quit = 1,
	resize
};

public:
	SystemEvent(int system_type) :
		Event(system_event_type, system_type)
	{}

	SystemEvent() :
		Event(system_event_type)
	{}

	int system_type() const
	{
		return Event::arg();
	}
};

}

#endif
