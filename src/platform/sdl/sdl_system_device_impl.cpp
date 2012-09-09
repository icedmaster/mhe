#include "platform/sdl/sdl_system_device_impl.hpp"

#include "events/system_event.hpp"
#include "platform/sdl/sdl_event_helper.hpp"

namespace mhe {
namespace sdl {

std::vector< boost::shared_ptr<Event> > SDLSystemDeviceImpl::check(const WindowSystem&)
{
	std::vector< boost::shared_ptr<Event> > events;
	std::vector<SDL_Event> sdl_events;
	int cnt = check_for_events(SDL_EVENTMASK(SDL_QUIT), sdl_events);
	if (cnt <= 0) return events;
	for (int i = 0; i < cnt; ++i)
		events.push_back(create_event(sdl_events[i]));
	return events;
}

boost::shared_ptr<SystemEvent> SDLSystemDeviceImpl::create_event(const SDL_Event& sdl_event) const
{
	int arg = Event::any_event;
	if (sdl_event.type == SDL_QUIT)
		arg = SystemEvent::quit;
	return boost::shared_ptr<SystemEvent>(new SystemEvent(arg));
}

}}
