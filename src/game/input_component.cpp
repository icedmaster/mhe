#include "game/input_component.hpp"

#include "game/mouse_messages.hpp"

namespace mhe {
namespace game {

bool MouseInputComponent::handle(const Event* event)
{
	const MouseEvent* me = static_cast<const MouseEvent*>(event);
	if (!rect_.is_empty() && !rect_.in(me->pos()))
	{
		mouse_on_ = false;
		handle_mouse_off();
		return false;
	}
	if (!mouse_on_)
	{
		mouse_on_ = true;
		handle_mouse_on();
	}
	handle_mouse_impl(me->pos(), me->event_type(), me->button());
	return false;
}

void MouseInputComponent::handle_mouse_on()
{
	update_children(MouseOnMessage(this));
}

void MouseInputComponent::handle_mouse_off()
{
	update_children(MouseOffMessage(this));
}

}}
