#ifndef __TRANSFORM_ANIMATION_COMPONENT_HPP__
#define __TRANSFORM_ANIMATION_COMPONENT_HPP__

#include "linear_animation_component.hpp"
#include "message_transform.hpp"

namespace mhe {
namespace game {

class TransformAnimationComponent : public LinearAnimationComponent<matrixf>
{
public:
	TransformAnimationComponent(cmn::uint duration, const std::string& name) :
		LinearAnimationComponent<matrixf>(duration, name)
	{}
private:
	std::string add_name_impl() const
	{
		return "transform";
	}

	void send_message(const matrixf& value)
	{	
		Component::send_message(TransformMessage(value, this));
	}
};

}}

#endif
