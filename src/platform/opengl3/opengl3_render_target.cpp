#include "platform/opengl3/opengl3_render_target.hpp"

#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl/opengl_types.hpp"

#include "utils/global_log.hpp"

namespace mhe {
namespace opengl {

bool OpenGL3RenderTarget::init(const RenderTargetDesc& desc, Texture** color_textures, Texture* depth_texture)
{
	target_ = get_framebuffer_format(desc.target);
	targets_number_ = desc.color_targets;
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
		if (!color_textures[i]->init(texture_desc, nullptr, 0))
			return false;
		const OpenGL3Texture* texture = static_cast<const OpenGL3Texture*>(color_textures[i]->impl());
		OpenGLExtensions::instance().glFramebufferTexture(target_, GL_COLOR_ATTACHMENT0 + i, texture->id(), 0);
		CHECK_GL_ERRORS();
	}

	if (depth_texture != nullptr)
	{
		texture_desc.format = desc.depth_format;
		texture_desc.datatype = desc.depth_datatype;
		if (!depth_texture->init(texture_desc, nullptr, 0))
			return false;
		const OpenGL3Texture* texture = static_cast<const OpenGL3Texture*>(depth_texture->impl());
		GLenum attachment = desc.use_depth ? (desc.use_stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT) : GL_STENCIL_ATTACHMENT;
		OpenGLExtensions::instance().glFramebufferTexture(target_, attachment, texture->id(), 0);
		CHECK_GL_ERRORS();
	}

	GLenum result = OpenGLExtensions::instance().glCheckFramebufferStatus(target_);
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		WARN_LOG("Framebuffer incomplete with status:" << result);
		return false;
	}

	OpenGLExtensions::instance().glBindFramebuffer(target_, 0);
	return true;
}

void OpenGL3RenderTarget::close()
{
}

void OpenGL3RenderTarget::enable() const
{
	OpenGLExtensions::instance().glBindFramebuffer(target_, id_);
	const GLenum draw_buffers[max_simultaneous_render_targets_number] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	OpenGLExtensions::instance().glDrawBuffers(targets_number_, draw_buffers);
}

void OpenGL3RenderTarget::disable() const
{
	OpenGLExtensions::instance().glBindFramebuffer(target_, 0);
}

}}
