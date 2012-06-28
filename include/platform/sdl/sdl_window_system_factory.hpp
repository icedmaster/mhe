#ifndef __SDL_WINDOW_SYSTEM_FACTORY_HPP__
#define __SDL_WINDOW_SYSTEM_FACTORY_HPP__

#include "impl/abstract_window_system_factory.hpp"
#include "sdl_window_system.hpp"
#include "sdl_keyboard_device_impl.hpp"
#include "sdl_mouse_device_impl.hpp"
#include "sdl_system_device_impl.hpp"

namespace mhe {
namespace sdl {

class SDLWindowSystemFactory : public AbstractWindowSystemFactory
{
public:
	std::string name() const
	{
		return "sdl";
	}

	WindowSystemImpl* create_window_system() const
	{
		return new SDLWindowSystem;
	}

	KeyboardDeviceImpl* create_keyboard_device_impl() const
	{
		return new SDLKeyboardDeviceImpl();
	}

	MouseDeviceImpl* create_mouse_device_impl() const
	{
		return new SDLMouseDeviceImpl();
	}

	SystemDeviceImpl* create_system_device_impl() const
	{
		return new SDLSystemDeviceImpl();
	}
};

}}

#endif
