#ifndef _SDL_INPUT_HPP_
#define _SDL_INPUT_HPP_

#include "event.hpp"
#include <map>
#include <vector>
#include <SDL/SDL.h>

namespace mhe
{
	class SDLInputSystem : public iInputSystem
	{
		private:
			typedef std::multimap< cmn::uint, boost::shared_ptr<EventListener> > mlisteners;
			typedef std::pair< mlisteners::iterator, mlisteners::iterator > mlitpair;

			struct TimerListener
			{
				cmn::uint start;
				boost::shared_ptr<EventListener> listener;
				bool once;
			};

			std::vector<TimerListener> timer_listeners_;

			mlisteners listmap;
			mlisteners arg_listmap;
			mlisteners gl_listmap;
			//std::vector< boost::shared_ptr<Event> > events;
			WindowSystem* ws_;
			bool input_enabled_;

			void check_listeners(Event& e);

			void add_keydown_event(const SDL_keysym&);
			void add_keyup_event(const SDL_keysym&);
			void add_mouse_move_event(const SDL_MouseMotionEvent&);
			void add_mouse_button_event(const SDL_MouseButtonEvent&);
			void add_mouse_event(int type, int x, int y, int button);
			void add_quit_event();
			void add_tick_event();

			void add_event_timestamp(Event* e);

			void add_timer_listener(EventListener* el);
			void check_timer_events();

			bool filter_event(const Event& e) const;
		public:
			SDLInputSystem();

			void check();
			void handle();
			void addListener(EventListener* el);
			void setWindowSystem(WindowSystem* ws)
			{
				ws_ = ws;
			}

			void set_input_state(bool enable);
	};
}

#endif
