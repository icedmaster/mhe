#include "game/animation_component.hpp"
#include "game/message_transform.hpp"

namespace mhe {
namespace game {

// TransformAnimationComponent
void TransformAnimationComponent::send_animation_message()
{
	animation_list()->update_node(nullptr);
    parent()->update(
		TransformMessage(transform_animation_list()->current_value(), this));
}

}}
