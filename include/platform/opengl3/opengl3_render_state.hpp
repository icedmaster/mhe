#ifndef __OPENGL3_RENDER_STATE_HPP__
#define __OPENGL3_RENDER_STATE_HPP__

#include "render/render_state.hpp"
#include "../opengl/mhe_gl.hpp"

namespace mhe {
namespace opengl {

struct OpenGL3ContextState;

class DepthState
{
public:
	void init(const DepthDesc& desc);
    void enable(OpenGL3ContextState& state) const;
private:
	struct Desc
	{
		bool test_enabled;
		bool write_enabled;
	};
	Desc desc_;
};

class StencilState
{
public:
	void init(const StencilDesc& desc);
    void enable(OpenGL3ContextState& state) const;
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
    void enable(OpenGL3ContextState& state) const;
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

class RasterizerState
{
public:
	void init(const RasterizerDesc& desc);
	void enable(OpenGL3ContextState& state) const;
private:
	struct Desc
	{
		GLenum cull;
		GLenum winding;
		GLboolean color_mask[4];
	};

	Desc desc_;
};

class OpenGL3RenderState : public RenderStateImpl
{
public:
	bool init(const RenderStateDesc& desc);
	void close();

	void update(const RenderStateDesc& desc);
	void update_viewport(const ViewportDesc& viewport_desc) override;
	void update_scissor(const ScissorDesc& scissor_desc) override;
	void update_rasterizer(const RasterizerDesc& rasterizer_desc) override;

	void enable(OpenGL3ContextState& state) const;
	void disable() const;
private:
	void set_viewport(OpenGL3ContextState& state, const rect<int>& viewport) const;
	void set_scissor(OpenGL3ContextState& state, const ScissorDesc& scissor_desc) const;

	DepthState depth_state_;
	StencilState stencil_state_;
	BlendState blend_state_;
	RasterizerState rasterizer_state_;
	rect<int> viewport_;
	ScissorDesc scissor_state_;
};

}}

#endif
