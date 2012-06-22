#include "events/keyboard_device.hpp"

#include "events/keyboard_event.hpp"
#include "impl/system_factory.hpp"

namespace mhe {

KeyboardDevice::KeyboardDevice(const std::string& name) :
	Device(name),
	impl_(SystemFactory::instance().create_keyboard_device_impl())
{}

std::vector< boost::shared_ptr<Event> > KeyboardDevice::check_impl()
{
	const std::vector< boost::shared_ptr<KeyboardEvent> >& events = impl_->check();
	std::vector< boost::shared_ptr<Event> > out;
	out.reserve(events.size());
	for (size_t i = 0; i < events.size(); ++i)
	{
		keys_[events[i]->sym()] = (events[i]->state() == KeyboardEvent::key_down) ? true : false;
		out.push_back(events[i]);
	}
	return out;
}

bool KeyboardDevice::is_key_pressed(int sym) const
{
	return keys_[sym];
}


}
