#include "game/move_animation_components.hpp"

namespace mhe {
namespace game {

void MoveAnimationComponent::do_subscribe(Component* component)
{
	assert(dynamic_cast<NodeComponent*>(component) != nullptr);
	parent_node_ = static_cast<NodeComponent*>(component);
}

// translate by
void TranslateByAnimationComponent::update_animation_impl(float v)
{
	node_component()->translate_to(initial_position_ + translation_ * v);
}

// rotate by
void RotateByAnimationComponent::update_animation_impl(float v)
{
	node_component()->rotate_to(initial_rotation_ + rotation_ * v);
}

// scale by
void ScaleByAnimationComponent::update_animation_impl(float v)
{
	node_component()->scale_to(initial_scale_ + scale_ * v);
}

}}
