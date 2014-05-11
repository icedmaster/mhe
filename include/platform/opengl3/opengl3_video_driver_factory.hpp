#ifndef __OPENGLES_VIDEO_DRIVER_FACTORY_HPP__
#define __OPENGLES_VIDEO_DRIVER_FACTORY_HPP__

#include "impl/abstract_video_driver_factory.hpp"
#include "core/types.hpp"
#include "opengl3_driver.hpp"

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
};

}}

#endif
