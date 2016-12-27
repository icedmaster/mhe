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

RenderStateImpl* SystemFactory::create_render_state() const
{
    return video_driver_factory_.create_render_state();
}

RenderTargetImpl* SystemFactory::create_render_target() const
{
    return video_driver_factory_.create_render_target();
}

TextureBufferImpl* SystemFactory::create_texture_buffer() const
{
    return video_driver_factory_.create_texture_buffer();
}

ShaderStorageBufferImpl* SystemFactory::create_shader_storage_buffer() const
{
    return video_driver_factory_.create_shader_storage_buffer();
}

QueryImpl* SystemFactory::create_query() const
{
    return video_driver_factory_.create_query();
}

AtomicImpl* SystemFactory::create_atomic() const
{
    return video_driver_factory_.create_atomic();
}

WindowSystemImpl* SystemFactory::create_window_system() const
{
    return window_system_factory_.create_window_system();
}

}  // mhe
