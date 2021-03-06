#include "platform/qt/qt_mouse_device_impl.hpp"

#include "events/events_provider.hpp"
#include "platform/qt/qt_view.hpp"

namespace mhe {
namespace qt {

void QtMouseDeviceImpl::check(Device::events_vector& events, const WindowSystem& window_system)
{
	const QtView* view = static_cast<const QtView*>(window_system.view());
	view->events_provider()->remove_mouse_events(events);
	float height = window_system.height();
	for (size_t i = 0; i < events.size(); ++i)
	{
		MouseEvent* me = static_cast<MouseEvent*>(events[i].get());
		me->invert_y_position(height);
	}
}

}}
