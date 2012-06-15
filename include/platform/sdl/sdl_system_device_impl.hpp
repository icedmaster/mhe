#ifndef __SDL_SYSTEM_DEVICE_IMPL_HPP__
#define __SDL_SYSTEM_DEVICE_IMPL_HPP__

#include "events/system_device_impl.hpp"
#include "mhe_sdl.hpp"

namespace mhe {
namespace sdl {

class SDLSystemDeviceImpl : public SystemDeviceImpl
{
public:
	std::vector< boost::shared_ptr<SystemEvent> > check();
private:
	boost::shared_ptr<SystemEvent> create_event(const SDL_Event& sdl_event) const;
};

}}

#endif
