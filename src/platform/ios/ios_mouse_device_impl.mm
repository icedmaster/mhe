#include "platform/ios/ios_mouse_device_impl.hpp"

#include "platform/ios/ios_view.hpp"

namespace mhe {
namespace ios {

void iOSMouseDeviceImpl::check(Device::events_vector& events, const WindowSystem& window_system)
{
	const iOSView* view = static_cast<const iOSView*>(window_system.view());
	view->events_provider()->remove_mouse_events(events);
	float height = window_system.height();
	for (size_t i = 0; i < events.size(); ++i)
	{
		MouseEvent* me = static_cast<MouseEvent*>(events[i].get());
		me->invert_y_position(height);
	}
}

}}
