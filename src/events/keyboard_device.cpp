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
	impl_->check(events, ws);
	for (size_t i = 0; i < events.size(); ++i)
		// for KeyboardEvent optarg() == sym() and arg() == state()
		// see keyboard_event.hpp
		keys_[events[i]->optarg()] = (events[i]->arg() == KeyboardEvent::key_down) ? true : false;
}

bool KeyboardDevice::is_key_pressed(int sym) const
{
	return keys_[sym];
}


}
