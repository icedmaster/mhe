#include "game/node_component.hpp"

namespace mhe {
namespace game {

void NodeComponent::do_subscribe(Component* parent)
{
	parent->subscribe(transform_event, this);
	parent->subscribe(enable_event, this);
	parent->subscribe(color_event, this);
}

bool NodeComponent::update_impl(const Message& message)
{
	if (message.type() == transform_event)
		process_transform_event(static_cast<const TransformMessage&>(message));
	else if (message.type() == enable_event)
		process_enable_event(static_cast<const EnableMessage&>(message));
	else if (message.type() == color_event)
		process_color_event(static_cast<const ColorMessage&>(message));
	return true;
}

void NodeComponent::process_transform_event(const TransformMessage& message)
{
	Node::set_transform(message.transform());
}

void NodeComponent::process_enable_event(const EnableMessage& message)
{
	// set frozen flag
}

void NodeComponent::process_color_event(const ColorMessage& message)
{
	Node::set_color(message.color());
}

}}
