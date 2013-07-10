#ifndef __OPENGLES_VIDEO_DRIVER_FACTORY_HPP__
#define __OPENGLES_VIDEO_DRIVER_FACTORY_HPP__

#include "impl/abstract_video_driver_factory.hpp"
#include "types.hpp"
#include "opengl3_driver.hpp"
#include "../opengl/opengl_texture.hpp"

namespace mhe {
namespace opengl {

class OpenGL3VideoDriverFactory : public AbstractVideoDriverFactory
{
public:
	std::string name() const
	{
		return "opengl3";
	}

	DriverImpl* create_video_driver() const
	{
		return new OpenGL3Driver;
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
