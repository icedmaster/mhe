#ifndef __OPENGL_TYPES_HPP__
#define __OPENGL_TYPES_HPP__

#include "render/render_buffer.hpp"

namespace mhe {
namespace opengl {

GLenum get_vbo_usage(BufferUpdateType type)
{
	switch (type)
	{
		case buffer_update_type_static:
			return GL_STATIC_DRAW;
		case buffer_update_type_dynamic:
			return GL_DYNAMIC_DRAW;
		default:
			ASSERT(0, "Invalid type " << type);
			return GL_STATIC_DRAW;
	}
}

}}

#endif
