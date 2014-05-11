#include "events/system_device.hpp"

#include "events/system_event.hpp"
#include "impl/system_factory.hpp"

namespace mhe {

SystemDevice::SystemDevice(const std::string& name) :
	Device(name),
	impl_(SystemFactory::instance().create_system_device_impl())
{
	init_events_with_type<SystemEvent>();
}

void SystemDevice::check_impl(events_vector& events, const WindowSystem& ws)
{
	impl_->check(events, ws);
}

}
