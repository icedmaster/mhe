#ifndef __COLOR_ANIMATION_COMPONENT_HPP__
#define __COLOR_ANIMATION_COMPONENT_HPP__

#include "linear_animation_component.hpp"
#include "message_color.hpp"

namespace mhe {
namespace game {

class ColorAnimationComponent : public LinearAnimationComponent<colorf>
{
private:
	void send_message(const colorf& value)
	{
		send_message(ColorMessage(value, this));
	}
};

}}

#endif
