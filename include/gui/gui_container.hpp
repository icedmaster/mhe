#ifndef _GUI_CONTAINER_HPP_
#define _GUI_CONTAINER_HPP_

#include "node.hpp"
#include "event.hpp"
#include "widget.hpp"
#include <vector>

namespace mhe
{
	namespace gui
	{
		class GUIContainer : public Node
		{
			private:
				std::vector< boost::shared_ptr<Widget> > widgets_;

				typedef PrivateEventListener<GUIContainer> gui_event_handler;
				friend class PrivateEventListener<GUIContainer>;

				boost::shared_ptr<gui_event_handler> mouse_move_listener_;				
				boost::shared_ptr<gui_event_handler> mouse_click_listener_;
				boost::shared_ptr<gui_event_handler> mouse_release_listener_;

				bool handle_mouse_click(const Event& e);
				bool handle_mouse_move(const Event& e);
			private:
				void draw_impl(boost::shared_ptr<Driver> driver);
				int get_priority() const
				{
					return 5;	// GUI will draw at last point
				}
			public:
				GUIContainer();
				GUIContainer(boost::shared_ptr<InputSystem> is);

				void add(const boost::shared_ptr<Widget>& widget);
				boost::shared_ptr<Widget> get(const std::string& name) const;

				void setupEvents(boost::shared_ptr<InputSystem> is);
		};
	}
}

#endif
