#ifndef __SDL_MOUSE_DEVICE_IMPL_HPP__
#define __SDL_MOUSE_DEVICE_IMPL_HPP__

#include "events/mouse_device_impl.hpp"
#include "mhe_sdl.hpp"

namespace mhe {
namespace sdl {

class SDLMouseDeviceImpl : public MouseDeviceImpl
{
public:
	void check(Device::events_vector& events, const WindowSystem&);
private:
	void setup_event(MouseEvent* event, 
					 const SDL_Event& sdl_event, const WindowSystem& ws) const;
};

}}

#endif
