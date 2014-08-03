#include "platform/opengl3/opengl3_render_state.hpp"

#include "platform/opengl/mhe_gl.hpp"

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

bool OpenGL3RenderState::init(const RenderStateDesc& desc)
{
	depth_state_.init(desc.depth);
	return true;
}

void OpenGL3RenderState::close()
{}

void OpenGL3RenderState::enable() const
{
	depth_state_.enable();
}

void OpenGL3RenderState::disable() const
{}

}}
