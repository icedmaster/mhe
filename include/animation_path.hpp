#ifndef __ANIMATION_PATH_HPP__
#define __ANIMATION_PATH_HPP__

#include <boost/shared_ptr.hpp>
#include "fixed_size_vector.hpp"
#include "animation.hpp"

namespace mhe {

class AnimationPath
{
public:
	typedef boost::shared_ptr<Animation> animation_ptr;
private:
	static const size_t default_animations_number = 8;
	typedef fixed_size_vector<animation_ptr, default_animations_number> animations_vector;
public:
	AnimationPath() :
		current_(0),
		total_duration_(0),
		start_tick_(0),
		started_(false),
		loop_(false)
	{}

	void add(Animation* animation);

	animation_ptr get(size_t index) const;
	animation_ptr get_normalized(float value) const;

	size_t size() const
	{
		return animations_.size();
	}

	void start();
	void stop();
	
	void update(cmn::uint tick);

	void set_loop(bool loop)
	{
		loop_ = loop;
	}
private:
	animations_vector animations_;
	size_t current_;
	cmn::uint total_duration_;
	cmn::uint start_tick_;
	bool started_;
	bool loop_;
};

}

#endif
