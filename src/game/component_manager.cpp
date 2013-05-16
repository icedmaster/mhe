#include "game/component_manager.hpp"

#include "game/simple_messages.hpp"
#include "game/engine.hpp"
#include "utils/global_log.hpp"

namespace mhe {
namespace game {

void ComponentManager::add(const component_ptr& component)
{
	components_.insert(std::make_pair(component->full_name(), component));
}

component_ptr ComponentManager::get(const std::string& fullname) const
{
	component_map::const_iterator it = components_.find(fullname);
	if (it == components_.end()) return component_ptr();
	return it->second;
}

void ComponentManager::update(cmn::uint tick, Engine* engine)
{
	for (component_map::iterator it = components_.begin(); it != components_.end();)
	{
		if (check_for_component_delete(it->second))
		{
			DEBUG_LOG("destroy component:" << it->second->full_name());
			destroy_component(it->second);
			components_.erase(it++);
		}
		else
		{
			it->second->update(tick, engine);
			++it;
		}
	}
}

bool ComponentManager::check_for_component_delete(const component_ptr& component) const
{
	return (component.use_count() == 1);
}

void ComponentManager::destroy_component(const component_ptr& component)
{
	component->update(DestroyMessage());
	const std::vector<component_ptr>& children = component->children();
	for (size_t i = 0; i < children.size(); ++i)
		components_.erase(children[i]->full_name());
}

}}
