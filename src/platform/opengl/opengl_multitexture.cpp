#include "platform/opengl/opengl_multitexture.hpp"
#include "platform/opengl/opengl_driver.hpp"

namespace mhe {
namespace opengl {

void OpenGLMultiTexture::set_image(boost::shared_ptr<Image> image,
								   boost::shared_ptr<Driver> driver, FilterType ft)
{
	boost::shared_ptr<OpenGLTexture> texture(new OpenGLTexture);
	texture->set_image(image, driver, ft);
	textures_.push_back(texture);
}

void OpenGLMultiTexture::prepare(boost::shared_ptr<Driver> driver)
{
	for (size_t i = 0; i < textures_.size(); ++i)
	{
		dynamic_cast<OpenGLDriver*>(driver.get())->get_extensions().glActiveTexture(GL_TEXTURE0_ARB + i);
		textures_[i]->prepare(driver);
	}
}

void OpenGLMultiTexture::clean(boost::shared_ptr<Driver> driver)
{
	for (size_t i = 0; i < textures_.size(); ++i)
	{
		dynamic_cast<OpenGLDriver*>(driver.get())->get_extensions().glActiveTexture(GL_TEXTURE0_ARB + i);
		textures_[i]->clean();
	}
}

boost::shared_ptr<Texture> OpenGLMultiTexture::clone() const
{
	boost::shared_ptr<OpenGLMultiTexture> clone_texture(new OpenGLMultiTexture);
	clone_texture->textures_ = textures_;
	return clone_texture;
}

}}	// namespace
