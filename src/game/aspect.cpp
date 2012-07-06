#include "game/aspect.hpp"
#include "utils/logutils.hpp"

namespace mhe {
namespace game {

void Aspect::attach(aspect_ptr aspect)
{
	DEBUG_LOG("Aspect::attach() " << full_name() << " " << aspect->full_name());
	children_.push_back(aspect);
	aspect->set_parent(this);
	aspect->do_subscribe(this);
}

void Aspect::attach(aspect_ptr aspect, const std::vector<int>& types)
{
	DEBUG_LOG("Aspect::attach() " << full_name() << " " << aspect->full_name());
	children_.push_back(aspect);
	aspect->set_parent(this);
	for (size_t i = 0; i < types.size(); ++i)
		subscribe(types[i], aspect.get());
}

void Aspect::attach(aspect_ptr aspect, int type)
{
	DEBUG_LOG("Aspect::attach() " << full_name() << " " << aspect->full_name());
	children_.push_back(aspect);
	aspect->set_parent(this);
	subscribe(type, aspect.get());
}

void Aspect::detach(Aspect* aspect)
{
	DEBUG_LOG("Aspect::detach():" << full_name() << " " << aspect->full_name());
	// remove aspect from subscribers
	for (subsmap::iterator it = subscribers_.begin(); it != subscribers_.end(); ++it)
	{
		std::vector<Aspect*>& childs = it->second;
		for (std::vector<Aspect*>::iterator vit = childs.begin(); vit != childs.end(); ++vit)
		{
			if (*vit == aspect)
			{
				DEBUG_LOG("Aspect removed from subscribers:" << full_name() << " " << aspect->full_name());
				childs.erase(vit);
				break;
			}
		}
	}
	// remove aspect from childs
	for (std::vector<aspect_ptr>::iterator it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it).get() == aspect)
		{
			DEBUG_LOG("Aspect detached:" << full_name() << " " << aspect->full_name());
			children_.erase(it);
			break;
		}
	}
}

void Aspect::update(const Message& message)
{
	if (message.type() == destroy_event)
	{
		DEBUG_LOG("Aspect " << full_name() << " receive destroy event");
		destroy_impl();
		for (size_t i = 0; i < children_.size(); ++i)
		{
			children_[i]->update(message);
		}
	}
	else if (update_impl(message))
		update_children(message);
}

void Aspect::update_children(const Message& message)
{
	subsmap::iterator it = subscribers_.find(message.type());
	if (it == subscribers_.end()) return;
	for (size_t i = 0; i < it->second.size(); ++i)
		it->second[i]->update_impl(message);
}

void Aspect::subscribe(int type, Aspect* aspect)
{
	subscribers_[type].push_back(aspect);
}

}}	// namespaces
