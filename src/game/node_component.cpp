#include "game/node_component.hpp"

namespace mhe {
namespace game {

void NodeComponent::do_subscribe(Component* parent)
{
	parent->subscribe(transform_event, this);
	parent->subscribe(enable_event, this);
}

bool NodeComponent::update_impl(const Message& message)
{
	if (message.type() == transform_event)
		process_transform_event(static_cast<const TransformMessage&>(message));
	else if (message.type() == enable_event)
		process_enable_event(static_cast<const EnableMessage&>(message));
	return true;
}

void NodeComponent::process_transform_event(const TransformMessage& message)
{
	node_->apply_transform(message.transform());
}

void NodeComponent::process_enable_event(const EnableMessage& message)
{
	if (at_scene_ == message.enabled()) return;
	at_scene_ = message.enabled();
	if (at_scene_) scene_->add(node_);
	else scene_->remove(node_);
}

}}
