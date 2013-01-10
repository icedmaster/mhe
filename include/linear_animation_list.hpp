#ifndef __LINEAR_ANIMATION_LIST_HPP__
#define __LINEAR_ANIMATION_LIST_HPP__

#include "animation_list_base.hpp"

namespace mhe {

template <class T>
class LinearAnimationList : public AnimationListBase
{
public:
	LinearAnimationList(size_t index, bool repeat = false) :
		AnimationListBase(index, repeat),
		current_animation_(0),
		get_from_value_(true)
	{}

	void set_range(const T& from, const T& to,
				   cmn::uint animation_part_time, cmn::uint total_animation_time)
	{
		from_ = from;
		get_from_value_ = false;
		init_animation_parameters(to, animation_part_time, total_animation_time);
		init_animation_delta();
	}

	void set_to_value(const T& to,
					  cmn::uint animation_part_time, cmn::uint total_animation_time)
	{
		init_animation_parameters(to, animation_part_time, total_animation_time);
	}

	void set_animation(const T& delta, cmn::uint animation_part_time,
					   cmn::uint animations_count)
	{
		delta_ = delta;
		animation_part_time_ = animation_part_time;
		animations_count_ = animations_count;
		get_from_value_ = false;
	}

	T current_value() const
	{
		return current_;
	}
private:
	size_t frames_number_impl() const
	{
		return animations_count_;
	}

	size_t current_frame_duration_impl() const
	{
		return animation_part_time_;
	}

	void start_impl()
	{
		current_ = from_;
		current_animation_ = 0;
	}

	bool next()
	{
		if (++current_animation_ > animations_count_)
			return false;
		return true;
	}

	/// If node is nullptr - animation list does only update current value
	void update_node_impl(Node* node)
	{
		if (!get_from_value_)
		{
			update_current_value(current_, delta_);
			if (node != nullptr)
				update_node_for_value(node, current_);
			get_from_value_ = false;
		}
		else
		{
			if (node != nullptr)
				from_ = get_current_node_value(node);
			init_animation_delta();
		}
	}
protected:
	void set_current_value(const T& value)
	{
		current_ = value;
	}

	void set_delta(const T& delta)
	{
		delta_ = delta;
	}
private:
	virtual void update_node_for_value(Node* node, const T& value) = 0;
	virtual T get_current_node_value(Node* node) const = 0;
	virtual void update_current_value(T& current, const T& delta) const
	{
		current += delta;
	}
private:
	void init_animation_parameters(const T& to,
								   cmn::uint animation_part_time, cmn::uint total_animation_time)
	{
		to_ = to;
		animation_part_time_ = animation_part_time;
		animations_count_ = total_animation_time / animation_part_time;
	}

	void init_animation_delta()
	{
		delta_ = (to_ - from_) / animations_count_;
		current_ = from_;
	}

	T from_;
	T to_;
	T current_;	
	T delta_;
	cmn::uint animations_count_;
	cmn::uint animation_part_time_;
	cmn::uint current_animation_;
	bool get_from_value_;
};

}

#endif
