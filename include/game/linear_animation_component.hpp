#ifndef __LINEAR_ANIMATION_COMPONENT_HPP__
#define __LINEAR_ANIMATION_COMPONENT_HPP__

#include "animation_component.hpp"
#include "math/math_utils.hpp"

namespace mhe {
namespace game {

template <class T>
class LinearAnimationComponent : public AnimationComponent
{
public:
	LinearAnimationComponent(cmn::uint duration, const std::string& name, const std::string& add_name) :
		AnimationComponent(duration ,name, add_name)
	{}

	virtual ~LinearAnimationComponent() {}

	void set_range(const T& from, const T& to)
	{
		from_ = from;
		to_ = to;
	}

	T current_value() const
	{
		return lerp(from_, to_, Animation::current_value());
	}

	T value(float v) const
	{
		return lerp(from_, to_, v);
	}
private:
	void update_animation_impl(float v)
	{
		const T& result = value(v);
		send_message(result);
	}

	virtual void send_message(const T& value) = 0;
private:
	T from_;
	T to_;
};

}}

#endif
