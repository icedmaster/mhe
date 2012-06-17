#include "events/mouse_device.hpp"

#include "events/mouse_event.hpp"

namespace mhe {

MouseDevice::MouseDevice(const std::string& name) :
	Device(name)
{}

std::vector< boost::shared_ptr<Event> > MouseDevice::check_impl()
{
	const std::vector< boost::shared_ptr<MouseEvent> >& events = impl_->check();
	std::vector< boost::shared_ptr<Event> > out;
	out.reserve(events.size());
	for (size_t i = 0; i < events.size(); ++i)
	{
		out.push_back(events[i]);
	}
	return out;
}

}
