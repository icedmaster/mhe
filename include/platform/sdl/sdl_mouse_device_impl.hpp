#ifndef __SDL_MOUSE_DEVICE_IMPL_HPP__
#define __SDL_MOUSE_DEVICE_IMPL_HPP__

#include "events/mouse_device_impl.hpp"
#include "mhe_sdl.hpp"

namespace mhe {
namespace sdl {

class SDLMouseDeviceImpl : public MouseDeviceImpl
{
public:
	std::vector< boost::shared_ptr<Event> > check();
private:
	boost::shared_ptr<MouseEvent> create_event(const SDL_Event& event) const;
};

}}

#endif
