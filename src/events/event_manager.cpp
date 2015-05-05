#include "events/event_manager.hpp"

#include "impl/system_factory.hpp"
#include "utils/global_log.hpp"

#include "events/keyboard_device.hpp"
#include "events/keyboard_event.hpp"
#include "events/mouse_device.hpp"
#include "events/mouse_event.hpp"

namespace mhe {

EventManager::EventManager() :
	backend_(SystemFactory::instance().create_event_system())
{
	backend_->init();
	init_default_bindings();
}

EventManager::~EventManager()
{
	backend_->deinit();
}

void EventManager::add_device(Device* device)
{
	INFO_LOG("EventManager::add_device:" << device->name());
	devices_[device->name()] = ref_ptr<Device>(device);
}

ref_ptr<Device> EventManager::get_device(const std::string& name) const
{
	devices_map::const_iterator it = devices_.find(name);
	if (it == devices_.end()) return ref_ptr<Device>();
	return it->second;
}

void EventManager::add_listener(const ref_ptr<EventListener>& listener)
{
	listeners_.insert(std::make_pair(listener->id(), listener));
}

void EventManager::add_listener(EventListener* listener)
{
	add_listener(ref_ptr<EventListener>(listener));
}

void EventManager::check(const WindowSystem& ws)
{
	clear_bindings();

	backend_->update_event_queue(ws);
	for (devices_map::iterator it = devices_.begin(); it != devices_.end(); ++it)
	{
		const Device::events_vector& events = it->second->check(ws);
		for (size_t i = 0; i < events.size(); ++i)
		{
			process_event(events[i].get());
		}
	}
	backend_->clear_event_queue(ws);
}
    
void EventManager::add_event(const Event *event)
{
    process_event(event);
}

void EventManager::add_keyboard(KeyboardDevice* device)
{
	keyboards_[hash(device->name())] = ref_ptr<KeyboardDevice>(device);
	add_device(device);
}

const KeyboardDevice* EventManager::keyboard() const
{
	if (keyboards_.empty()) return nullptr;
	return keyboards_.begin()->second.get();
}

void EventManager::add_mouse(MouseDevice* device)
{
	mice_[hash(device->name())] = ref_ptr<MouseDevice>(device);
	add_device(device);
}

const MouseDevice* EventManager::mouse() const
{
	if (mice_.empty()) return nullptr;
	return mice_.begin()->second.get();
}

void EventManager::add_bind(const char* name, EventType type, int arg, int optarg)
{
	EventBind bind;
	bind.id = Event::create_event_id(type, arg, optarg);
	bindings_[string(name)] = bind;
}

bool EventManager::check_bind(const char* name) const
{
	EventBindings::const_iterator it = bindings_.find(string(name));
	if (it == bindings_.end()) return false;
	return it->value.enabled;
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
			break;
	}

	for (EventBindings::iterator it = bindings_.begin(); it != bindings_.end(); ++it)
	{
		if (it->value.id == id)
			it->value.enabled = true;
	}
}

void EventManager::init_default_bindings()
{
	add_bind("left", keyboard_event_type, KeyboardEvent::key_down, KeyboardDevice::key_a);
	add_bind("right", keyboard_event_type, KeyboardEvent::key_down, KeyboardDevice::key_d);
	add_bind("up", keyboard_event_type, KeyboardEvent::key_down, KeyboardDevice::key_w);
	add_bind("down", keyboard_event_type, KeyboardEvent::key_down, KeyboardDevice::key_s);
}

void EventManager::clear_bindings()
{
	for (EventBindings::iterator it = bindings_.begin(); it != bindings_.end(); ++it)
		it->value.enabled = false;
}

}
