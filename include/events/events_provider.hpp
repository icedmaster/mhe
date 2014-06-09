#ifndef __EVENTS_PROVIDER_HPP__
#define __EVENTS_PROVIDER_HPP__

#include "system_event.hpp"
#include "mouse_event.hpp"
#include "keyboard_event.hpp"
#include "device.hpp"
#include "core/fixed_size_vector.hpp"

namespace mhe {

class MHE_EXPORT EventsProvider
{
	static const size_t events_number = 10;
public:
	void clear();

	void add_system_event(int event);
	void add_mouse_event(int button_state, int button, const vector2<float>& position);
	void add_keyboard_event(int type, int button);

	void remove_system_events(Device::events_vector& events);
	void remove_mouse_events(Device::events_vector& events);
	void remove_keyboard_events(Device::events_vector& events);
private:
	template <class EventsVector>
	void remove_events(Device::events_vector& events, EventsVector& events_vector)
	{
		typedef typename EventsVector::value_type Type;
		events.resize(events_vector.size());
		for (size_t i = 0 ; i < events_vector.size(); ++i)
		{
			Type* event = static_cast<Type*>(events[i].get());
			*event = events_vector[i];
		}
		events_vector.clear();
	}

	fixed_size_vector<SystemEvent, events_number> system_events_;
	fixed_size_vector<MouseEvent, events_number> mouse_events_;
	fixed_size_vector<KeyboardEvent, events_number> keyboard_events_;
};

}

#endif
