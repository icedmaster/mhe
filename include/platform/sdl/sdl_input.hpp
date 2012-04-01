#ifndef _SDL_INPUT_HPP_
#define _SDL_INPUT_HPP_

#include "event.hpp"
#include <map>
#include <vector>
#include <boost/weak_ptr.hpp>
#include <SDL/SDL.h>

namespace mhe {
namespace sdl {

	class SDLInputSystem : public InputSystem
	{
	public:
		SDLInputSystem();
    private:
		WindowSystem* ws_;

		void add_keydown_event(const SDL_keysym&);
		void add_keyup_event(const SDL_keysym&);
		void add_mouse_move_event(const SDL_MouseMotionEvent&);
		void add_mouse_button_event(const SDL_MouseButtonEvent&);
		void add_mouse_event(int type, int x, int y, int button);
		void add_quit_event();
		void add_tick_event();

		void check_impl();
		void handle_impl();
		void set_window_system_impl(WindowSystem* ws)
		{
			ws_ = ws;
		}			
	};
}}

#endif
