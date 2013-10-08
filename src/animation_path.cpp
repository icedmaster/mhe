#include "animation_path.hpp"

#include "utils/sysutils.hpp"

namespace mhe {

void AnimationPath::add(Animation* animation)
{
	animations_.push_back(animation_ptr(animation));
	total_duration_ += animation->duration();
}

AnimationPath::animation_ptr AnimationPath::get(size_t index) const
{
	assert(index < (animations_.size() - 1));
	return animations_[index];
}

AnimationPath::animation_ptr AnimationPath::get_normalized(float value) const
{
	cmn::uint total = 0;
	float begin = 0.0;
	for (size_t i = 0; i < animations_.size(); ++i)
	{
		total += animations_[i]->duration();
		float end = static_cast<float>(total) / total_duration_;
		if ( (value >= begin) && (value < end) )
			return animations_[i];
		begin = end;
	}
	return animation_ptr();
}

void AnimationPath::start()
{
	started_ = true;
	start_tick_ = utils::get_current_tick();
	current_ = 0;
}

void AnimationPath::stop()
{
	started_ = false;
}
	
void AnimationPath::update(cmn::uint tick)
{
	if (!started_) return;
	if ( (tick - start_tick_) > animations_[current_]->duration() )
	{
		if (++current_ == animations_.size()) // last
		{
			if (loop_) current_ = 0;
			else
			{
				stop();
				return;
			}
		}
		start_tick_ = tick;
	}
	animations_[current_]->update(tick);
}

}
