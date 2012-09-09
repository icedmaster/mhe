#ifndef __EVENT_SYSTEM_HPP__
#define __EVENT_SYSTEM_HPP__

namespace mhe {

class EventSystem
{
public:
	virtual ~EventSystem() {}
	virtual bool init() = 0;
	virtual void deinit() = 0;
	virtual void clear_event_queue() {}
	virtual void update_event_queue() {}
};

}

#endif
