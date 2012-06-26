#ifndef __SDL_INPUT_FACTORY_HPP__
#define __SDL_INPUT_FACTORY_HPP__

#include "impl/input_factory.hpp"
#include "sdl_keyboard_device_impl.hpp"
#include "sdl_mouse_device_impl.hpp"
#include "sdl_system_device_impl.hpp"

namespace mhe {
namespace sdl {

class SDLInputFactory : public InputFactory
{
public:
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
