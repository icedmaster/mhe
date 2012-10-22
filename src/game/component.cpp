#include "game/component.hpp"
#include "utils/logutils.hpp"

namespace mhe {
namespace game {

void Component::attach(component_ptr aspect)
{
	DEBUG_LOG("Component::attach() " << full_name() << " " << aspect->full_name());
	children_.push_back(aspect);
	aspect->set_parent(this);
	aspect->do_subscribe(this);
}

void Component::attach(component_ptr aspect, const std::vector<int>& types)
{
	DEBUG_LOG("Component::attach() " << full_name() << " " << aspect->full_name());
	children_.push_back(aspect);
	aspect->set_parent(this);
	for (size_t i = 0; i < types.size(); ++i)
		subscribe(types[i], aspect.get());
}

void Component::attach(component_ptr aspect, int type)
{
	DEBUG_LOG("Component::attach() " << full_name() << " " << aspect->full_name());
	children_.push_back(aspect);
	aspect->set_parent(this);
	subscribe(type, aspect.get());
}

void Component::detach(Component* aspect)
{
	DEBUG_LOG("Component::detach():" << full_name() << " " << aspect->full_name());
	// remove aspect from subscribers
	for (subsmap::iterator it = subscribers_.begin(); it != subscribers_.end(); ++it)
	{
		std::vector<Component*>& childs = it->second;
		for (std::vector<Component*>::iterator vit = childs.begin(); vit != childs.end(); ++vit)
		{
			if (*vit == aspect)
			{
				DEBUG_LOG("Component removed from subscribers:" << full_name() << " " << aspect->full_name());
				childs.erase(vit);
				break;
			}
		}
	}
	// remove aspect from childs
	for (std::vector<component_ptr>::iterator it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it).get() == component)
		{
			DEBUG_LOG("Component detached:" << full_name() << " " << component->full_name());
			children_.erase(it);
			break;
		}
	}
}

void Component::update(const Message& message)
{
	if (message.sender() == this) return;
	if (message.type() == destroy_event)
	{
		DEBUG_LOG("Component " << full_name() << " receive destroy event");
		destroy_impl();
		for (size_t i = 0; i < children_.size(); ++i)
		{
			children_[i]->update(message);
		}
	}
	else if (update_impl(message))
		update_children(message);
}

void Component::update_children(const Message& message)
{
	subsmap::iterator it = subscribers_.find(message.type());
	if (it == subscribers_.end()) return;
	for (size_t i = 0; i < it->second.size(); ++i)
		it->second[i]->update_impl(message);
}

void Component::subscribe(int type, Component* component)
{
	subscribers_[type].push_back(component);
}

void Component::send_message(const Message& message)
{
	update_children(message);
	parent_->update(message);
}

}}	// namespaces
