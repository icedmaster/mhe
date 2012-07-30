#include "animation_list.hpp"

namespace mhe {

AnimationList::AnimationList(bool repeat) :
	autorepeat_(repeat),
	next_tick_(0),
	cur_animation_(0),
	index_(0)
{
}

void AnimationList::reset()
{
	stop();
}

void AnimationList::add(const Animation& a)
{
	animations_.push_back(a);
	size_t sz = animations_.size();
	if ( !a.texture() && (sz > 1) )
		animations_[sz - 1].set_texture(animations_[sz - 2].texture());			
}

void AnimationList::remove(cmn::uint begin, cmn::uint number)
{
	if (next_tick_) return; // need to stop animation before removing
	if (animations_.size() <= begin)
		return; // invalid index
	std::vector<Animation>::iterator first = animations_.begin() + begin;
	std::vector<Animation>::iterator last = (animations_.size() >= (begin + number)) ?
		first + number :
		animations_.end();
	animations_.erase(first, last);
}

void AnimationList::start(cmn::uint tick)
{
	// nothing to animate
	if (animations_.empty()) return;
	next_tick_ = tick + animations_[cur_animation_].duration();
}

void AnimationList::stop()
{
	next_tick_ = 0;
	cur_animation_ = 0;
}

bool AnimationList::next()
{
	if (++cur_animation_ >= animations_.size())
	{
		if (autorepeat_)
			cur_animation_ = 0;
		else
		{
			stop();
			return false;
		}
	}
	if (next_tick_)
		next_tick_ += animations_[cur_animation_].duration();
	return true;
}

bool AnimationList::get(Animation& a) const
{
	if (!next_tick_)    // stopped
		return false;
	a = animations_[cur_animation_];
	return true;
}

bool AnimationList::get(cmn::uint tick, Animation& a)
{
	if (!next_tick_)    // stopped
		return false;
	update(tick);
	a = animations_[cur_animation_];
	return true;
}

AnimationList::AnimationChangeType AnimationList::update(cmn::uint tick)
{
	if ( next_tick_ && (tick >= next_tick_) )
	{
		if (!next())
		{
			delegates_(this, last_animation);
			return last_animation;
		}
		if (!cur_animation_)
		{
			delegates_(this, first_animation);
			return first_animation;
		}
		delegates_(this, next_animation);
		return next_animation;
	}	
	return no_change;
}

}
