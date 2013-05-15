#include "platform/ios/ios_event_system.hpp"

#include "platform/ios/ios_view.hpp"

namespace mhe {
namespace ios {

void iOSEventSystem::clear_event_queue(const WindowSystem& window_system)
{
	const iOSView* view = static_cast<const iOSView*>(window_system.view());
	view->events_provider()->clear();
}

}}
