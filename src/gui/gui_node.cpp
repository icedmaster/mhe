#include "gui/gui_node.hpp"

namespace mhe {
namespace gui {

GUINode::GUINode()
{
}

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
	event_manager.add_listener(new EventListener(mouse_event_type, MouseEvent::move, Event::any_event,
												 create_delegate(this, &GUINode::on_mouse_move)));
	event_manager.add_listener(new EventListener(mouse_event_type, MouseEvent::button_pressed, Event::any_event,
												 create_delegate(this, &GUINode::on_mouse_button_pressed)));
	event_manager.add_listener(new EventListener(mouse_event_type, MouseEvent::button_released, Event::any_event,
												 create_delegate(this, &GUINode::on_mouse_button_released)));
}

void GUINode::add_widget(Widget* widget)
{
	widget_->add_widget(widgetptr(widget));
}

widgetptr GUINode::get_widget(const std::string& name) const
{
	return widget_->get_widget(name);
}

bool GUINode::on_mouse_move(Event* event)
{
	widget_->process_mouse_move_event(static_cast<MouseEvent*>(event));
	return true;
}

bool GUINode::on_mouse_button_pressed(Event* event)
{
	widget_->process_mouse_press_event(static_cast<MouseEvent*>(event));
	return true;
}

bool GUINode::on_mouse_button_released(Event* event)
{
	widget_->process_mouse_release_event(static_cast<MouseEvent*>(event));
	return true;
}

}}
