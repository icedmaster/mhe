#include "platform/sdl/sdl_keyboard_device_impl.hpp"

#include "events/keyboard_event.hpp"
#include "platform/sdl/sdl_event_helper.hpp"

namespace mhe {
namespace sdl {

std::vector< boost::shared_ptr<KeyboardEvent> > SDLKeyboardDeviceImpl::check()
{
	std::vector< boost::shared_ptr<KeyboardEvent> > events;
	std::vector<SDL_Event> sdl_events;
	int cnt = check_for_events(SDL_KEYDOWN | SDL_KEYUP, sdl_events);
	if (cnt <= 0) return events;
	for (int i = 0; i < cnt; ++i)
		events.push_back(create_event(sdl_events[i]));
	return events;
}

boost::shared_ptr<KeyboardEvent> SDLKeyboardDeviceImpl::create_event(const SDL_Event& sdl_event) const
{
	int et = (sdl_event.type == SDL_KEYDOWN) ? KeyboardEvent::key_down : KeyboardEvent::key_up;
	return boost::shared_ptr<KeyboardEvent>(new KeyboardEvent(et, sdl_event.key.keysym.sym));
}

}}
