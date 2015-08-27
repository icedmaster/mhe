#include "render/render_buffer.hpp"

#include "impl/system_factory.hpp"

namespace mhe {

RenderBuffer::RenderBuffer() :
	impl_(SystemFactory::instance().create_render_buffer())
{}

IndexBuffer::IndexBuffer() :
	impl_(SystemFactory::instance().create_index_buffer())
{}

Layout::Layout() :
	impl_(SystemFactory::instance().create_layout())
{}

UniformBuffer::UniformBuffer() :
	impl_(SystemFactory::instance().create_uniform_buffer())
{}

TextureBuffer::TextureBuffer() :
	impl_(SystemFactory::instance().create_texture_buffer())
{}

}
