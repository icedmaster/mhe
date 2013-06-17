#include "platform/opengl/opengl_multitexture.hpp"

#include "platform/opengl/opengl_driver.hpp"
#include "platform/opengl/opengl_extension.hpp"

namespace mhe {
namespace opengl {

void OpenGLMultiTexture::set_image(boost::shared_ptr<Image> image, FilterType ft)
{
	boost::shared_ptr<OpenGLTexture> texture(new OpenGLTexture);
	texture->set_image(image, ft);
	textures_.push_back(texture);
}

void OpenGLMultiTexture::prepare(Driver* driver)
{
	for (size_t i = 0; i < textures_.size(); ++i)
	{
		OpenGLExtensions::instance().glActiveTexture(GL_TEXTURE0_ARB + i);
		textures_[i]->prepare(driver);
	}
}

void OpenGLMultiTexture::clean(Driver* /*driver*/)
{
	for (size_t i = 0; i < textures_.size(); ++i)
	{
		OpenGLExtensions::instance().glActiveTexture(GL_TEXTURE0_ARB + i);
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
