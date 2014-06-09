#ifndef __SDL2_EVENT_SYSTEM_HPP__
#define __SDL2_EVENT_SYSTEM_HPP__

#include "events/event_system.hpp"
#include "mhe_sdl.hpp"

namespace mhe {
namespace sdl {

class SDLEventSystem : public EventSystem
{
public:
	bool init()
	{
		// really initialized with SDL_INIT_VIDEO
		return true;
	}

	void deinit() {}	// do nothing

	void clear_event_queue(const WindowSystem& /*window_system*/)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event)) ;
	}
};

}}

#endif
