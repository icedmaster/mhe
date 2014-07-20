#ifndef __OPENGL3_RENDER_STATE_HPP__
#define __OPENGL3_RENDER_STATE_HPP__

#include "render/render_state.hpp"

namespace mhe {
namespace opengl {

class OpenGL3RenderState : public RenderStateImpl
{
public:
	bool init(const RenderStateDesc& desc);
	void close();
};

}}

#endif
