#include "multitexture.hpp"

namespace mhe {

void MultiTexture::setImage(const boost::shared_ptr<Image>& image, FilterType ft)
{
	boost::shared_ptr<Texture> texture(new Texture);
	texture->setImage(image, ft);
	textures_.push_back(texture);
}

void MultiTexture::prepare()
{
	for (size_t i = 0; i < textures_.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0_ARB + i);
		textures_[i]->prepare();
	}
}

void MultiTexture::clean()
{
	for (size_t i = 0; i < textures_.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0_ARB + i);
		textures_[i]->clean();
	}
}

boost::shared_ptr<iTexture> MultiTexture::clone() const
{
	boost::shared_ptr<MultiTexture> clone_texture(new MultiTexture);
	clone_texture->textures_ = textures_;
	return clone_texture;
}

}	// namespace
