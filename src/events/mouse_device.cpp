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
	wheel_delta_.set(0);
	delta_.set(0);

	impl_->check(events, ws);
	if (!events.empty())
	{
		const MouseEvent* e = static_cast<const MouseEvent*>(events.back().get());
		vector2<int> prev_position = position_;
		position_ = e->pos();
		delta_ = position_ - prev_position;
		for (size_t i = 0; i < events.size(); ++i)
		{
			const MouseEvent* me = static_cast<const MouseEvent*>(events[i].get());
			if (me->event_type() == MouseEvent::button_pressed)
				buttons_[me->button()] = true;
			else if (me->event_type() == MouseEvent::button_released)
				buttons_[me->button()] = false;
			else if (me->event_type() == MouseEvent::wheel)
				wheel_delta_ = me->wheel_delta();
			if (!me->any_button_pressed())
				buttons_.fill(false);
		}
	}
}

}
