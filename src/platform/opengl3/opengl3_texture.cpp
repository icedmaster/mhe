#include "platform/opengl3/opengl3_texture.hpp"

#include "platform/opengl/opengl_types.hpp"
#include "platform/opengl/opengl_extension.hpp"

namespace mhe {
namespace opengl {

bool OpenGL3Texture::init(const TextureDesc& desc, const uint8_t* data, size_t size)
{
	glGenTextures(1, &id_);
	if (id_ == 0) return false;
	target_ = get_texture_target(desc.type);
	glBindTexture(target_, id_);
	glTexParameteri(target_, GL_TEXTURE_WRAP_T, get_texture_address_mode(desc.address_mode_t));
	glTexParameteri(target_, GL_TEXTURE_WRAP_S, get_texture_address_mode(desc.address_mode_s));
	glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, get_texture_filter(desc.mag_filter));
	glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, get_texture_filter(desc.min_filter));

	if (desc.type == texture_cube)
	{
		init_cubemap(desc, data, size);
	}
	else
	{
		glTexImage2D(target_, 0,
			get_texture_format(desc.format), desc.width, desc.height, 0,
			get_texture_format(desc.format), get_texture_datatype(desc.datatype), data);
	}
	return true;
}

void OpenGL3Texture::init_cubemap(const TextureDesc& desc, const uint8_t* data, size_t size)
{
	size_t stride = desc.width * desc.height * get_bytes_per_format(get_texture_format(desc.datatype));
	ASSERT(size == stride * 6, "Invalid size");
	GLenum targets[6] = {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z};
	for (int i = 0; i < 6; ++i)
		glTexImage2D(targets[i], 0,
			get_texture_format(desc.format), desc.width, desc.height, 0,
			get_texture_format(desc.format), get_texture_datatype(desc.datatype), data + i * stride);
}

void OpenGL3Texture::close()
{
	glDeleteTextures(1, &id_);
}

void OpenGL3Texture::enable(size_t unit) const
{
	OpenGLExtensions::instance().glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(target_, id_);
	unit_ = unit;
}

void OpenGL3Texture::disable() const
{
	OpenGLExtensions::instance().glActiveTexture(GL_TEXTURE0 + unit_);
	glBindTexture(target_, 0);
}

}}
