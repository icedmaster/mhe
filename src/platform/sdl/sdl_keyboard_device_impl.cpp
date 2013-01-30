#include "platform/sdl/sdl_keyboard_device_impl.hpp"

#include "events/keyboard_event.hpp"
#include "platform/sdl/sdl_event_helper.hpp"

namespace mhe {
namespace sdl {

void SDLKeyboardDeviceImpl::check(Device::events_vector& events, const WindowSystem& /*ws*/)
{
	SDL_events_vector sdl_events;
	int cnt = check_for_events(SDL_EVENTMASK(SDL_KEYDOWN) | SDL_EVENTMASK(SDL_KEYUP), sdl_events);
	if (cnt <= 0) return;
	events.resize(cnt);
	for (int i = 0; i < cnt; ++i)
		setup_event(static_cast<KeyboardEvent*>(events[i].get()), sdl_events[i]);
}

void SDLKeyboardDeviceImpl::setup_event(KeyboardEvent* event, const SDL_Event& sdl_event) const
{
	int et = (sdl_event.type == SDL_KEYDOWN) ? KeyboardEvent::key_down : KeyboardEvent::key_up;
	return event->setup_event(et, sdl_event.key.keysym.sym);
}

}}
