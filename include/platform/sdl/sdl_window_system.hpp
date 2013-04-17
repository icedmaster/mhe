#ifndef _SDL_WINDOW_SYSTEM_HPP_
#define _SDL_WINDOW_SYSTEM_HPP_

#include "window_system.hpp"
#include <SDL/SDL.h>

namespace mhe {
namespace sdl {

class SDLView : public View
{
public:
	SDLView() :
		surface_(nullptr)
	{}

	void set(SDL_Surface* surface)
	{
		surface_ = surface;
	}

	const SDL_Surface* get() const
	{
		return surface_;
	}
private:
	SDL_Surface* surface_;
};

class SDLWindowSystem : public WindowSystemImpl
{
public:
	bool init(const vector2<int>& r, int bpp, bool fullscreen);
	void close();
	void set_caption(const std::string& caption);
	void swap_buffers();
	void show_cursor(bool show);
	// TODO:
	void resize(const vector2<int>&) {}

	View* view()
	{
		return &view_;
	}
private:
	SDLView view_;
};

}}

#endif
