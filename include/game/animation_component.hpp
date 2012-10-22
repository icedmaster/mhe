#ifndef __ANIMATION_COMPONENT_HPP__
#define __ANIMATION_COMPONENT_HPP__

#include "component.hpp"
#include "linear_animation_list_impl.hpp"

namespace mhe {
namespace game {

class AnimationComponent : public Component
{
protected:
	AnimationComponent(const std::string& name, const std::string& add_name,
		AnimationListBase* animation_list) :
		Component(name, add_name),
		animation_list_(animation_list)
	{}

	virtual ~AnimationComponent() {}

	template <class T>
	const T* get_animation_list() const
	{
		return static_cast<const T*>(animation_list_.get());
	}
private:
	void do_subscribe(Component* /*parent*/) {}
	bool update_impl(const Message&)
	{
		return true;
	}
	void update_impl(cmn::uint tick);

	virtual void send_animation_message() = 0;

	boost::shared_ptr<AnimationListBase> animation_list_;
};

class TransformAnimationComponent : public AnimationComponent
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
