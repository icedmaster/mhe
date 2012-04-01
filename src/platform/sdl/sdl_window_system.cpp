#include "platform/sdl/sdl_window_system.hpp"

namespace mhe {
namespace sdl {

	bool SDLWindowSystem::init(const vector2<int>& r, int bpp, bool fullscreen)
	{
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

		int flags = SDL_OPENGL;
		if (fullscreen)
			flags |= SDL_FULLSCREEN;
        srf = SDL_SetVideoMode(r.x(), r.y(), bpp, flags);
		if (!srf) return false;

		int bpb = bpp >> 2;

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, bpb);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, bpb);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, bpb);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, bpb);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, bpp);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		return true;
	}

	void SDLWindowSystem::close()
	{
		SDL_Quit();
	}

	void SDLWindowSystem::set_caption(const std::string& caption)
	{
		SDL_WM_SetCaption(caption.c_str(), 0);
	}

	void SDLWindowSystem::swap_buffers()
	{
		SDL_GL_SwapBuffers();
	}

	void SDLWindowSystem::show_cursor(bool show)
	{
		int en = (show) ? SDL_ENABLE : SDL_DISABLE;
		SDL_ShowCursor(en);
	}
}}
