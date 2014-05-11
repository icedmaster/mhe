#include "platform/qt/qt_keyboard_device_impl.hpp"

#include "events/events_provider.hpp"
#include "platform/qt/qt_view.hpp"

namespace mhe {
namespace qt {

void QtKeyboardDeviceImpl::check(Device::events_vector& events, const WindowSystem& window_system)
{
	const QtView* view = static_cast<const QtView*>(window_system.view());
	view->events_provider()->remove_keyboard_events(events);
}

}}
