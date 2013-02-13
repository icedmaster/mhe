#include "platform/sdl/sdl_system_device_impl.hpp"

#include "events/system_event.hpp"
#include "platform/sdl/sdl_event_helper.hpp"

namespace mhe {
namespace sdl {

void SDLSystemDeviceImpl::check(Device::events_vector& events, const WindowSystem&)
{
	SDL_events_vector sdl_events;
	int cnt = check_for_events(SDL_EVENTMASK(SDL_QUIT), sdl_events);
	if (cnt <= 0) return;
	events.resize(cnt);
	for (int i = 0; i < cnt; ++i)
		setup_event(static_cast<SystemEvent*>(events[i].get()), sdl_events[i]);
}

void SDLSystemDeviceImpl::setup_event(SystemEvent* event, const SDL_Event& sdl_event) const
{
	int arg = Event::any_event;
	if (sdl_event.type == SDL_QUIT)
		arg = SystemEvent::quit;
	return event->setup_event(arg);
}

}}
