#include "gui/event_widget.hpp"

namespace mhe {
namespace gui {

EventWidget::EventWidget() :
	mouse_on_(false), button_pressed_(false)
{}

void EventWidget::add_handler(int event, EventHandler* handler)
{
	handlers_[event] += handler;
}

void EventWidget::process_mouse_move_event(const boost::shared_ptr<MouseEvent>& event)
{
	if (geom_.in(event->pos()))
	{
		if (!mouse_on_)
			process_event(mouse_move_in_event, event);
		mouse_on_ = true;
		process_event(mouse_move_event, event);
	}
	else if (mouse_on_)
	{
		mouse_on_ = false;
		process_event(mouse_move_out_event, event);
	}
}

void EventWidget::process_mouse_press_event(const boost::shared_ptr<MouseEvent>& event)
{
	if (geom_.in(event->pos()))
	{
		button_pressed_ = true;
		process_event(mouse_button_press_event, event);
	}
}

void EventWidget::process_mouse_release_event(const boost::shared_ptr<MouseEvent>& event)
{
	if (button_pressed_)
	{
		button_pressed_ = false;
		process_event(mouse_button_release_event, event);
	}
}

void EventWidget::process_event(int event, const boost::shared_ptr<MouseEvent>& mouse_event)
{
	std::map<int, Delegate>::iterator it = handlers_.find(event);
	if (it == handlers_.end()) return;
	it->second(this);
	process_event_impl(event, mouse_event);
}

}}
