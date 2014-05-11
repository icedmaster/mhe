#ifndef __QT_EVENT_SYSTEM_HPP__
#define __QT_EVENT_SYSTEM_HPP__

#include "events/event_system.hpp"
#include "qt_view.hpp"

namespace mhe {
namespace qt {

class QtEventSystem : public EventSystem
{
public:
	bool init()
	{
		return true;
	}

	void deinit() {}	// do nothing

	void clear_event_queue(const WindowSystem& window_system)
	{
		const QtView* view = static_cast<const QtView*>(window_system.view());
		view->events_provider()->clear();
	}
};

}}

#endif
