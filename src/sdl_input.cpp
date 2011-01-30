#include "sdl_input.hpp"
#include "globals.hpp"

namespace mhe {

SDLInputSystem::SDLInputSystem()
{
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	keyboard_events.reserve(5);
}

void SDLInputSystem::check_impl()
{
	SDL_Event event;
	SDL_PollEvent(&event);
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


void SDLInputSystem::handle_impl()
{
	// handle keyboard events
	if (keyboard_handler)
	{
		for (std::vector<KeyboardEvent>::iterator i = keyboard_events.begin();
												  i != keyboard_events.end();
												  ++i)
		{
			if (keyboard_handler->handle(*i))
			{
				// event handled - remove it
				keyboard_events.erase(i);
				--i;
			}
		}
	}
	else
        keyboard_events.clear();

	// handle mouse events
	if (mouse_handler)
	{
		for (std::vector<MouseEvent>::iterator i = mouse_events.begin();
                                               i != mouse_events.end();
											   ++i)
		{
			if (mouse_handler->handle(*i))
			{
				// event handled - remove it
				mouse_events.erase(i);
				--i;
			}
		}
	}
	else
        mouse_events.clear();
}


void SDLInputSystem::add_listener(EventListener* el)
{
	m_listeners.insert(listmap::value_type(el->id(), boost::shared_ptr<EventListener>(el)));
}


void SDLInputSystem::check_listeners(const Event& event)
{
    listmap::iterator it = m_listeners.find(event.id());
    if (it == m_listeners.end()) return;
    it->second->handle(event);
}


void SDLInputSystem::add_keydown_event(const SDL_keysym& sym)
{
	KeyboardEvent ke(KEY_DOWN, (cmn::uint)sym.sym);
	keyboard_events.push_back(ke);
	check_listeners(ke);
}


void SDLInputSystem::add_quit_event()
{
    SystemEvent se(QUIT);
    check_listeners(se);
}


void SDLInputSystem::add_keyup_event(const SDL_keysym& sym)
{
	KeyboardEvent ke(KEY_UP, sym.sym);
	keyboard_events.push_back(ke);
}


void SDLInputSystem::add_mouse_move_event(const SDL_MouseMotionEvent& event)
{
    Point p;
    p.x = event.x;
    p.y = globals::instance().window_height() - event.y;
    MouseEvent me(MOUSE_MOVE, p, event.state);
    mouse_events.push_back(me);
    check_listeners(me);
}


void SDLInputSystem::add_mouse_button_event(const SDL_MouseButtonEvent& event)
{
    int type = (event.type == SDL_MOUSEBUTTONDOWN) ? MOUSE_BUTTON_PRESSED : MOUSE_BUTTON_RELEASED;
    cmn::uint but;
    switch (event.button)
    {
        case SDL_BUTTON_LEFT:  but = BUTTON_LEFT; break;
        case SDL_BUTTON_RIGHT: but = BUTTON_RIGHT; break;
        default: return;    // not supported now
    }
    Point p;
    p.x = event.x;
    p.y = globals::instance().window_height()- event.y;
    MouseEvent me(type, p, but);
    mouse_events.push_back(me);
    check_listeners(me);
}


void SDLInputSystem::set_keyboard_event_handler(KeyboardEventHandler *keh)
{
	keyboard_handler.reset(keh);
}


void SDLInputSystem::set_mouse_event_handler(MouseEventHandler* meh)
{
    mouse_handler.reset(meh);
}

};	// namespace
