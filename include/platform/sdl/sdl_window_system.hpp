#ifndef _SDL_WINDOW_SYSTEM_HPP_
#define _SDL_WINDOW_SYSTEM_HPP_

#include "window_system.hpp"
#include <SDL/SDL.h>

namespace mhe {
namespace sdl {
	class SDLWindowSystem : public iWindowSystem
	{
		private:
			SDL_Surface *srf;
		public:
			bool init(const vector2<int>& r, int bpp, bool fullscreen);
			void close();
			void set_caption(const std::string& caption);
			void swap_buffers();
			void show_cursor(bool show);
	};
}}

#endif