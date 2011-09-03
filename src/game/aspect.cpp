#include "game/aspect.hpp"

namespace mhe {
namespace game {

void Aspect::add(Aspect* aspect)
{
	childs_.push_back(aspect);
	aspect->set_parent(this);
}

void Aspect::update_impl(int aspect_event, cmn::uint tick, const AspectParam& prm)
{
	for (size_t i = 0; i < childs_.size(); ++i)
		childs_[i]->update(aspect_event, tick, prm);
}

}}	// namespaces
