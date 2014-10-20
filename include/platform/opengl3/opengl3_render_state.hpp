#ifndef __OPENGL3_RENDER_STATE_HPP__
#define __OPENGL3_RENDER_STATE_HPP__

#include "render/render_state.hpp"
#include "../opengl/mhe_gl.hpp"

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

class StencilState
{
public:
	void init(const StencilDesc& desc);
	void enable() const;
private:
	struct Desc
	{
		GLenum front_op_pass;
		GLenum front_op_dfail;
		GLenum front_op_sfail;
		GLenum front_compare_mode;
		GLint front_ref;
		GLint front_mask;
		bool enabled;
	};

	Desc desc_; 
};

class BlendState
{
public:
	void init(const BlendDesc& desc);
	void enable() const;
private:
	struct Desc
	{
		bool enabled;
		GLenum func;
		GLenum func_alpha;
		GLenum srcmode;
		GLenum dstmode;
		GLenum srcmode_alpha;
		GLenum dstmode_alpha;
	};

	Desc desc_;
};

class OpenGL3RenderState : public RenderStateImpl
{
public:
	bool init(const RenderStateDesc& desc);
	void close();

	void update(const RenderStateDesc& desc);

	void enable() const;
	void disable() const;
private:
	DepthState depth_state_;
	StencilState stencil_state_;
	BlendState blend_state_;
};

}}

#endif
