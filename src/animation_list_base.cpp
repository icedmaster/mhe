#include "animation_list_base.hpp"

#include "utils/global_log.hpp"
#include <iostream>

namespace mhe {

void AnimationListBase::start(cmn::uint tick)
{
	if (next_tick_) return;	// animation already running
	next_tick_ = tick + current_frame_duration_impl();
	DEBUG_LOG("AnimationList " << index_ << " start:" << tick  << " " << next_tick_);
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
				stop();
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
