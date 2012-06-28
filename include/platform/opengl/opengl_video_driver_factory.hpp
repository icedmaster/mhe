#ifndef __OPENGL_VIDEO_DRIVER_FACTORY_HPP__
#define __OPENGL_VIDEO_DRIVER_FACTORY_HPP__

#include "impl/abstract_video_driver_factory.hpp"
#include "opengl_driver.hpp"
#include "opengl_multitexture.hpp"

namespace mhe {
namespace opengl {

class OpenGLVideoDriverFactory : public AbstractVideoDriverFactory
{
public:
	std::string name() const
	{
		return "opengl";
	}

	Driver* create_video_driver() const
	{
		return new OpenGLDriver;
	}

	Texture* create_texture() const 
	{
		return new OpenGLTexture;
	}

	Texture* create_multitexture() const
	{
		return new OpenGLMultiTexture;
	}
};

}}

#endif
