#ifndef __DELEGATE_EVENT_LISTENER_HPP__
#define __DELEGATE_EVENT_LISTENER_HPP__

#include "core/delegate.hpp"
#include "event_listener.hpp"

namespace mhe {

class MHE_EXPORT DelegateEventListener : public EventListener
{
public:
	typedef Delegate1< bool, const Event* > EventHandler;
	DelegateEventListener(EventType type, int arg, int optarg,
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

	bool handle(const Event* event)
	{
		return handler_(event);
	}
private:
	int id_;
	EventHandler handler_;
};

}

#endif
