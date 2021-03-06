#ifndef __SDL2_WINDOW_SYSTEM_FACTORY_HPP__
#define __SDL2_WINDOW_SYSTEM_FACTORY_HPP__

#include "impl/abstract_window_system_factory.hpp"
#include "sdl2_window_system.hpp"
#include "sdl2_keyboard_device_impl.hpp"
#include "sdl2_mouse_device_impl.hpp"
#include "sdl2_system_device_impl.hpp"
#include "sdl2_event_system.hpp"

namespace mhe {
namespace sdl {

class SDL2WindowSystemFactory : public AbstractWindowSystemFactory
{
public:
	std::string name() const
	{
		return "sdl2";
	}

	WindowSystemImpl* create_window_system() const
	{
		return new SDL2WindowSystem;
	}

	KeyboardDeviceImpl* create_keyboard_device_impl() const
	{
		return new SDL2KeyboardDeviceImpl;
	}

	MouseDeviceImpl* create_mouse_device_impl() const
	{
		return new SDL2MouseDeviceImpl;
	}

	SystemDeviceImpl* create_system_device_impl() const
	{
		return new SDLSystemDeviceImpl();
	}

	EventSystem* create_event_system() const
	{
		return new SDLEventSystem();
	}
};

}}

#endif
