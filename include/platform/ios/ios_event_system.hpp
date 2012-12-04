#ifndef __IOS_EVENT_SYSTEM_HPP__
#define __IOS_EVENT_SYSTEM_HPP__

#include "events/event_system.hpp"

namespace mhe {
namespace ios {

class iOSEventSystem : public EventSystem
{
public:
	bool init()
	{
		// really initialized with SDL_INIT_VIDEO
		return true;
	}

	void deinit() {}	// do nothing

	void clear_event_queue()
	{
	}
};

}}

#endif
