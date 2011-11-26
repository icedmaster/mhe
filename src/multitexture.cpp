#include "multitexture.hpp"
#include "opengl_driver.hpp"

namespace mhe {

void MultiTexture::setImage(const boost::shared_ptr<Image>& image,
							boost::shared_ptr<iDriver> driver, FilterType ft)
{
	boost::shared_ptr<Texture> texture(new Texture);
	texture->setImage(image, driver, ft);
	textures_.push_back(texture);
}

void MultiTexture::prepare(boost::shared_ptr<iDriver> driver)
{
	for (size_t i = 0; i < textures_.size(); ++i)
	{
		dynamic_cast<OpenGLDriver*>(driver.get())->get_extensions().glActiveTexture(GL_TEXTURE0_ARB + i);
		textures_[i]->prepare(driver);
	}
}

void MultiTexture::clean(boost::shared_ptr<iDriver> driver)
{
	for (size_t i = 0; i < textures_.size(); ++i)
	{
		dynamic_cast<OpenGLDriver*>(driver.get())->get_extensions().glActiveTexture(GL_TEXTURE0_ARB + i);
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
