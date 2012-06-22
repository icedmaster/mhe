#include "events/system_device.hpp"

#include "events/system_event.hpp"
#include "impl/system_factory.hpp"

namespace mhe {

SystemDevice::SystemDevice(const std::string& name) :
	Device(name),
	impl_(SystemFactory::instance().create_system_device_impl())
{
}

std::vector< boost::shared_ptr<Event> > SystemDevice::check_impl()
{
	const std::vector< boost::shared_ptr<SystemEvent> >& events = impl_->check();
	std::vector< boost::shared_ptr<Event> > out;
	out.reserve(events.size());
	for (size_t i = 0; i < events.size(); ++i)
		out.push_back(events[i]);
	return out;
}

}
