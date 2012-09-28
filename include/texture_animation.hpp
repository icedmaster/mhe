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

	void set_texcoord(const std::vector<float>& coord)
	{
		texcoord_ = coord;
	}
private:
	void update_impl(Node* node)
	{
		node->set_texture(texture_);
		if (!texcoord_.empty())
			node->set_texcoord(texcoord_);
	}

	boost::shared_ptr<Texture> texture_;
	std::vector<float> texcoord_;
};

}

#endif
