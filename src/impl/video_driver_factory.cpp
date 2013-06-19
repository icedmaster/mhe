#include "impl/video_driver_factory.hpp"

#include "platform/platform.hpp"

namespace mhe {

VideoDriverFactory::VideoDriverFactory()
{
	#ifdef MHE_OPENGL
	drivers_.push_back(boost::shared_ptr<AbstractVideoDriverFactory>(new opengl::OpenGLVideoDriverFactory));
	#endif

	#ifdef MHE_OPENGL3
	drivers_.push_back(boost::shared_ptr<AbstractVideoDriverFactory>(new opengl::OpenGL3VideoDriverFactory));
	#endif
    
#ifdef MHE_OPENGLES
    drivers_.push_back(boost::shared_ptr<AbstractVideoDriverFactory>(new opengl::OpenGLESVideoDriverFactory));
#endif
    if (!drivers_.empty())
        // select highest video driver
        current_driver_factory_ = drivers_.back();
}

std::vector<std::string> VideoDriverFactory::available_drivers_list() const
{
	std::vector<std::string> names;
	names.reserve(drivers_.size());
	for (size_t i = 0; i < drivers_.size(); ++i)
		names.push_back(drivers_[i]->name());
	return names;
}

std::string VideoDriverFactory::set_next_driver()
{
	drvvec::iterator it = std::find(drivers_.begin(), drivers_.end(), current_driver_factory_);
	if (it == drivers_.begin())
		return std::string();
	current_driver_factory_ = *(--it);
	return current_driver_factory_->name();
}

std::string VideoDriverFactory::current_driver_name() const
{
	return current_driver_factory_->name();
}

DriverImpl* VideoDriverFactory::create_video_driver() const
{
	return current_driver_factory_->create_video_driver();
}

Texture* VideoDriverFactory::create_texture() const
{
	return current_driver_factory_->create_texture();
}

Texture* VideoDriverFactory::create_multitexture() const
{
	return current_driver_factory_->create_multitexture();
}

}
