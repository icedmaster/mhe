#include "events/mouse_device.hpp"

#include "events/mouse_event.hpp"
#include "impl/system_factory.hpp"

namespace mhe {

MouseDevice::MouseDevice(const std::string& name) :
	Device(name),
	impl_(SystemFactory::instance().create_mouse_device_impl())
{}

std::vector< boost::shared_ptr<Event> > MouseDevice::check_impl()
{
	const std::vector< boost::shared_ptr<Event> >& events = impl_->check();
	return events;
}

}
