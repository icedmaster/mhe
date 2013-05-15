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

void EventsProvider::add_keyboard_event(int type, int button)
{
	keyboard_events_.push_back(KeyboardEvent(type, button));
}

void EventsProvider::remove_system_events(Device::events_vector& events)
{
	remove_events(events, system_events_);
}

void EventsProvider::remove_mouse_events(Device::events_vector& events)
{
	remove_events(events, mouse_events_);
}

void EventsProvider::remove_keyboard_events(Device::events_vector& events)
{
	remove_events(events, keyboard_events_);
}

}
