#ifndef __ABSTRACT_WINDOW_SYSTEM_FACTORY_HPP__
#define __ABSTRACT_WINDOW_SYSTEM_FACTORY_HPP__

#include <string>
#include "core/ref_counter.hpp"

namespace mhe {

class WindowSystemImpl;
class KeyboardDeviceImpl;
class MouseDeviceImpl;
class SystemDeviceImpl;
class EventSystem;

class AbstractWindowSystemFactory : public ref_counter
{
public:
	virtual ~AbstractWindowSystemFactory() {}
	virtual std::string name() const = 0;
	virtual WindowSystemImpl* create_window_system() const = 0;
	virtual KeyboardDeviceImpl* create_keyboard_device_impl() const = 0;
	virtual MouseDeviceImpl* create_mouse_device_impl() const = 0;
	virtual SystemDeviceImpl* create_system_device_impl() const = 0;
	virtual EventSystem* create_event_system() const = 0;
};

}

#endif
