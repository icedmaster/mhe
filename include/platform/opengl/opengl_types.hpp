#ifndef __OPENGL_TYPES_HPP__
#define __OPENGL_TYPES_HPP__

#include "render/render_buffer.hpp"
#include "render/mesh.hpp"
#include "render/texture.hpp"

namespace mhe {
namespace opengl {

inline GLenum get_vbo_usage(BufferUpdateType type)
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

inline GLenum get_primitive_type(Primitive primitive)
{
	ASSERT(primitive < 2, "Invalid primitive:" << primitive);
	GLenum types[2] = {GL_TRIANGLES, GL_LINES};

	return types[primitive];
}

inline GLenum get_texture_target(int type)
{
	ASSERT(type < 2, "Invalid texture type");
	GLenum targets[2] = {GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP};
	return targets[type];
}

inline GLenum get_texture_filter(int filter)
{
	ASSERT(filter < 1, "Invalid texture filter");
	GLenum filters[1] = {GL_LINEAR};
	return filters[filter];
}

inline GLenum get_texture_address_mode(int mode)
{
	ASSERT(mode < 3, "Invalid texture address mode");
	GLenum modes[3] = {GL_REPEAT, GL_CLAMP, GL_CLAMP_TO_EDGE};
	return modes[mode];
}

inline GLenum get_texture_format(int format)
{
	ASSERT(format < 2, "Invalid texture format");
	GLenum formats[2] = {GL_RGBA, GL_BGRA};
	return formats[format];
}

inline GLenum get_texture_datatype(int type)
{
	ASSERT(type < 1, "Invalid texture datatype");
	GLenum types[1] = {GL_UNSIGNED_BYTE};
	return types[type];
}

inline GLenum get_framebuffer_format(int format)
{
    ASSERT(format < 3, "Invalid framebuffer format");
	GLenum formats[3] = {GL_READ_FRAMEBUFFER, GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER};
	return formats[format];
}

inline size_t get_bytes_per_format(GLenum format)
{
	switch (format)
	{
	case GL_RGBA:
		return 4;
	default:
		ASSERT(false, "Invalid format:" << format);
		break;
	}
	return 0;
}

}}

#endif
