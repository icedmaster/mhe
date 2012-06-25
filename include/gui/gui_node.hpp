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
	bool on_mouse_move(const boost::shared_ptr<Event>& event);
	bool on_mouse_button_pressed(const boost::shared_ptr<Event>& event);
	bool on_mouse_button_released(const boost::shared_ptr<Event>& event);
private:
	widgetptr widget_;
};

}}

#endif
