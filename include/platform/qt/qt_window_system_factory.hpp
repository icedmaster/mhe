#ifndef __QT_WINDOW_SYSTEM_FACTORY_HPP__
#define __QT_WINDOW_SYSTEM_FACTORY_HPP__

#include "impl/abstract_window_system_factory.hpp"
#include "qt_window_system.hpp"

namespace mhe {
namespace qt {

class QtWindowSystemFactory : public AbstractWindowSystemFactory
{
public:
	std::string name() const
	{
		return "qt";
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
		return nullptr;
	}

	EventSystem* create_event_system() const
	{
		return nullptr;
	}
};

}}

#endif
