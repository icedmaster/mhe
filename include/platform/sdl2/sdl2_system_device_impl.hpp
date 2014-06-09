#ifndef __SDL2_SYSTEM_DEVICE_IMPL_HPP__
#define __SDL2_SYSTEM_DEVICE_IMPL_HPP__

#include "events/system_device_impl.hpp"
#include "mhe_sdl.hpp"

namespace mhe {
namespace sdl {

class SDLSystemDeviceImpl : public SystemDeviceImpl
{
public:
	void check(Device::events_vector& events, const WindowSystem&);
private:
	void setup_event(SystemEvent* event, const SDL_Event& sdl_event) const;
};

}}

#endif
