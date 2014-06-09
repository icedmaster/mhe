#include "platform/sdl2/sdl2_mouse_device_impl.hpp"

#include "events/mouse_event.hpp"
#include "platform/sdl2/sdl2_event_helper.hpp"
#include "platform/sdl2/sdl2_window_system.hpp"

namespace mhe {
namespace sdl {

void SDL2MouseDeviceImpl::check(Device::events_vector& events, const WindowSystem& ws)
{
	SDL_events_vector sdl_events;
	int masks[3] = {SDL_MOUSEMOTION, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP}; 
	int cnt = check_for_events(masks, 3, sdl_events);
	if (cnt <= 0) return;
	events.resize(cnt);
	for (int i = 0; i < cnt; ++i)
		setup_event(static_cast<MouseEvent*>(events[i].get()), sdl_events[i], ws);
}

void SDL2MouseDeviceImpl::setup_event(MouseEvent* event, 
	  			 const SDL_Event& sdl_event, const WindowSystem& ws) const
{
	int arg = Event::any_event, optarg = Event::any_event;
	vector2<int> pos;
	if (sdl_event.type == SDL_MOUSEMOTION)
	{
		arg = MouseEvent::move;
		pos.set(sdl_event.motion.x, ws.height() - sdl_event.motion.y);
	}
	else
	{
		arg = (sdl_event.type == SDL_MOUSEBUTTONDOWN) ? MouseEvent::button_pressed : MouseEvent::button_released;
		pos.set(sdl_event.button.x, ws.height() - sdl_event.button.y);
		optarg = sdl_event.button.button;
	}
	return event->setup_event(arg, optarg, pos);
}

}}
