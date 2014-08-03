#ifndef __OPENGL3_RENDER_TARGET_HPP__
#define __OPENGL3_RENDER_TARGET_HPP__

#include "render/render_target.hpp"
#include "../opengl/mhe_gl.hpp"
#include "opengl3_texture.hpp"

namespace mhe {
namespace opengl {

class OpenGL3RenderTarget : public RenderTargetImpl
{
public:
	bool init(const RenderTargetDesc& desc);
	void close();

	void enable() const;
	void disable() const;
private:
	RenderTargetDesc desc_;
	Texture rt_[max_simultaneous_render_targets_number];
	Texture ds_;
	GLenum target_;
	GLuint id_;
};

}}

#endif
