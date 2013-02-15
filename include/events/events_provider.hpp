#ifndef __EVENTS_PROVIDER_HPP__
#define __EVENTS_PROVIDER_HPP__

#include "system_event.hpp"
#include "mouse_event.hpp"
#include "keyboard_event.hpp"
#include "device.hpp"
#include "fixed_size_vector.hpp"

namespace mhe {

class EventsProvider
{
	static const size_t events_number = 10;
public:
	void clear();

	void add_system_event(int event);
	void add_mouse_event(int button_state, int button, const vector2<float>& position);

	void remove_system_events(Device::events_vector& events);
	void remove_mouse_events(Device::events_vector& events);
private:
	fixed_size_vector<SystemEvent, events_number> system_events_;
	fixed_size_vector<MouseEvent, events_number> mouse_events_;
};

}

#endif
