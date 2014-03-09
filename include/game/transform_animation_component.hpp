#ifndef __TRANSFORM_ANIMATION_COMPONENT_HPP__
#define __TRANSFORM_ANIMATION_COMPONENT_HPP__

#include "linear_animation_component.hpp"
#include "node_component.hpp"
#include "message_transform.hpp"

namespace mhe {
namespace game {

class TransformAnimationComponent : public LinearAnimationComponent<matrixf>
{
public:
	TransformAnimationComponent(cmn::uint duration, const std::string& name, bool check_initial_transform = false) :
		LinearAnimationComponent<matrixf>(duration, name),
		check_initial_(check_initial_transform)
	{}
private:
	void do_subscribe(Component* component)
	{
		if (!check_initial_) return;
		initial_ = component->transform();
	}

	std::string add_name_impl() const
	{
		return "transform";
	}

	void send_message(const matrixf& value)
	{	
		Component::send_message(TransformMessage(check_initial_ ? initial_ * value : value, this));
	}
private:
	mat4x4 initial_;
	bool check_initial_;
};

}}

#endif
