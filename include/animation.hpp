#ifndef __ANIMATION_HPP__
#define __ANIMATION_HPP__

#include "utils/sysutils.hpp"

namespace mhe {

class Animation
{
public:
	
public:
	Animation() :
		atime_(0),
		loop_(false)
	{}

	Animation(cmn::uint duration) :
		atime_(duration)
	{}

	virtual ~Animation() {}

	void set_duration(cmn::uint duration)
	{
		atime_ = duration;
	}

	cmn::uint duration() const
	{
		return atime_;
	}

	void start()
	{
		running_ = true;
		start_ = utils::get_current_tick();
		start_impl();
	}

	void stop()
	{
		running_ = false;
		stop_impl();
	}

	bool running() const
	{
		return running_;
	}

	void set_loop(bool loop)
	{
		loop_ = loop;
	}

	void update(cmn::uint tick)
	{
		if (!running_) return;
		cmn::uint delta = tick - start_;
		if (delta > atime_)
		{
			if (!loop_)
			{
				stop();
				return;
			}
			else
			{
				start();
				delta = 0;
			}
		}
		update_animation_impl(static_cast<float>(delta) / atime_);
	}

	float current_value() const
	{
		if (!running_) return 0;
		cmn::uint now = utils::get_current_tick();
		return static_cast<float>(now - start_) / atime_;
	}
private:
	virtual void start_impl() {}
	virtual void stop_impl() {}
	virtual void update_animation_impl(float value) = 0;

	cmn::uint atime_;    // time of playing current animation
	cmn::uint start_;
	bool running_;
	bool loop_;
};

}

#endif
