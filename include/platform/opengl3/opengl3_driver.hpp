#ifndef __OPENGL3_DRIVER_HPP__
#define __OPENGL3_DRIVER_HPP__

#include "render/video_driver.hpp"

namespace mhe {
namespace opengl {

class OpenGL3IndexBuffer;
class OpenGL3ShaderProgram;
class OpenGL3RenderTarget;

class OpenGL3Driver : public DriverImpl
{
public:
private:
    bool init();
	void close() {}

	void enable_blending();
	void disable_blending();

	void enable_depth() {}
	void disable_depth() {}

	void clear_depth();
	void clear_color();
	void clear_stencil();

	void set_clear_color(const colorf&);

	void set_viewport(int x, int y, int w, int h);

	uint major_version_need() const
	{
		return 3;
	}

	uint minor_version_need() const
	{
		return 3;
	}

	void set_state(const RenderState& state);
	void set_shader_program(const ShaderProgram& program);
	void set_vertex_buffer(const RenderBuffer& vbuffer);
	void set_index_buffer(const IndexBuffer& ibuffer);
	void set_uniform(const UniformBuffer& uniform);
	void set_layout(const Layout& layout);
	void set_texture(const Texture& texture, size_t unit);
	void set_render_target(const RenderTarget& render_target);
	void set_default_render_target();
	void draw(const RenderData& data);

	void flush();
private:
	const OpenGL3IndexBuffer* current_index_buffer_;
	const OpenGL3ShaderProgram* current_shader_program_;
	const OpenGL3RenderTarget* current_render_target_;
};

}}

#endif
