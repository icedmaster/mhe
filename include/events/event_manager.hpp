#ifndef __EVENT_MANAGER_HPP__
#define __EVENT_MANAGER_HPP__

#include <map>
#include "device.hpp"
#include "event_listener.hpp"

namespace mhe {

class EventManager
{
public:
	void add_device(Device* device);
	boost::shared_ptr<Device> get_device(const std::string& name) const;

	void add_listener(const boost::shared_ptr<EventListener>& listener);
	void add_listener(EventListener* listener);
	void check();
private:
	void process_event(const boost::shared_ptr<Event>& event);
	void process_event_with_id(int id, const boost::shared_ptr<Event>& event);

	typedef std::map< std::string, boost::shared_ptr<Device> > devices_map;
	typedef std::multimap< int, boost::shared_ptr<EventListener> > listeners_map;
	devices_map devices_;
	listeners_map listeners_;
};

}

#endif