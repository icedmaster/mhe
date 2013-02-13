#include "events/mouse_device.hpp"

#include "events/mouse_event.hpp"
#include "impl/system_factory.hpp"

namespace mhe {

MouseDevice::MouseDevice(const std::string& name) :
	Device(name),
	impl_(SystemFactory::instance().create_mouse_device_impl())
{
	init_events_with_type<MouseEvent>();
}

void MouseDevice::check_impl(events_vector& events, const WindowSystem& ws)
{
	impl_->check(events, ws);
}

}
