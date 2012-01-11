#ifndef _SDL_WINDOW_SYSTEM_HPP_
#define _SDL_WINDOW_SYSTEM_HPP_

#include "window_system.hpp"
#include <SDL/SDL.h>

namespace mhe
{
	class SDLWindowSystem : public iWindowSystem
	{
		private:
			SDL_Surface *srf;
		public:
			bool init(const vector2<int>& r, int bpp, bool fullscreen);
			void close();
			void setCaption(const std::string& caption);
			void swapBuffers();
			void showCursor(bool show);
	};
}

#endif
