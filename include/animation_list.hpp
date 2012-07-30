#ifndef __ANIMATION_LIST_HPP__
#define __ANIMATION_LIST_HPP__

#include <vector>
#include "animation.hpp"
#include "delegate.hpp"

namespace mhe {

class AnimationList
{
public:
	enum AnimationChangeType
	{
		first_animation,
		next_animation,
		last_animation,
		no_change
	};

	typedef DelegateFunc2<void, AnimationList*, AnimationChangeType> AnimationListDelegate;
public:
	AnimationList(bool repeat = false);

	void reset();
	void add(const Animation& a);
	void remove(cmn::uint begin, cmn::uint number);
	void start(cmn::uint tick);
	void stop();
	bool next();

	AnimationChangeType update(cmn::uint tick);

	bool get(Animation& a) const;
	bool get(cmn::uint tick, Animation& a);

	bool is_running() const
	{
		return next_tick_;
	}

	// index getter/setter
	void set_index(cmn::uint index)
	{
		index_ = index;
	}

	cmn::uint index() const
	{
		return index_;
	}

	size_t get_frames_number() const
	{
		return animations_.size();
	}

	void add_delegate(AnimationListDelegate* delegate)
	{
		delegates_ += delegate;
	}
private:
	std::vector<Animation> animations_;
	bool autorepeat_;
	cmn::uint next_tick_;
	size_t cur_animation_;
	cmn::uint index_;

	typedef Delegate2<void, AnimationList*, AnimationChangeType> delegate_type;
	delegate_type delegates_;
};

}

#endif
