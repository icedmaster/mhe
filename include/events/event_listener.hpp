#ifndef __EVENT_LISTENER_HPP__
#define __EVENT_LISTENER_HPP__

#include "core/ref_counter.hpp"
#include "event.hpp"

namespace mhe {

class EventListener : public ref_counter
{
public:
	virtual ~EventListener() {}

	virtual int id() const
	{
		return Event::create_event_id(type(), arg(), optarg());
	}

	virtual EventType type() const = 0;
	virtual int arg() const = 0;
	virtual int optarg() const { return Event::any_event; }
	virtual bool handle(const Event* event) = 0;
};

}

#endif
