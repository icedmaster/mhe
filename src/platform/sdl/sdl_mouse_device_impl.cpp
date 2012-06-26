#include "platform/sdl/sdl_mouse_device_impl.hpp"

#include "events/mouse_event.hpp"
#include "platform/sdl/sdl_event_helper.hpp"

namespace mhe {
namespace sdl {

std::vector< boost::shared_ptr<Event> > SDLMouseDeviceImpl::check()
{
	std::vector< boost::shared_ptr<Event> > events;
	std::vector<SDL_Event> sdl_events;
	int cnt = check_for_events(SDL_MOUSEMOTION | SDL_MOUSEBUTTONDOWN | SDL_MOUSEBUTTONUP, sdl_events);
	if (cnt <= 0) return events;
	for (int i = 0; i < cnt; ++i)
		events.push_back(create_event(sdl_events[i]));
	return events;
}

boost::shared_ptr<MouseEvent> SDLMouseDeviceImpl::create_event(const SDL_Event& event) const
{
	int arg = Event::any_event, optarg = Event::any_event;
	vector2<int> pos;
	if (event.type == SDL_MOUSEMOTION)
	{
		arg = MouseEvent::move;
		pos.set(event.motion.x, event.motion.y);
	}
	else
	{
		arg = (event.type == SDL_MOUSEBUTTONDOWN) ? MouseEvent::button_pressed : MouseEvent::button_released;
		pos.set(event.button.x, event.button.y);
		optarg = event.button.button;
	}
	return boost::shared_ptr<MouseEvent>(new MouseEvent(arg, optarg, pos));
}

}}
