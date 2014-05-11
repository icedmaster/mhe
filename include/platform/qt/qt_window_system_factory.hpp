#ifndef __QT_WINDOW_SYSTEM_FACTORY_HPP__
#define __QT_WINDOW_SYSTEM_FACTORY_HPP__

#include "impl/abstract_window_system_factory.hpp"
#include "qt_window_system.hpp"
#include "qt_event_system.hpp"
#include "qt_system_device_impl.hpp"
#include "qt_mouse_device_impl.hpp"
#include "qt_keyboard_device_impl.hpp"

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
		return new QtWindowSystem;
	}

	KeyboardDeviceImpl* create_keyboard_device_impl() const
	{
		return new QtKeyboardDeviceImpl;
	}

	MouseDeviceImpl* create_mouse_device_impl() const
	{
		return new QtMouseDeviceImpl;
	}

	SystemDeviceImpl* create_system_device_impl() const
	{
		return new QtSystemDeviceImpl;
	}

	EventSystem* create_event_system() const
	{
		return new QtEventSystem;
	}
};

}}

#endif
