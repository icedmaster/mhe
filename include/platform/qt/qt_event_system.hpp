#ifndef __QT_EVENT_SYSTEM_HPP__
#define __QT_EVENT_SYSTEM_HPP__

#include "events/event_system.hpp"

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

	void clear_event_queue()
	{}
};

}}

#endif
