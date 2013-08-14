#ifndef __SDL_EVENT_HELPER_HPP__
#define __SDL_EVENT_HELPER_HPP__

#include "mhe_sdl.hpp"
#include "fixed_size_vector.hpp"

#ifdef MHE_SDL2
#define SDL_EVENTMASK(event)	(event)
#endif

namespace mhe {
namespace sdl {

const int max_number_of_events = 4;
typedef fixed_size_vector<SDL_Event, max_number_of_events> SDL_events_vector;

// events must be empty before calling this function
inline int check_for_events(int mask, SDL_events_vector& events)
{
	SDL_PumpEvents();
	return SDL_PeepEvents(&events[0], max_number_of_events, SDL_GETEVENT, mask
#ifdef MHE_HAS_SDL2
		,mask
#endif
		);
}

#ifdef MHE_SDL2
inline int check_for_events(int* masks, int masks_size, SDL_events_vector& events)
{
	SDL_PumpEvents();
	int peeped = 0;
	for (int i = 0; i < masks_size; ++i)
	{		
		peeped += SDL_PeepEvents(&events[peeped], max_number_of_events - peeped, SDL_GETEVENT,
								 masks[i], masks[i]);
	}
	return peeped;
}
#endif

}}

#endif
