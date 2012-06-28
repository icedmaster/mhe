#include "events/system_device.hpp"

#include "events/system_event.hpp"
#include "impl/system_factory.hpp"

namespace mhe {

SystemDevice::SystemDevice(const std::string& name) :
	Device(name),
	impl_(SystemFactory::instance().create_system_device_impl())
{
}

std::vector< boost::shared_ptr<Event> > SystemDevice::check_impl(const WindowSystem& ws)
{
	const std::vector< boost::shared_ptr<Event> >& events = impl_->check(ws);
	return events;
}

}
