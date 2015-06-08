#ifndef __OPENGL3_DRIVER_HPP__
#define __OPENGL3_DRIVER_HPP__

#include "render/video_driver.hpp"
#include "core/array.hpp"
#include "core/types.hpp"
#include "../opengl/mhe_gl.hpp"

namespace mhe {
namespace opengl {

class OpenGL3IndexBuffer;
class OpenGL3ShaderProgram;
class OpenGL3RenderTarget;

struct OpenGL3ContextState
{
    array<size_t, 16> uniforms;
	vector2<int> window_size;
	rect<int> viewport;
    bool depth;
    bool stencil;
    bool blend;

    OpenGL3ContextState();
};

class OpenGL3Driver : public DriverImpl
{
public:
	OpenGL3Driver();
private:
    bool init(DriverRenderingCapabilities& caps);
	void close() {}

	void enable_blending();
	void disable_blending();

	void enable_depth() {}
	void disable_depth() {}

	void clear_depth();
	void clear_color();
	void clear_stencil();

	void set_clear_color(const colorf&);

	void set_window_size(const vector2<int>& size)
	{
		state_.window_size = size;
	}

	void set_viewport(int x, int y, int w, int h);

	uint major_version_need() const override
	{
		return 3;
	}

	uint minor_version_need() const override
	{
		return 3;
	}

	uint supported_versions(pair<uint, uint>* versions, uint size) const override;

	void set_state(const RenderState& state);
	void set_shader_program(const ShaderProgram& program);
	void set_vertex_buffer(const RenderBuffer& vbuffer);
	void set_index_buffer(const IndexBuffer& ibuffer);
    void set_uniform(const UniformBuffer& uniform, size_t unit);
	void set_layout(const Layout& layout);
	void set_texture(const Texture& texture, size_t unit);
	void set_render_target(const RenderTarget& render_target);
	void set_default_render_target();
	void draw(const RenderData& data);

	void flush();
private:
	void setup_caps(DriverRenderingCapabilities& caps);

	pair<uint, uint> versions_[4];
	OpenGL3ContextState state_;
	const OpenGL3IndexBuffer* current_index_buffer_;
	const OpenGL3ShaderProgram* current_shader_program_;
	const OpenGL3RenderTarget* current_render_target_;
};

}}

#endif
