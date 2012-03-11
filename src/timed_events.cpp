#include "timed_events.hpp"

namespace mhe {

void TimedEventsManager::update(cmn::uint tick)
{
	for (std::list<timed_event_ptr>::iterator it = events_.begin(); it != events_.end();)
	{
		timed_event_ptr event = *it;
		if (event->try_execute(tick) && !event->is_running())
		{
			// remove event
			it = events_.erase(it);
		}
		else ++it;
	}
}

}
