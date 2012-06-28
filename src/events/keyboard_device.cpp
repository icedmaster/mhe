#include "events/keyboard_device.hpp"

#include "events/keyboard_event.hpp"
#include "impl/system_factory.hpp"

namespace mhe {

KeyboardDevice::KeyboardDevice(const std::string& name) :
	Device(name),
	impl_(SystemFactory::instance().create_keyboard_device_impl())
{}

std::vector< boost::shared_ptr<Event> > KeyboardDevice::check_impl(const WindowSystem& ws)
{
	const std::vector< boost::shared_ptr<Event> >& events = impl_->check(ws);
	for (size_t i = 0; i < events.size(); ++i)
		// for KeyboardEvent optarg() == sym() and arg() == state()
		// see keyboard_event.hpp
		keys_[events[i]->optarg()] = (events[i]->arg() == KeyboardEvent::key_down) ? true : false;
	return events;
}

bool KeyboardDevice::is_key_pressed(int sym) const
{
	return keys_[sym];
}


}
