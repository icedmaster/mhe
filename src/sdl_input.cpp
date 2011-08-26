#include "sdl_input.hpp"

namespace mhe
{
	SDLInputSystem::SDLInputSystem() :
        ws_(0)
	{
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	}

	void SDLInputSystem::check()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					add_keydown_event(event.key.keysym);
				break;

				case SDL_KEYUP:
					add_keyup_event(event.key.keysym);
				break;

				case SDL_MOUSEMOTION:
                	add_mouse_move_event(event.motion);
           		break;

				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
                	add_mouse_button_event(event.button);
            	break;

				case SDL_QUIT:
                	add_quit_event();
            	break;

				default: break;
			}
		}
	}

	void SDLInputSystem::handle()
	{
		/*for (size_t i = 0; i < events.size(); ++i)
			check_listeners(events[i].get());
		events.clear();*/
	}

	void SDLInputSystem::addListener(EventListener* el)
	{
		if (!get_event_optarg(el->id()) && get_event_arg(el->id()))
			arg_listmap.insert(mlisteners::value_type(el->id(),
		                                              boost::shared_ptr<EventListener>(el)));
		else if (get_event_arg(el->id()))
			listmap.insert(mlisteners::value_type(el->id(),
                                                  boost::shared_ptr<EventListener>(el)));
		else
			gl_listmap.insert(mlisteners::value_type(el->id(),
                                                     boost::shared_ptr<EventListener>(el)));
	}

	void SDLInputSystem::add_keydown_event(const SDL_keysym& sym)
	{
		KeyboardEvent ke(KEY_DOWN, sym.sym);
		//events.push_back(boost::shared_ptr<Event>(ke));
		check_listeners(ke);
	}

	void SDLInputSystem::add_keyup_event(const SDL_keysym& sym)
	{
		KeyboardEvent ke(KEY_UP, sym.sym);
		//events.push_back(boost::shared_ptr<Event>(ke));
		check_listeners(ke);
	}

	void SDLInputSystem::add_mouse_move_event(const SDL_MouseMotionEvent& event)
	{
		add_mouse_event(MOUSE_MOVE, event.x, event.y, event.state);
	}

	void SDLInputSystem::add_mouse_button_event(const SDL_MouseButtonEvent& event)
	{
		int type = (event.type == SDL_MOUSEBUTTONDOWN) ? MOUSE_BUTTON_PRESSED : MOUSE_BUTTON_RELEASED;
		add_mouse_event(type, event.x, event.y, event.button);
	}

	void SDLInputSystem::add_mouse_event(int type, int x, int y, int button)
	{
		y = ws_->height() - y;
		vector2<int> pos(x, y);
		int but = 0;
		switch (button)
		{
			case SDL_BUTTON_LEFT:  but = BUTTON_LEFT; break;
			case SDL_BUTTON_RIGHT: but = BUTTON_RIGHT; break;
			default: break;
		}
		MouseEvent me(type, but, pos);
		check_listeners(me);
	}

	void SDLInputSystem::add_quit_event()
	{
		SystemEvent se(QUIT);
		//events.push_back(boost::shared_ptr<Event>(se));
		check_listeners(se);
	}

	void SDLInputSystem::add_event_timestamp(Event* e)
	{
		e->set_timestamp(SDL_GetTicks());
	}

	void SDLInputSystem::check_listeners(Event& e)
	{
		add_event_timestamp(&e);
		mlitpair pit = listmap.equal_range(e.id());
        for (mlisteners::iterator it = pit.first; it != pit.second; ++it)
            it->second->handle(e);
		// check for global listeners
		if (arg_listmap.size())
		{
			int t = get_event_type(e.id());
			int gl_arg = get_event_arg(e.id());
			mlitpair pit = arg_listmap.equal_range(create_event_id(t, gl_arg, 0));
            for (mlisteners::iterator it = pit.first; it != pit.second; ++it)
                it->second->handle(e);
		}
		if (gl_listmap.size())
		{
			cmn::uint gl_id = get_event_type(e.id());
			mlitpair pit = gl_listmap.equal_range(gl_id);
			for (mlisteners::iterator it = pit.first; it != pit.second; ++it)
				it->second->handle(e);
		}
	}
}
