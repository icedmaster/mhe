#include "render/render_buffer.hpp"

#include "impl/system_factory.hpp"

namespace mhe {

RenderBuffer::RenderBuffer() :
	impl_(SystemFactory::instance().create_render_buffer())
{}

Layout::Layout() :
	impl_(SystemFactory::instance().create_layout())
{}

}
