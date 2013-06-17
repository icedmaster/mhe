#ifndef __ABSTRACT_VIDEO_DRIVER_FACTORY_HPP__
#define __ABSTRACT_VIDEO_DRIVER_FACTORY_HPP__

#include <string>

namespace mhe {

class DriverImpl;
class Texture;

class AbstractVideoDriverFactory
{
public:
	virtual ~AbstractVideoDriverFactory() {}
	
	virtual std::string name() const = 0;

	virtual DriverImpl* create_video_driver() const = 0;
	virtual Texture* create_texture() const = 0;
	virtual Texture* create_multitexture() const = 0;
};

}

#endif
