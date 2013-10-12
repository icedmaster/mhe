#ifndef __COLOR_ANIMATION_COMPONENT_HPP__
#define __COLOR_ANIMATION_COMPONENT_HPP__

#include "linear_animation_component.hpp"
#include "message_color.hpp"

namespace mhe {
namespace game {

class ColorAnimationComponent : public LinearAnimationComponent<colorf>
{
public:
	ColorAnimationComponent(cmn::uint duration, const std::string& name) :
		LinearAnimationComponent<colorf>(duration, name)
	{}
private:
	std::string add_name_impl() const
	{
		return "color_animation";
	}

	void send_message(const colorf& value)
	{
		Component::send_message(ColorMessage(value, this));
	}
};

}}

#endif
