#ifndef __TRANSFORM_ANIMATION_HPP__
#define __TRANSFORM_ANIMATION_HPP__

#include "animation.hpp"
#include "node.hpp"

namespace mhe {

class TransformAnimation : public Transform, public Animation
{
private:
	void update_impl(Node* node)
	{
		node->set_transform(get_transform() * node->get_transform());
	}
};

}

#endif
