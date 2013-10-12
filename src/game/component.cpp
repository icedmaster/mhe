#include "game/component.hpp"
#include "utils/logutils.hpp"

namespace mhe {
namespace game {

void Component::attach(component_ptr component)
{
	DEBUG_LOG("Component::attach() " << full_name() << " " << component->full_name());
	children_.push_back(component);
	component->set_parent(this);
	component->root_ = (root_ == nullptr) ? this : root_;
	component->do_subscribe(this);
	on_attach(component.get());    
}

void Component::attach(component_ptr component, const std::vector<int>& types)
{
	DEBUG_LOG("Component::attach() " << full_name() << " " << component->full_name());
	children_.push_back(component);
	component->set_parent(this);
	for (size_t i = 0; i < types.size(); ++i)
		subscribe(types[i], component.get());
	on_attach(component.get());
}

void Component::attach(component_ptr component, int type)
{
	DEBUG_LOG("Component::attach() " << full_name() << " " << component->full_name());
	children_.push_back(component);
	component->set_parent(this);
	subscribe(type, component.get());
	on_attach(component.get());
}

void Component::detach(component_ptr component)
{
	detach(component.get());
}

void Component::detach(Component* component)
{
	DEBUG_LOG("Component::detach():" << full_name() << " " << component->full_name());
	on_detach(component);
	// remove component from subscribers
	for (subsmap::iterator it = subscribers_.begin(); it != subscribers_.end(); ++it)
	{
		std::vector<Component*>& childs = it->second;
		for (std::vector<Component*>::iterator vit = childs.begin(); vit != childs.end(); ++vit)
		{
			if (*vit == component)
			{
				DEBUG_LOG("Component removed from subscribers:" << full_name() << " " << component->full_name());
				childs.erase(vit);
				break;
			}
		}
	}
	// remove component from childs
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
	if (root_ != nullptr)
		root_->update(message);
}

component_ptr Component::child_by_add_name(const std::string& add_name) const
{
	for (size_t i = 0; i < children_.size(); ++i)
	{
		if (children_[i]->add_name() == add_name) return children_[i];
	}
	return component_ptr();
}

}}	// namespaces
