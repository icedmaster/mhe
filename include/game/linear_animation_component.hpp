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
	virtual LinearAnimationComponent() {}

	void set_range(const T& from, const T& to)
	{
		from_ = from;
		to_ = to;
	}

	T current_value() const
	{
		return lerp(from_, to_, Animation::current_value());
	}
private:
	void update_impl(float value)
	{
		const T& result = current_value();
		send_message(result);
	}

	virtual void send_message(const T& value) = 0;
private:
	T from_;
	T to_;
};

}}

#endif
