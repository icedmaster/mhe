#ifndef __ANIMATION_HPP__
#define __ANIMATION_HPP__

#include "delegate.hpp"
#include "transform.hpp"
#include "texture.hpp"

namespace mhe {

class Animation : public Transform
{
public:
	Animation() :
		atime_(0)
	{
	}

	Animation(cmn::uint animation_time) :
		atime_(animation_time)
	{}

	Animation(cmn::uint animation_time,
			  const boost::shared_ptr<Texture>& animation_texture) :
		atime_(animation_time),
		atexture_(animation_texture)
	{
	}

	cmn::uint duration() const
	{
		return atime_;
	}

	const boost::shared_ptr<Texture>& texture() const
	{
		return atexture_;
	}

	void set_texture(const boost::shared_ptr<Texture>& t)
	{
		atexture_ = t;
	}
private:
	cmn::uint atime_;    // time of playing current animation
	boost::shared_ptr<Texture> atexture_;
};

}

#endif
