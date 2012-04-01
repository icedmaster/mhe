#include "gui/cursor.hpp"

namespace mhe {
namespace gui {

Cursor::Cursor(boost::shared_ptr<InputSystem> is) :
		move_handler_(new cursor_event_handler(MouseEventType, MOUSE_MOVE, 0,
												this, &Cursor::handle_mouse_move)),
		click_handler_(new cursor_event_handler(MouseEventType, MOUSE_BUTTON_PRESSED, 0,
												this, &Cursor::handle_mouse_click))
	{
		is->add_listener(move_handler_);
		is->add_listener(click_handler_);
	}

	void Cursor::setSprite(const boost::shared_ptr<Sprite>& sprite)
	{
		sprite_ = sprite;
	}

	bool Cursor::handle_mouse_move(const Event& e)
	{
		const MouseEvent& me = dynamic_cast<const MouseEvent&>(e);
		pos_ = me.position();
		return true;
	}

	bool Cursor::handle_mouse_click(const Event& e)
	{
		static const int default_animation_number = 0;
		static const int clicked_animation_number = 1;
		const MouseEvent& me = dynamic_cast<const MouseEvent&>(e);
		if (e.type() == MOUSE_BUTTON_RELEASED)
			clicked_ = false;
		else if (me.button() == BUTTON_LEFT)
			clicked_ = true;
		int anum = (clicked_) ? clicked_animation_number : 
								default_animation_number;
		sprite_->execute(anum);			
		return true;
	}

	void Cursor::update_impl(cmn::uint tick)
	{
		sprite_->update(tick);
	}

	void Cursor::draw_impl(boost::shared_ptr<Driver> driver)
	{
		sprite_->identity();
		sprite_->translate(v3d(pos_.x() - sprite_->width() / 2,
							   pos_.y() - sprite_->height() / 2, 0));
		sprite_->draw(driver);
	}

}
}
