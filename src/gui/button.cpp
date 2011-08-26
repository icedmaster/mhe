#include "gui/button.hpp"

namespace mhe {
namespace gui {

	int Button::supported_handlers[Button::supported_handlers_number] = {OnMouseMove, OnMouseLeft,
																	     OnMouseLeftClick, OnMouseRightClick};

	Button::Button() :
		mouse_on_(false),
		enabled_(true),
		pressed_(false)
	{
	}

	void Button::draw_impl(const boost::shared_ptr<iDriver>& driver)
	{
		boost::shared_ptr<iTexture> cur = texture_;
		if (pressed_)
			cur = pressed_texture_;

		Widget::draw_rect(driver, cur);
	}

	void Button::set_handler(int event, const guieventptr& handler)
	{
		int* it = std::find(supported_handlers,
							supported_handlers + supported_handlers_number,
							event);
		if ( it == (supported_handlers + supported_handlers_number) ) return;
		// delete previous handler
		handlers.erase(event);
		// set new handler
		handlers[event] = handler;
	}

	guieventptr Button::get_gui_handler(int event) const
	{
		if (!get_visible() || !enabled_) return guieventptr();
		std::map<int, guieventptr>::const_iterator it = handlers.find(event);
		if (it == handlers.end())
			return guieventptr();
		return it->second;
	}

	void Button::process_mouse_left_click(const MouseEvent& me)
	{
		pressed_ = true;
		Widget::process_mouse_left_click(me);
	}

	void Button::process_mouse_button_release(const MouseEvent& me)
	{
		pressed_ = false;
		Widget::process_mouse_button_release(me);
	}

	void Button::process_mouse_left(const MouseEvent& me)
	{
		pressed_ = false;
		Widget::process_mouse_left(me);
	}

}
}
