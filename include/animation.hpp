#ifndef __ANIMATION_HPP__
#define __ANIMATION_HPP__

#include "texture.hpp"

namespace mhe {

class Node;

class Animation
{
public:
	Animation(cmn::uint animation_time) :
		atime_(animation_time)
	{}

	virtual ~Animation() {}

	cmn::uint duration() const
	{
		return atime_;
	}

	void update(Node* node)
	{
		update_impl(node);
	}
private:
	virtual void update_impl(Node* node) = 0;

	cmn::uint atime_;    // time of playing current animation
};

}

#endif
