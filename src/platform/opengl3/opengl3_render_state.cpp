#include "platform/opengl3/opengl3_render_state.hpp"

#include "platform/opengl/opengl_types.hpp"
#include "platform/opengl/opengl_extension.hpp"

namespace mhe {
namespace opengl {

void DepthState::init(const DepthDesc& desc)
{
	desc_.enabled = desc.enabled;
}

void DepthState::enable() const
{
	if (desc_.enabled)
		glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

void BlendState::init(const BlendDesc& desc)
{
	desc_.enabled = desc.enabled;
	if (desc_.enabled)
	{
		desc_.func = get_blend_operation(desc.func);
		desc_.func_alpha = get_blend_operation(desc.func_alpha);
		desc_.srcmode = get_blend_mode(desc.srcmode);
		desc_.dstmode = get_blend_mode(desc.dstmode);
		desc_.srcmode_alpha = get_blend_mode(desc.srcmode_alpha);
		desc_.dstmode_alpha = get_blend_mode(desc.dstmode_alpha);
	}
}

void BlendState::enable() const
{
	if (desc_.enabled)
	{
		glEnable(GL_BLEND);
		OpenGLExtensions::instance().glBlendFuncSeparate(desc_.srcmode, desc_.dstmode, desc_.srcmode_alpha, desc_.dstmode_alpha);
		OpenGLExtensions::instance().glBlendEquation(desc_.func);
	}
	else glDisable(GL_BLEND);
}

bool OpenGL3RenderState::init(const RenderStateDesc& desc)
{
	depth_state_.init(desc.depth);
	blend_state_.init(desc.blend);
	return true;
}

void OpenGL3RenderState::close()
{}

void OpenGL3RenderState::update(const RenderStateDesc& desc)
{
	init(desc);
}

void OpenGL3RenderState::enable() const
{
	depth_state_.enable();
	blend_state_.enable();
}

void OpenGL3RenderState::disable() const
{}

}}
