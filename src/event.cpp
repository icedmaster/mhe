#include "event.hpp"
#include "utils/sysutils.hpp"

namespace mhe {

void InputSystem::add_listener(boost::shared_ptr<EventListener> el)
{
	Listener listener;
	listener.listener = boost::weak_ptr<EventListener>(el);
	if (get_event_type(el->id()) == TimerEventType)
	{
		add_timer_listener(listener);
		return;
	}

	add_listener(listener);
}

void InputSystem::add_listener(const Listener& l)
{
	boost::shared_ptr<EventListener> el = l.listener.lock();	
	if (!get_event_optarg(el->id()) && get_event_arg(el->id()))
		arg_listmap.insert(mlisteners::value_type(el->id(), l));
	else if (get_event_arg(el->id()))
		listmap.insert(mlisteners::value_type(el->id(), l));
	else
		gl_listmap.insert(mlisteners::value_type(el->id(), l));
												 
}

void InputSystem::set_input_state(bool enable)
{
	input_enabled_ = enable;
}

void InputSystem::add_timer_listener(const Listener& listener)
{
	TimerListener tl;
	boost::shared_ptr<EventListener> el = listener.listener.lock();
	tl.start = utils::get_current_tick();
	tl.once = (get_event_optarg(el->id()) == TimerEvent::TIMER_ONCE) ? true : false;
	tl.listener = listener;
	timer_listeners_.push_back(tl);
}

void InputSystem::add_listener(EventListener* el)
{
	// once listener
	Listener listener;
	listener.once_listener = boost::shared_ptr<EventListener>(el);
	listener.listener = boost::weak_ptr<EventListener>(listener.once_listener);
	if (get_event_type(el->id()) == TimerEventType)
	{
		add_timer_listener(listener);
		return;
	}

	add_listener(listener);
}

void InputSystem::check_timer_events()
{
	cmn::uint now = utils::get_current_tick();
	for (std::vector<TimerListener>::iterator it = timer_listeners_.begin();
		 it != timer_listeners_.end();)
	{
		TimerListener& tl = *it;
		cmn::uint delay = get_event_arg(tl.listener.listener.lock()->id());
		if (now >= (tl.start + delay))
		{
			if (tl.listener.listener.expired())
			{
				it = timer_listeners_.erase(it);
				continue;
			}
			SystemEvent se(TimerEventType);
			add_event_timestamp(&se);
			tl.listener.listener.lock()->handle(se);
			if (tl.once || tl.listener.once_listener)
			{
				it = timer_listeners_.erase(it);
				continue;
			}
			tl.start = now;
		}
		++it;
	}
}

void InputSystem::add_event_timestamp(Event* e)
{
	e->set_timestamp(utils::get_current_tick());
}

void InputSystem::check_listeners(Event& e)
{
	if (!filter_event(e)) return;
	add_event_timestamp(&e);
	mlitpair pit = listmap.equal_range(e.id());
	for (mlisteners::iterator it = pit.first; it != pit.second;)
	{
		if (it->second.listener.expired())
		{
			listmap.erase(it++);
			continue;
		}
		it->second.listener.lock()->handle(e);
		if (it->second.once_listener) listmap.erase(it++);
		else ++it;
	}
	// check for global listeners
	if (arg_listmap.size())
	{
		int t = get_event_type(e.id());
		int gl_arg = get_event_arg(e.id());
		mlitpair pit = arg_listmap.equal_range(create_event_id(t, gl_arg, 0));
		for (mlisteners::iterator it = pit.first; it != pit.second;)
		{
			if (it->second.listener.expired())
			{
				arg_listmap.erase(it++);
				continue;
			}
			it->second.listener.lock()->handle(e);
			if (it->second.once_listener) listmap.erase(it++);			
			else ++it;
		}
	}
	if (gl_listmap.size())
	{
		cmn::uint gl_id = get_event_type(e.id());
		mlitpair pit = gl_listmap.equal_range(gl_id);
		for (mlisteners::iterator it = pit.first; it != pit.second;)
		{
			if (it->second.listener.expired())
			{
				gl_listmap.erase(it++);
				continue;
			}
			it->second.listener.lock()->handle(e);
			if (it->second.once_listener) listmap.erase(it++);			
			else ++it;
		}
	}
}

bool InputSystem::filter_event(const Event& e) const
{
	if (input_enabled_) return true;
	if (e.type() == MouseEventType)
	{
		if (get_event_arg(e.id()) != MOUSE_BUTTON_RELEASED)
			return false;
	}

	if (e.type() == KeyboardEventType)
	{
		if (get_event_arg(e.id()) != KEY_UP)
			return false;
	}
	return true;
}


}
