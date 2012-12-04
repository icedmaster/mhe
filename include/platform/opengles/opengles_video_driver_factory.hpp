#ifndef __OPENGLES_VIDEO_DRIVER_FACTORY_HPP__
#define __OPENGLES_VIDEO_DRIVER_FACTORY_HPP__

#include "impl/abstract_video_driver_factory.hpp"
#include "types.hpp"
#include "opengles_driver.hpp"

namespace mhe {
namespace opengl {

class OpenGLESVideoDriverFactory : public AbstractVideoDriverFactory
{
public:
	std::string name() const
	{
		return "opengles";
	}

	Driver* create_video_driver() const
	{
		return new OpenGLESDriver;
	}

	Texture* create_texture() const 
	{
		return nullptr;
	}

	Texture* create_multitexture() const
	{
		return nullptr;
	}
};

}}

#endif
