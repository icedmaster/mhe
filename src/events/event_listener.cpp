#include "events/event_listener.hpp"

namespace mhe {

EventListener::EventListener(EventType type, int arg, int optarg,
							 const EventHandler& handler) :
	handler_(handler)
{
	id_ = Event::create_event_id(type, arg, optarg);
}

}
