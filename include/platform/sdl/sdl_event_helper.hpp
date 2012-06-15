#ifndef __SDL_EVENT_HELPER_HPP__
#define __SDL_EVENT_HELPER_HPP__

#include "mhe_sdl.hpp"

namespace mhe {
namespace sdl {

// events must be empty before calling this function
inline int check_for_events(int mask, std::vector<SDL_Event>& events)
{
	const int max_number_of_events = 4;
	events.resize(max_number_of_events);
	return SDL_PeepEvents(&events[0], max_number_of_events, SDL_GETEVENT, mask);
}

}}

#endif
