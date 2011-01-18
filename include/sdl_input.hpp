#ifndef _SDL_INPUT_HPP_
#define _SDL_INPUT_HPP_

#include "input.hpp"
#include <SDL/SDL.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>

namespace mhe
{
	class SDLInputSystem : public iInputSystem
	{
		private:
			void check_impl();
			void handle_impl();
			void add_listener(EventListener* el);
			void set_keyboard_event_handler(KeyboardEventHandler *keh);
			void set_mouse_event_handler(MouseEventHandler* meh);
		private:
			boost::shared_ptr<KeyboardEventHandler> keyboard_handler;
			boost::shared_ptr<MouseEventHandler> mouse_handler;
			std::vector<KeyboardEvent> keyboard_events;
			std::vector<MouseEvent> mouse_events;

			typedef std::map< cmn::uint, boost::shared_ptr<EventListener> > listmap;
			listmap m_listeners;
			void check_listeners(const Event& ev);
			void add_keydown_event(const SDL_keysym&);
			void add_keyup_event(const SDL_keysym&);
			void add_mouse_move_event(const SDL_MouseMotionEvent&);
			void add_mouse_button_event(const SDL_MouseButtonEvent&);
			void add_quit_event();
		public:
			SDLInputSystem();
	};

};

#endif
