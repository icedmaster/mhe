#ifndef __EVENT_MANAGER_HPP__
#define __EVENT_MANAGER_HPP__

#include <map>
#include "render/window_system.hpp"
#include "core/hash.hpp"
#include "device.hpp"
#include "event_listener.hpp"
#include "event_system.hpp"

namespace mhe {

class KeyboardDevice;
class MouseDevice;

class MHE_EXPORT EventManager
{
private:
	struct EventBind
	{
		int id;
		bool enabled;
	};
public:
	EventManager();
	~EventManager();

	void add_device(Device* device);
	ref_ptr<Device> get_device(const std::string& name) const;

	void add_listener(const ref_ptr<EventListener>& listener);
	void add_listener(EventListener* listener);
	void check(const WindowSystem& ws);
    
    void add_event(const Event* event);

	void add_keyboard(KeyboardDevice* device);
	const KeyboardDevice* keyboard() const;

	void add_mouse(MouseDevice* device);
	const MouseDevice* mouse() const;

	void add_bind(const char* name, EventType type, int arg, int optarg = 0);
	bool check_bind(const char* name) const;
private:
	void init_default_bindings();
  	void process_event(const Event* event);
	void process_event_with_id(int id, const Event* event);
	void clear_bindings();

	// generic interface
	typedef std::map< std::string, ref_ptr<Device> > devices_map;
	typedef std::multimap< int, ref_ptr<EventListener> > listeners_map;
	devices_map devices_;
	listeners_map listeners_;

	typedef std::map< hash_type, ref_ptr<KeyboardDevice> > keyboards_map;
	keyboards_map keyboards_;

	typedef std::map< hash_type, ref_ptr<MouseDevice> > mice_map;
	mice_map mice_;

	typedef std::map<hash_type, EventBind> EventBindings;
	EventBindings bindings_;

    unique_ptr<EventSystem> backend_;
};

}

#endif
