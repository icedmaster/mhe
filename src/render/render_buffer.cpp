#include "render/render_buffer.hpp"

#include "impl/system_factory.hpp"

namespace mhe {

RenderBuffer::RenderBuffer() :
    impl_(SystemFactory::instance().create_render_buffer())
{}

RenderBuffer::~RenderBuffer()
{
    SystemFactory::instance().destroy_object(impl_);
}

IndexBuffer::IndexBuffer() :
    impl_(SystemFactory::instance().create_index_buffer())
{}

IndexBuffer::~IndexBuffer()
{
    SystemFactory::instance().destroy_object(impl_);
}

Layout::Layout() :
    impl_(SystemFactory::instance().create_layout())
{}

Layout::~Layout()
{
    SystemFactory::instance().destroy_object(impl_);
}

UniformBuffer::UniformBuffer() :
    impl_(SystemFactory::instance().create_uniform_buffer())
{}

UniformBuffer::~UniformBuffer()
{
    SystemFactory::instance().destroy_object(impl_);
}

TextureBuffer::TextureBuffer() :
    impl_(SystemFactory::instance().create_texture_buffer())
{
}

TextureBuffer::~TextureBuffer()
{
    SystemFactory::instance().destroy_object(impl_);
}

ShaderStorageBuffer::ShaderStorageBuffer() :
    impl_(SystemFactory::instance().create_shader_storage_buffer())
{}

ShaderStorageBuffer::~ShaderStorageBuffer()
{
    SystemFactory::instance().destroy_object(impl_);
}

}
