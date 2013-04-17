#include "gui/button.hpp"

namespace mhe {
namespace gui {

void Button::draw(const Context& context)
{
	if (mouse_on() && button_pressed())
		draw_impl(pressed_sprite_, context);
	else Widget::draw(context);
}

}}
