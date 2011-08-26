#ifndef _GUI_CONTAINER_HPP_
#define _GUI_CONTAINER_HPP_

#include "inode.hpp"
#include "event.hpp"
#include "widget.hpp"
#include <vector>

namespace mhe
{
	namespace gui
	{
		class GUIContainer : public iNode
		{
			private:
				std::vector< boost::shared_ptr<Widget> > widgets_;

				typedef PrivateEventListener<GUIContainer> gui_event_handler;
				friend class PrivateEventListener<GUIContainer>;

				bool handle_keyboard(const Event& e);
				bool handle_mouse_click(const Event& e);
				bool handle_mouse_move(const Event& e);
			private:
				void draw_impl(const boost::shared_ptr<iDriver>& driver);
				int get_priority() const
				{
					return 5;	// GUI will draw at last point
				}
			public:
				GUIContainer();
				GUIContainer(InputSystem& is);

				void add(const boost::shared_ptr<Widget>& widget);

				void setupEvents(InputSystem& is);
		};
	}
}

#endif
