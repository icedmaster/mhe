#ifndef __KEYBOARD_EVENT_HPP__
#define __KEYBOARD_EVENT_HPP__

#include "event.hpp"

namespace mhe {

class KeyboardEvent : public Event
{
public:
enum
{
	key_down = 1,
	key_up
};
public:
	KeyboardEvent(int state, int sym) :
		Event(keyboard_event_type, state, sym)
	{}

	int state() const
	{
		return Event::arg();
	}

	int sym() const
	{
		return Event::optarg();
	}
};

}

#endif
