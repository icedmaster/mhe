#ifndef _SDL2_WINDOW_SYSTEM_HPP_
#define _SDL2_WINDOW_SYSTEM_HPP_

#include "render/window_system.hpp"
#include "mhe_sdl.hpp"

namespace mhe {
namespace sdl {

class SDL2WindowSystem;

class SDL2View : public View
{
	friend class SDL2WindowSystem;
public:
	SDL2View() :
		surface_(nullptr)
	{}

	void set(struct SDL_Window* surface)
	{
		surface_ = surface;
	}

	const SDL_Window* get() const
	{
		return surface_;
	}
private:
	SDL_Window* surface_;
};

class SDL2WindowSystem : public WindowSystemImpl
{
public:
	bool init(const vector2<int>& r, int bpp, bool fullscreen);
	bool set_format(const WindowContextFormat& format);
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
	SDL2View view_;
	vector2<int> size_;
	int bpp_;
	bool fullscreen_;
};

}}

#endif
