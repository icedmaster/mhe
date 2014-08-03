#ifndef __OPENGL3_RENDER_STATE_HPP__
#define __OPENGL3_RENDER_STATE_HPP__

#include "render/render_state.hpp"

namespace mhe {
namespace opengl {

class DepthState
{
public:
	void init(const DepthDesc& desc);
	void enable() const;
private:
	struct Desc
	{
		bool enabled;
	};
	Desc desc_;
};

class OpenGL3RenderState : public RenderStateImpl
{
public:
	bool init(const RenderStateDesc& desc);
	void close();

	void enable() const;
	void disable() const;
private:
	DepthState depth_state_;
};

}}

#endif
