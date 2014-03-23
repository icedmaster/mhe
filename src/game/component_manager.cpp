#include "game/component_manager.hpp"

#include "game/engine.hpp"
#include "utils/global_log.hpp"

namespace mhe {
namespace game {

void ComponentManager::add(const component_ptr& component)
{
	common_pool_.add(component);
}

void ComponentManager::add(Component* component)
{
	add(component_ptr(component));
}

void ComponentManager::update(cmn::uint tick, Engine* engine)
{
	common_pool_.update(tick, engine);
	for (size_t i = 0; i < pools_.size(); ++i)
		pools_[i]->update(tick, engine);
}

}}
