#include "game/aspect.hpp"

namespace mhe {
namespace game {

void Aspect::attach(aspect_ptr aspect)
{
	childs_.push_back(aspect_weak_ptr(aspect));
	aspect->set_parent(this);
	aspect->do_subscribe(this);
}

void Aspect::attach(aspect_ptr aspect, const std::vector<int>& types)
{
	childs_.push_back(aspect_weak_ptr(aspect));
	aspect->set_parent(this);
	for (size_t i = 0; i < types.size(); ++i)
		subscribe(types[i], aspect.get());
}

void Aspect::attach(aspect_ptr aspect, int type)
{
	childs_.push_back(aspect_weak_ptr(aspect));
	aspect->set_parent(this);
	subscribe(type, aspect.get());
}

void Aspect::update(int type, const void* prm)
{
	if (type == destroy_event)
	{
		destroy_impl();
		update_childs(destroy_event, nullptr);
	}
	else if (update_impl(type, prm))
		update_childs(type, prm);
}

void Aspect::update_childs(int type, const void* prm)
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
