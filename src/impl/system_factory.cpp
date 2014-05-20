#include "impl/system_factory.hpp"
#include "platform/platform.hpp"

namespace mhe {

DriverImpl* SystemFactory::create_driver() const
{
	return video_driver_factory_.create_video_driver();
}

RenderBufferImpl* SystemFactory::create_render_buffer() const
{
	return video_driver_factory_.create_render_buffer();
}

LayoutImpl* SystemFactory::create_layout() const
{
	return video_driver_factory_.create_layout();
}

/*
Texture* SystemFactory::create_texture() const
{
	return video_driver_factory_.create_texture();
}
*/

WindowSystemImpl* SystemFactory::create_window_system() const
{
	return window_system_factory_.create_window_system();
}	

}  // mhe
