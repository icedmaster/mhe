#ifndef __COMPONENT_MANAGER_HPP__
#define __COMPONENT_MANAGER_HPP__

#include "component.hpp"

namespace mhe {
namespace game {

class Engine;

class ComponentManager
{
public:
	void add(const component_ptr& component);
	component_ptr get(const std::string& full_name) const;

	void update(cmn::uint tick, Engine* engine);
private:
	bool check_for_component_delete(const component_ptr& component) const;
	void destroy_component(const component_ptr& component);

	typedef std::map<std::string, component_ptr> component_map;
	component_map components_;
};

}}

#endif
