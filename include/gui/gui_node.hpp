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

    void set_widget(Widget* widget);
	void add_widget(Widget* widget);
	widgetptr get_widget(const std::string& name) const;
private:
	void draw_impl(Context& context);
private:
	bool on_mouse_move(const Event* event);
	bool on_mouse_button_pressed(const Event* event);
	bool on_mouse_button_released(const Event* event);
private:
	widgetptr widget_;
};

}}

#endif
