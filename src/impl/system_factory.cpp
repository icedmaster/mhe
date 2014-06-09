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

IndexBufferImpl* SystemFactory::create_index_buffer() const
{
    return video_driver_factory_.create_index_buffer();
}

LayoutImpl* SystemFactory::create_layout() const
{
	return video_driver_factory_.create_layout();
}

UniformBufferImpl* SystemFactory::create_uniform_buffer() const
{
	return video_driver_factory_.create_uniform_buffer();
}

ShaderProgramImpl* SystemFactory::create_shader_program() const
{
	return video_driver_factory_.create_shader_program();
}

TextureImpl* SystemFactory::create_texture() const
{
	return video_driver_factory_.create_texture();
}

WindowSystemImpl* SystemFactory::create_window_system() const
{
	return window_system_factory_.create_window_system();
}	

}  // mhe
