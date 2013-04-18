#include "gui/gui_node.hpp"

namespace mhe {
namespace gui {

GUINode::GUINode()
{}

GUINode::GUINode(EventManager& event_manager)
{
	setup_events(event_manager);
}

GUINode::GUINode(Widget* widget) :
	widget_(widget)
{
}

void GUINode::setup_events(EventManager& event_manager)
{
	event_manager.add_listener(new DelegateEventListener(mouse_event_type, MouseEvent::move, Event::any_event,
														 create_delegate(this, &GUINode::on_mouse_move)));
	event_manager.add_listener(new DelegateEventListener(mouse_event_type, MouseEvent::button_pressed, Event::any_event,
												 create_delegate(this, &GUINode::on_mouse_button_pressed)));
	event_manager.add_listener(new DelegateEventListener(mouse_event_type, MouseEvent::button_released, Event::any_event,
														 create_delegate(this, &GUINode::on_mouse_button_released)));
}

void GUINode::set_widget(Widget* widget)
{
    widget_.reset(widget);
}

void GUINode::add_widget(Widget* widget)
{
	widget_->add_widget(widgetptr(widget));
}

widgetptr GUINode::get_widget(const std::string& name) const
{
	return widget_->get_widget(name);
}

void GUINode::draw_impl(const Context& context)
{
	widget_->draw(context);
}

bool GUINode::on_mouse_move(const Event* event)
{
	widget_->process_mouse_move_event(static_cast<const MouseEvent*>(event));
	return true;
}

bool GUINode::on_mouse_button_pressed(const Event* event)
{
	widget_->process_mouse_press_event(static_cast<const MouseEvent*>(event));
	return true;
}

bool GUINode::on_mouse_button_released(const Event* event)
{
	widget_->process_mouse_release_event(static_cast<const MouseEvent*>(event));
	return true;
}

}}
