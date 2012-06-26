#include "gui/gui_container.hpp"

namespace mhe {
namespace gui {

GUIContainer::GUIContainer()
{}


GUIContainer::GUIContainer(EventManager& is)
{
	setup_events(is);
}

void GUIContainer::add(const boost::shared_ptr<Widget>& widget)
{
	widgets_.push_back(widget);
}

boost::shared_ptr<Widget> GUIContainer::get(const std::string& name) const
{
	for (size_t i = 0; i < widgets_.size(); ++i)
	{
		if (widgets_[i]->get_name() == name)
			return widgets_[i];
	}
	return boost::shared_ptr<Widget>();
}

void GUIContainer::draw_impl(boost::shared_ptr<Driver> driver)
{
	for (size_t i = 0; i < widgets_.size(); ++i)
		widgets_[i]->draw(driver);
}

bool GUIContainer::handle_mouse_click(const boost::shared_ptr<Event>& e)
{
	const boost::shared_ptr<MouseEvent>& me = static_cast<const boost::shared_ptr<MouseEvent>&>(e);

	for (size_t i = 0; i < widgets_.size(); ++i)
	{
		if (widgets_[i]->handle_mouse_click(me))
			return true;
	}

	return true;
}

bool GUIContainer::handle_mouse_move(const boost::shared_ptr<Event>& e)
{
	const boost::shared_ptr<MouseEvent>& me = static_cast<const boost::shared_ptr<MouseEvent>&>(e);

	for (size_t i = 0; i < widgets_.size(); ++i)
	{
		if (widgets_[i]->handle_mouse_move(me))
			return true;
	}

	return true;
}

void GUIContainer::setup_events(EventManager& is)
{
	is.add_listener(new EventListener(mouse_event_type, MouseEvent::move, Event::any_event,
									  create_delegate(this, &GUIContainer::handle_mouse_move)));
	is.add_listener(new EventListener(mouse_event_type, MouseEvent::button_pressed, Event::any_event,
									  create_delegate(this, &GUIContainer::handle_mouse_click)));
	is.add_listener(new EventListener(mouse_event_type, MouseEvent::button_released, Event::any_event,
									  create_delegate(this, &GUIContainer::handle_mouse_click)));
}

}
}
