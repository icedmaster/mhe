#ifndef __GUI_NODE_HPP__
#define __GUI_NODE_HPP__

#include "node.hpp"
#include "mhe_event.hpp"
#include "widget.hpp"

namespace mhe {
namespace gui {

class GUINode : public Node
{
	friend class EventListener;
public:
	GUINode();
	GUINode(EventManager& event_manager);
	GUINode(Widget* widget);

	void setup_events(EventManager& event_manager);

	void add_widget(Widget* widget);
	widgetptr get_widget(const std::string& name) const;
private:
	void draw_impl(const Context& context);
private:
	bool on_mouse_move(Event* event);
	bool on_mouse_button_pressed(Event* event);
	bool on_mouse_button_released(Event* event);
private:
	widgetptr widget_;
};

}}

#endif
