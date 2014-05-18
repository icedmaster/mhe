#ifndef __EVENT_MANAGER_HPP__
#define __EVENT_MANAGER_HPP__

#include <map>
#include "render/window_system.hpp"
#include "device.hpp"
#include "event_listener.hpp"
#include "event_system.hpp"

namespace mhe {

class EventManager
{
public:
	EventManager();
	~EventManager();

	void add_device(Device* device);
	ref_ptr<Device> get_device(const std::string& name) const;

	void add_listener(const ref_ptr<EventListener>& listener);
	void add_listener(EventListener* listener);
	void check(const WindowSystem& ws);
    
    void add_event(const Event* event);
private:
  	void process_event(const Event* event);
	void process_event_with_id(int id, const Event* event);

	typedef std::map< std::string, ref_ptr<Device> > devices_map;
	typedef std::multimap< int, ref_ptr<EventListener> > listeners_map;
	devices_map devices_;
	listeners_map listeners_;
    unique_ptr<EventSystem> backend_;
};

}

#endif
