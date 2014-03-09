#ifndef __ANIMATION_COMPONENT_HPP__
#define __ANIMATION_COMPONENT_HPP__

#include "component.hpp"
#include "animation.hpp"

namespace mhe {
namespace game {

class AnimationComponent : public Component, public Animation
{	
public:
	virtual ~AnimationComponent() {}

	void set_detach_on_completion(bool enable)
	{
		detach_on_completion_ = enable;
	}
protected:
	AnimationComponent(const std::string& name) :
		Component(name), Animation(),
		detach_on_completion_(false)
	{}

	AnimationComponent(cmn::uint duration, const std::string& name) :
		Component(name), Animation(duration),
		detach_on_completion_(false)
	{}
private:
	virtual void do_subscribe(Component* /*parent*/) {}
	bool update_impl(const Message&)
	{
		return true;
	}

	void update_impl(cmn::uint tick, Engine*)
	{
		Animation::update(tick);
	}

	virtual void stop_impl()
	{
		if (detach_on_completion_)
			detach_self();
	}

	bool detach_on_completion_;
};

}}

#endif
