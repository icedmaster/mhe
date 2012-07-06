#include "game/aspect_manager.hpp"

#include "game/message_destroy.hpp"
#include "utils/global_log.hpp"

namespace mhe {
namespace game {

void AspectManager::add(const aspect_ptr& aspect)
{
	aspects_.insert(std::make_pair(aspect->full_name(), aspect));
}

aspect_ptr AspectManager::get(const std::string& fullname) const
{
	aspect_map::const_iterator it = aspects_.find(fullname);
	if (it == aspects_.end()) return aspect_ptr();
	return it->second;
}

void AspectManager::update(cmn::uint tick)
{
	for (aspect_map::iterator it = aspects_.begin(); it != aspects_.end();)
	{
		if (check_for_aspect_delete(it->second))
		{
			DEBUG_LOG("destroy aspect:" << it->second->full_name());
			destroy_aspect(it->second);
			aspects_.erase(it++);
		}
		else
		{
			it->second->update(tick);
			++it;
		}
	}
}

bool AspectManager::check_for_aspect_delete(const aspect_ptr& aspect) const
{
	return (aspect.use_count() == 1);
}

void AspectManager::destroy_aspect(const aspect_ptr& aspect)
{
	aspect->update(DestroyMessage());
	const std::vector<aspect_ptr>& children = aspect->children();
	for (size_t i = 0; i < children.size(); ++i)
		aspects_.erase(children[i]->full_name());
}

}}
