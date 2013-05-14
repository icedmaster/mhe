#include "events/event_manager.hpp"

#include "impl/system_factory.hpp"
#include "utils/global_log.hpp"

namespace mhe {

EventManager::EventManager() :
	backend_(SystemFactory::instance().create_event_system())
{
	backend_->init();
}

EventManager::~EventManager()
{
	backend_->deinit();
}

void EventManager::add_device(Device* device)
{
	INFO_LOG("EventManager::add_device:" << device->name());
	devices_[device->name()] = boost::shared_ptr<Device>(device);
}

boost::shared_ptr<Device> EventManager::get_device(const std::string& name) const
{
	devices_map::const_iterator it = devices_.find(name);
	if (it == devices_.end()) return boost::shared_ptr<Device>();
	return it->second;
}

void EventManager::add_listener(const boost::shared_ptr<EventListener>& listener)
{
	listeners_.insert(std::make_pair(listener->id(), listener));
}

void EventManager::add_listener(EventListener* listener)
{
	add_listener(boost::shared_ptr<EventListener>(listener));
}

void EventManager::check(const WindowSystem& ws)
{
	backend_->update_event_queue();
	for (devices_map::iterator it = devices_.begin(); it != devices_.end(); ++it)
	{
		const Device::events_vector& events = it->second->check(ws);
		for (size_t i = 0; i < events.size(); ++i)
		{
			process_event(events[i].get());
		}
	}
	backend_->clear_event_queue();
}
    
void EventManager::add_event(const Event *event)
{
    process_event(event);
}

void EventManager::process_event(const Event* event)
{
	// find global events
	int arg = event->arg(), optarg = event->optarg();
	EventType type = event->type();
	int id = Event::create_event_id(type, Event::any_event, Event::any_event);
	process_event_with_id(id, event);
	if (arg == Event::any_event) return;
	id = Event::create_event_id(type, arg, Event::any_event);
	process_event_with_id(id, event);
	if (optarg == Event::any_event) return;
	process_event_with_id(event->id(), event);
}

void EventManager::process_event_with_id(int id, const Event* event)
{
	typedef std::pair<listeners_map::iterator, listeners_map::iterator> find_result;
	find_result res = listeners_.equal_range(id);
	for (listeners_map::iterator it = res.first; it != res.second; ++it)
	{
		if (it->second->handle(event))
			return;
	}
}

}
