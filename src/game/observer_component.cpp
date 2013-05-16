#include "game/observer_component.hpp"

#include "game/simple_messages.hpp"

namespace mhe {
namespace game {

void ObserverComponent::update_impl(cmn::uint /*tick*/, Engine*)
{
	for (size_t i = 0; i < components_.size(); ++i)
	{
		if (!components_[i].expired()) return;
	}
	// send end message to parent (not root!) and children
	EndEventMessage msg(this);
	parent()->update(msg);
	update_children(msg);
	detach_self();
}

}}
