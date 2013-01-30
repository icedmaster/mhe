#ifndef __SDL_KEYBOARD_DEVICE_IMPL_HPP__
#define __SDL_KEYBOARD_DEVICE_IMPL_HPP__

#include "events/keyboard_device_impl.hpp"
#include "mhe_sdl.hpp"

namespace mhe {
namespace sdl {

class SDLKeyboardDeviceImpl : public KeyboardDeviceImpl
{
public:
	void check(Device::events_vector& events, const WindowSystem&);
private:
	void setup_event(KeyboardEvent* event, const SDL_Event& sdl_event) const;
};

}}

#endif
