#ifndef _CURSOR_HPP_
#define _CURSOR_HPP_

#include "sprite.hpp"
#include "event.hpp"

namespace mhe
{
	namespace gui
	{
		class Cursor : public iNode
		{
			private:
				boost::shared_ptr<Sprite> sprite_;
				vector2<int> pos_;
				bool clicked_;
				typedef PrivateEventListener<Cursor> cursor_event_handler;
				friend class PrivateEventListener<Cursor>;

				bool handle_mouse_move(const Event& e);
				bool handle_mouse_click(const Event& e);
			private:
				void draw_impl(const boost::shared_ptr<iDriver>& driver);
				void update_impl(cmn::uint tick);
			public:
				Cursor(InputSystem& is);

				void setSprite(const boost::shared_ptr<Sprite>& sprite);
		};
	}
}

#endif
