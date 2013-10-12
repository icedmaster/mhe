#ifndef __OBSERVER_COMPONENT_HPP__
#define __OBSERVER_COMPONENT_HPP__

#include "component.hpp"

namespace mhe {
namespace game {

/**
 This component must be attached to component stored with ComponentManager,
 because it works into update(tick) method
*/
class ObserverComponent : public Component
{
public:
	ObserverComponent(const std::string& name) :
		Component(name)
	{}

	void add(const component_ptr& component)
	{
		components_.push_back(component_weak_ptr(component));
	}

	size_t observed_count() const
	{
		return components_.size();
	}
private:
	std::string add_name_impl() const
	{
		return "observer";
	}

	void do_subscribe(Component* /*parent*/)
	{}

	bool update_impl(const Message&)
	{
		return true;
	}

	void update_impl(cmn::uint tick, Engine* engine);
private:
	std::vector<component_weak_ptr> components_;
};

}}

#endif
