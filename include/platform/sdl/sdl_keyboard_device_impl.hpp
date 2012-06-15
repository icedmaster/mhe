#ifndef __SDL_KEYBOARD_DEVICE_IMPL_HPP__
#define __SDL_KEYBOARD_DEVICE_IMPL_HPP__

#include "events/keyboard_device_impl.hpp"
#include "mhe_sdl.hpp"

namespace mhe {
namespace sdl {

class SDLKeyboardDeviceImpl : public KeyboardDeviceImpl
{
public:
	std::vector< boost::shared_ptr<KeyboardEvent> > check();
private:
	boost::shared_ptr<KeyboardEvent> create_event(const SDL_Event& sdl_event) const;
};

}}

#endif
