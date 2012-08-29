#ifndef __TEXTURE_ANIMATION_HPP__
#define __TEXTURE_ANIMATION_HPP__

#include "animation.hpp"
#include "node.hpp"

namespace mhe {

class TextureAnimation : public Animation
{
public:
	TextureAnimation(cmn::uint duration, const boost::shared_ptr<Texture>& texture) :
		Animation(duration),
		texture_(texture)
	{}
private:
	void update_impl(Node* node)
	{
		node->set_texture(texture_);
	}

	boost::shared_ptr<Texture> texture_;
};

}

#endif
