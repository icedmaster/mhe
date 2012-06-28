#ifndef __VIDEO_DRIVERS_FACTORY_HPP__
#define __VIDEO_DRIVERS_FACTORY_HPP__

#include <vector>
#include <boost/shared_ptr.hpp>
#include "abstract_video_driver_factory.hpp"

namespace mhe {

class Driver;
class Texture;
class MultiTexture;

class VideoDriverFactory
{
public:
	VideoDriverFactory();

	std::vector<std::string> available_drivers_list() const;
	std::string current_driver_name() const;

	Driver* create_video_driver() const;
	Texture* create_texture() const;
	Texture* create_multitexture() const;
private:
	typedef std::vector< boost::shared_ptr<AbstractVideoDriverFactory> > drvvec;
	drvvec drivers_;
	boost::shared_ptr<AbstractVideoDriverFactory> current_driver_factory_;
};

}

#endif
