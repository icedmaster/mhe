#ifndef __EVENT_LISTENER_HPP__
#define __EVENT_LISTENER_HPP__

#include <boost/shared_ptr.hpp>
#include "delegate.hpp"
#include "event.hpp"

namespace mhe {

class EventListener
{
public:
	typedef Delegate1< bool, const boost::shared_ptr<Event>& > EventHandler;
	EventListener(EventType type, int arg, int optarg,
				  const EventHandler& handler);

	int id() const
	{
		return id_;
	}

	EventType type() const
	{
		return Event::get_event_type(id_);
	}

	int arg() const
	{
		return Event::get_event_arg(id_);
	}

	int optarg() const
	{
		return Event::get_event_optarg(id_);
	}

	bool handle(const boost::shared_ptr<Event>& event)
	{
		return handler_(event);
	}
private:
	int id_;
	EventHandler handler_;
};

}

#endif
