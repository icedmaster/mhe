#include "platform/sdl2/sdl2_window_system.hpp"

#include "utils/global_log.hpp"

namespace mhe {
namespace sdl {

	bool SDL2WindowSystem::init(const vector2<int>& r, int bpp, bool fullscreen)
	{
		size_ = r;
		bpp_ = bpp;
		fullscreen_ = fullscreen;
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
		return true;
	}

	void SDL2WindowSystem::close()
	{
		if (view_.surface_ != nullptr)
			SDL_DestroyWindow(view_.surface_);
		SDL_Quit();
	}

	bool SDL2WindowSystem::set_format(const WindowContextFormat& format)
	{
		SDL_Window* window = view_.surface_;
		if (window != nullptr)
			SDL_DestroyWindow(window);
		int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
		if (fullscreen_)
			flags |= SDL_WINDOW_FULLSCREEN;

        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, (bpp_ != 32) ? bpp_ : 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, format.major_version);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, format.minor_version);

		window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size_.x(), size_.y(), flags);
		if (!window) return false;

		SDL_GLContext context = SDL_GL_CreateContext(window);
		if (context == nullptr)
		{
			ERROR_LOG("SDL2WindowSystem::Failed to create OpenGL context:" << format.major_version << " " << format.minor_version);
			SDL_DestroyWindow(window);
			return false;
		}
		if (SDL_GL_MakeCurrent(window, context) != 0)
		{
			ERROR_LOG("SDL2WindowSystem::failed to setup current context");
			SDL_DestroyWindow(window);
			return false;
		}

		view_.set(window);
		return true;
	}

	void SDL2WindowSystem::set_caption(const std::string& caption)
	{
		SDL_SetWindowTitle(view_.surface_, caption.c_str());
	}

	void SDL2WindowSystem::swap_buffers()
	{
		SDL_GL_SwapWindow(view_.surface_);
	}

	void SDL2WindowSystem::show_cursor(bool show)
	{
		int en = (show) ? SDL_ENABLE : SDL_DISABLE;
		SDL_ShowCursor(en);
	}

	void SDL2WindowSystem::set_vsync_enabled(bool enabled)
	{
		if (SDL_GL_SetSwapInterval(enabled ? 1 : 0) != 0)
		{
			ERROR_LOG("Can't set vsync settings:" << SDL_GetError());
		}
	}
}}
