#ifndef __IOS_WINDOW_SYSTEM_FACTORY_HPP__
#define __IOS_WINDOW_SYSTEM_FACTORY_HPP__

#include "impl/abstract_window_system_factory.hpp"
#include "ios_window_system.hpp"
#include "ios_event_system.hpp"
#include "ios_system_device_impl.hpp"

namespace mhe {
namespace ios {

class iOSWindowSystemFactory : public AbstractWindowSystemFactory
{
public:
	std::string name() const
	{
		return "ios";
	}

	WindowSystemImpl* create_window_system() const
	{
		return new iOSWindowSystem;
	}

	KeyboardDeviceImpl* create_keyboard_device_impl() const
	{
		return nullptr;
	}

	MouseDeviceImpl* create_mouse_device_impl() const
	{
		return nullptr;
	}

	SystemDeviceImpl* create_system_device_impl() const
	{
		return new iOSSystemDeviceImpl;
	}

	EventSystem* create_event_system() const
	{
		return new iOSEventSystem;
	}
};

}}

#endif
