#include "events/events_provider.hpp"

namespace mhe {

void EventsProvider::clear()
{
	system_events_.clear();
	mouse_events_.clear();
}

void EventsProvider::add_system_event(int event)
{
	system_events_.push_back(SystemEvent(event));
}

void EventsProvider::add_mouse_event(int button_state, int button, const vector2<float>& position)
{
	mouse_events_.push_back(MouseEvent(button_state, button, position));
}

void EventsProvider::remove_system_events(Device::events_vector& events)
{
	events.resize(system_events_.size());
	for (size_t i = 0 ; i < system_events_.size(); ++i)
	{
		SystemEvent* event = static_cast<SystemEvent*>(events[i].get());
		*event = system_events_[i];
	}
	system_events_.clear();
}

void EventsProvider::remove_mouse_events(Device::events_vector& events)
{
	events.resize(mouse_events_.size());
	for (size_t i = 0 ; i < mouse_events_.size(); ++i)
	{
		MouseEvent* event = static_cast<MouseEvent*>(events[i].get());
		*event = mouse_events_[i];
	}
	mouse_events_.clear();
}

}
