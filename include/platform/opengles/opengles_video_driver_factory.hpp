#ifndef __OPENGLES_VIDEO_DRIVER_FACTORY_HPP__
#define __OPENGLES_VIDEO_DRIVER_FACTORY_HPP__

#include "impl/abstract_video_driver_factory.hpp"
#include "types.hpp"
#include "opengles_driver.hpp"
#include "../opengl/opengl_texture.hpp"

namespace mhe {
namespace opengl {

class OpenGLESVideoDriverFactory : public AbstractVideoDriverFactory
{
public:
	std::string name() const
	{
		return "opengles";
	}

	DriverImpl* create_video_driver() const
	{
		return new OpenGLESDriver;
	}

	Texture* create_texture() const 
	{
		return new OpenGLTexture;
	}

	Texture* create_multitexture() const
	{
		return nullptr;
	}
};

}}

#endif
