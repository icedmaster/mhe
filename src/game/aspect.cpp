#include "game/aspect.hpp"
#include "utils/logutils.hpp"

namespace mhe {
namespace game {

void Aspect::attach(aspect_ptr aspect)
{
	DEBUG_LOG("Aspect::attach() " << full_name() << " " << aspect->full_name());
	childs_.push_back(aspect_weak_ptr(aspect));
	aspect->set_parent(this);
	aspect->do_subscribe(this);
}

void Aspect::attach(aspect_ptr aspect, const std::vector<int>& types)
{
	DEBUG_LOG("Aspect::attach() " << full_name() << " " << aspect->full_name());
	childs_.push_back(aspect_weak_ptr(aspect));
	aspect->set_parent(this);
	for (size_t i = 0; i < types.size(); ++i)
		subscribe(types[i], aspect.get());
}

void Aspect::attach(aspect_ptr aspect, int type)
{
	DEBUG_LOG("Aspect::attach() " << full_name() << " " << aspect->full_name());
	childs_.push_back(aspect_weak_ptr(aspect));
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
	for (std::vector<aspect_weak_ptr>::iterator it = childs_.begin(); it != childs_.end(); ++it)
	{
		if ((*it).lock().get() == aspect)
		{
			DEBUG_LOG("Aspect detached:" << full_name() << " " << aspect->full_name());
			childs_.erase(it);
			break;
		}
	}
}

void Aspect::update(int type, const void* prm)
{
	if (type == destroy_event)
	{
		DEBUG_LOG("Aspect " << full_name() << " receive destroy event");
		destroy_impl();
		for (size_t i = 0; i < childs_.size(); ++i)
		{
			if (childs_[i].expired())
				WARN_LOG("Aspect " << full_name() << " child " << i << " expired");
			childs_[i].lock()->update(destroy_event, nullptr);
		}
	}
	else if (update_impl(type, prm))
		update_children(type, prm);
}

void Aspect::update_children(int type, const void* prm)
{
	subsmap::iterator it = subscribers_.find(type);
	if (it == subscribers_.end()) return;
	for (size_t i = 0; i < it->second.size(); ++i)
		it->second[i]->update_impl(type, prm);
}

void Aspect::subscribe(int type, Aspect* aspect)
{
	subscribers_[type].push_back(aspect);
}

}}	// namespaces
