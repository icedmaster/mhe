#include "game/animation_component.hpp"
#include "game/message_transform.hpp"

namespace mhe {
namespace game {

void AnimationComponent::update_impl(cmn::uint tick)
{
	switch (animation_list_->update(tick))
	{
		case AnimationListBase::next_animation:
			send_animation_message();
			break;

		default: break;
	}
}

// TransformAnimationComponent
void TransformAnimationComponent::send_animation_message()
{
	send_message(TransformMessage(get_animation_list<TransformLinearAnimationList>()->current_value(), this));
}

}}
