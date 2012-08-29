#include "animation_list_base.hpp"

namespace mhe {

void AnimationListBase::start(cmn::uint tick)
{
	if (next_tick_) return;	// animation already running
	next_tick_ = tick + current_frame_duration_impl();
	start_impl();
}

void AnimationListBase::stop()
{
	next_tick_ = 0;
	stop_impl();
}

AnimationListBase::AnimationChangeType AnimationListBase::update(cmn::uint tick)
{
	if ( next_tick_ && (tick >= next_tick_) )
	{
		if (!next())
		{
			if (!repeat_)
			{
				delegates_(this, last_animation);
				return last_animation;
			}
			start_impl();	// restart animation
		}
		next_tick_ = tick + current_frame_duration_impl();
		delegates_(this, next_animation);
		return next_animation;
	}	
	return no_change;
}

}
