#include "gui/button.hpp"
#include "utils/sysutils.hpp"

namespace mhe {
namespace gui {

	bool Button::is_handler_supported(int event) const
	{
		static const int supported[] = {OnMouseMove, OnMouseLeft,
									    OnMouseLeftClick, OnMouseRightClick};
		if (std::find(supported, supported + 4, event) == (supported + 4))
			return false;
		return true;
	}

	void Button::process_mouse_left_click(const MouseEvent& me)
	{
		get_sprite()->execute(1);
		Widget::process_mouse_left_click(me);
	}

	void Button::process_mouse_button_release(const MouseEvent& me)
	{
		get_sprite()->execute(0);
		Widget::process_mouse_button_release(me);
	}

	void Button::process_mouse_left(const MouseEvent& me)
	{
		get_sprite()->execute(0);
		Widget::process_mouse_left(me);
	}

}
}
