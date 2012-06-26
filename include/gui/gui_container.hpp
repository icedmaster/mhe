#ifndef _GUI_CONTAINER_HPP_
#define _GUI_CONTAINER_HPP_

#include <vector>
#include "node.hpp"
#include "mhe_event.hpp"
#include "widget.hpp"

namespace mhe {
namespace gui {

class GUIContainer : public Node
{
public:
	GUIContainer();
	GUIContainer(EventManager& is);

	void add(const boost::shared_ptr<Widget>& widget);
	boost::shared_ptr<Widget> get(const std::string& name) const;

	void setup_events(EventManager& is);
private:
	void draw_impl(boost::shared_ptr<Driver> driver);
	int get_priority() const
	{
		return 5;	// GUI will draw at last point
	}
private:
	friend class EventListener;

	bool handle_mouse_click(const boost::shared_ptr<Event>& e);
	bool handle_mouse_move(const boost::shared_ptr<Event>& e);

	std::vector< boost::shared_ptr<Widget> > widgets_;

	/*boost::shared_ptr<gui_event_handler> mouse_move_listener_;				
	boost::shared_ptr<gui_event_handler> mouse_click_listener_;
	boost::shared_ptr<gui_event_handler> mouse_release_listener_;*/
};
}}

#endif
