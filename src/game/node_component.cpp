#include "game/node_component.hpp"

namespace mhe {
namespace game {

void NodeComponent::do_subscribe(Component* parent)
{
	parent->subscribe(transform_event);
}

bool NodeComponent::update_impl(const Message& message)
{
	if (message.type() == transform_event)
		process_transform_event(static_cast<const MessageTransform&>(message));
	return true;
}

void NodeComponent::process_transform_event(const MessageTransform& message)
{
	node_->apply_transform(message.transform());
}

}}
