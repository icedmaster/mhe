#ifndef _CURSOR_HPP_
#define _CURSOR_HPP_

#include "sprite.hpp"
#include "event.hpp"

namespace mhe
{
	namespace gui
	{
		class Cursor : public Node
		{
			private:
				boost::shared_ptr<Sprite> sprite_;
				vector2<int> pos_;
				bool clicked_;
				typedef PrivateEventListener<Cursor> cursor_event_handler;
				friend class PrivateEventListener<Cursor>;
				boost::shared_ptr<cursor_event_handler> move_handler_;
				boost::shared_ptr<cursor_event_handler> click_handler_;

				bool handle_mouse_move(const Event& e);
				bool handle_mouse_click(const Event& e);
			private:
				void draw_impl(boost::shared_ptr<Driver> driver);
				void update_impl(cmn::uint tick);
			public:
				Cursor(boost::shared_ptr<InputSystem> is);

				void setSprite(const boost::shared_ptr<Sprite>& sprite);
		};
	}
}

#endif
