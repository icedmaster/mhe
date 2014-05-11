#include "events/mouse_device.hpp"

#include "events/mouse_event.hpp"
#include "impl/system_factory.hpp"

namespace mhe {

MouseDevice::MouseDevice(const std::string& name) :
	Device(name),
	impl_(SystemFactory::instance().create_mouse_device_impl())
{
	init_events_with_type<MouseEvent>();
	buttons_.fill(false);
}

void MouseDevice::check_impl(events_vector& events, const WindowSystem& ws)
{
	impl_->check(events, ws);
	if (!events.empty())
	{
		const MouseEvent* e = static_cast<const MouseEvent*>(events.back().get());
		position_ = e->pos();
		for (size_t i = 0; i < events.size(); ++i)
		{
			const MouseEvent* me = static_cast<const MouseEvent*>(events[i].get());			
			if (me->event_type() == MouseEvent::button_pressed)
				buttons_[me->button()] = true;
			else if (me->event_type() == MouseEvent::button_released)
				buttons_[me->button()] = false;
		}
	}
}

}
