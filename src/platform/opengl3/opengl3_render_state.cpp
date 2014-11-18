#include "platform/opengl3/opengl3_render_state.hpp"
#include "platform/opengl3/opengl3_driver.hpp"

#include "platform/opengl/opengl_types.hpp"
#include "platform/opengl/opengl_extension.hpp"

namespace mhe {
namespace opengl {

void DepthState::init(const DepthDesc& desc)
{
	desc_.enabled = desc.enabled;
}

void DepthState::enable(OpenGL3ContextState& state) const
{
    if (desc_.enabled == state.depth) return;
	if (desc_.enabled)
		glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
    state.depth = desc_.enabled;
}

void StencilState::init(const StencilDesc& desc)
{
	desc_.enabled = desc.enabled;
	desc_.front_op_pass = get_stencil_op(desc.front_operation_pass);
	desc_.front_op_dfail = get_stencil_op(desc.front_operation_dfail);
	desc_.front_op_sfail = get_stencil_op(desc.front_operation_sfail);
	desc_.front_compare_mode = get_compare_mode(desc.front_compare_func);
	desc_.front_ref = desc.front_reference;
	desc_.front_mask = desc_.front_mask;
}

void StencilState::enable(OpenGL3ContextState& state) const
{
    if (state.stencil == desc_.enabled) return;
	if (desc_.enabled)
	{
		glEnable(GL_STENCIL_TEST);
		OpenGLExtensions::instance().glStencilFuncSeparate(GL_FRONT, desc_.front_compare_mode, desc_.front_ref, desc_.front_mask);
		OpenGLExtensions::instance().glStencilOpSeparate(GL_FRONT, desc_.front_op_sfail, desc_.front_op_dfail, desc_.front_op_pass);
	}
	else glDisable(GL_STENCIL_TEST);
    state.stencil = desc_.enabled;
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

void BlendState::enable(OpenGL3ContextState& state) const
{
    if (state.blend == desc_.enabled) return;
	if (desc_.enabled)
	{
		glEnable(GL_BLEND);
		OpenGLExtensions::instance().glBlendFuncSeparate(desc_.srcmode, desc_.dstmode, desc_.srcmode_alpha, desc_.dstmode_alpha);
		OpenGLExtensions::instance().glBlendEquation(desc_.func);
	}
	else glDisable(GL_BLEND);
    state.blend = desc_.enabled;
}

bool OpenGL3RenderState::init(const RenderStateDesc& desc)
{
	depth_state_.init(desc.depth);
	stencil_state_.init(desc.stencil);
	blend_state_.init(desc.blend);
	return true;
}

void OpenGL3RenderState::close()
{}

void OpenGL3RenderState::update(const RenderStateDesc& desc)
{
	init(desc);
}

void OpenGL3RenderState::enable(OpenGL3ContextState& state) const
{
    depth_state_.enable(state);
    stencil_state_.enable(state);
    blend_state_.enable(state);
}

void OpenGL3RenderState::disable() const
{}

}}
