#ifndef __ANIMATION_COMPONENT_HPP__
#define __ANIMATION_COMPONENT_HPP__

#include "component.hpp"
#include "linear_animation_list_impl.hpp"
#include "utils/sysutils.hpp"

namespace mhe {
namespace game {

template <class AnimationListClass>
class AnimationComponent : public Component
{	
public:
	AnimationListClass* animation_list()
	{
		return animation_list_.get();
	}

	const AnimationListClass* animation_list() const
	{
		return animation_list_.get();
	}

	void start()
	{
		animation_list_->start(utils::get_current_tick());
	}

	void stop()
	{
		animation_list_->stop();
	}

	void set_detach_on_completion(bool enable)
	{
		detach_on_completion_ = enable;
	}
protected:
	AnimationComponent(const std::string& name, const std::string& add_name,
		AnimationListClass* animation_list) :
		Component(name, add_name),
		animation_list_(animation_list),
		detach_on_completion_(false)
	{}

	virtual ~AnimationComponent() {}
private:
	void do_subscribe(Component* /*parent*/) {}
	bool update_impl(const Message&)
	{
		return true;
	}

	void update_impl(cmn::uint tick)
	{
		switch (animation_list_->update(tick))
		{
			case AnimationListBase::next_animation:
				send_animation_message();
				break;

			case AnimationListBase::last_animation:
				if (detach_on_completion_)
					detach_self();
				break;

			default: break;
		}
	}

	virtual void send_animation_message() = 0;

	boost::shared_ptr<AnimationListClass> animation_list_;
	bool detach_on_completion_;
};

class TransformAnimationComponent : public AnimationComponent<TransformLinearAnimationList>
{
public:
	TransformAnimationComponent(const std::string& name, const std::string& add_name) :
		AnimationComponent(name, add_name, new TransformLinearAnimationList(0))
	{}
private:
	void send_animation_message();
};

}}

#endif
