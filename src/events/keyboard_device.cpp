#include "events/keyboard_device.hpp"

#include "events/keyboard_event.hpp"
#include "impl/system_factory.hpp"

namespace mhe {

KeyboardDevice::KeyboardDevice(const std::string& name) :
	Device(name),
	impl_(SystemFactory::instance().create_keyboard_device_impl())
{
	::memset(keys_, 0, sizeof(keys_));
	init_events_with_type<KeyboardEvent>();
}

void KeyboardDevice::check_impl(Device::events_vector& events, const WindowSystem& ws)
{
	::memset(was_pressed_, 0, sizeof(was_pressed_));
	::memset(was_released_, 0, sizeof(was_released_));
	impl_->check(events, ws);
	for (size_t i = 0; i < events.size(); ++i)
	{
		// for KeyboardEvent optarg() == sym() and arg() == state()
		// see keyboard_event.hpp
		bool pressed = (events[i]->arg() == KeyboardEvent::key_down) ? true : false;
		int sym = events[i]->optarg();
		if (pressed != keys_[sym])
		{
			if (pressed) was_pressed_[sym] = true;
			else was_released_[sym] = true;
		}
		keys_[sym] = pressed;
	}
}

bool KeyboardDevice::is_key_down(int sym) const
{
	return keys_[sym];
}

bool KeyboardDevice::is_key_pressed(int sym) const
{
	return was_pressed_[sym];
}

bool KeyboardDevice::is_key_released(int sym) const
{
	return was_released_[sym];
}


}
