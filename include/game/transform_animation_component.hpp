#ifndef __TRANSFORM_ANIMATION_COMPONENT_HPP__
#define __TRANSFORM_ANIMATION_COMPONENT_HPP__

#include "linear_animation_component.hpp"
#include "message_transform.hpp"

namespace mhe {
namespace game {

class TransformAnimationComponent : public LinearAnimationList<matrixf>
{
private:
	void send_message(const matrixf& value)
	{	
		send_message(TransformMessage(value, this));
	}
};

}}

#endif
