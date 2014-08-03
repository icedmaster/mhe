#include "platform/opengl3/opengl3_render_target.hpp"

#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl/opengl_types.hpp"

namespace mhe {
namespace opengl {

bool OpenGL3RenderTarget::init(const RenderTargetDesc& desc)
{
	target_ = get_framebuffer_format(desc.target);
	desc_ = desc;
	OpenGLExtensions::instance().glGenFramebuffers(1, &id_);
	OpenGLExtensions::instance().glBindFramebuffer(target_, id_);

	TextureDesc texture_desc;
	texture_desc.type = texture_2d;
	texture_desc.width = desc.width;
	texture_desc.height = desc.height;
	texture_desc.address_mode_s = texture_desc.address_mode_t = texture_clamp;
	for (size_t i = 0; i < desc.color_targets; ++i)
	{
		texture_desc.format = desc.color_format[i];
		texture_desc.datatype = desc.color_datatype[i];
		if (!rt_[i].init(texture_desc, nullptr, 0))
			return false;
		const OpenGL3Texture* texture = static_cast<const OpenGL3Texture*>(rt_[i].impl());
		OpenGLExtensions::instance().glFramebufferTexture(target_, GL_COLOR_ATTACHMENT0 + i, texture->id(), 0);
	}

	if (desc.use_depth || desc.use_stencil)
	{
		texture_desc.format = desc.depth_format;
		texture_desc.datatype = desc.depth_datatype;
		if (!ds_.init(texture_desc, nullptr, 0))
			return false;
		const OpenGL3Texture* texture = static_cast<const OpenGL3Texture*>(ds_.impl());
		GLenum attachment = desc.use_depth ? (desc.use_stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT) : GL_STENCIL_ATTACHMENT;
		OpenGLExtensions::instance().glFramebufferTexture(target_, attachment, texture->id(), 0);
	}

	return false;
}

void OpenGL3RenderTarget::close()
{
}

void OpenGL3RenderTarget::enable() const
{
}

void OpenGL3RenderTarget::disable() const
{
}

}}
