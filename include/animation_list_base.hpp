#ifndef __ANIMATION_LIST_BASE_HPP__
#define __ANIMATION_LIST_BASE_HPP__

#include "delegate.hpp"
#include "types.hpp"
#include "node.hpp"

namespace mhe {

class AnimationListBase
{
public:
	enum AnimationChangeType
	{
		first_animation,
		next_animation,
		last_animation,
		no_change
	};

	typedef DelegateFunc2<void, AnimationListBase*, AnimationChangeType> AnimationListDelegate;
public:
	AnimationListBase(size_t index, bool repeat = false) :
		index_(index),
		repeat_(repeat),
		next_tick_(0)
	{}

	virtual ~AnimationListBase() {}

	size_t index() const
	{
		return index_;
	}

	void start(cmn::uint tick);
	void stop();

	AnimationChangeType update(cmn::uint tick);

	bool is_running() const
	{
		return next_tick_;
	}

	size_t frames_number() const
	{
		return frames_number_impl();
	}

	size_t current_frame_duration() const
	{
		return current_frame_duration_impl();
	}

	void add_delegate(AnimationListDelegate* delegate)
	{
		delegates_ += delegate;
	}

	void update_node(Node* node)
	{
		update_node_impl(node);
	}
private:
	virtual void start_impl() {}
	virtual void stop_impl() {}
	virtual size_t frames_number_impl() const = 0;
	virtual size_t current_frame_duration_impl() const = 0;
	virtual bool next() = 0;
	virtual void update_node_impl(Node* node) = 0;
private:
	size_t index_;
	bool repeat_;
	cmn::uint next_tick_;

	typedef Delegate2<void, AnimationListBase*, AnimationChangeType> delegate_type;
	delegate_type delegates_;
};

}

#endif
