#ifndef __OPENGL3_DRIVER_HPP__
#define __OPENGL3_DRIVER_HPP__

#include "video_driver.hpp"
#include "opengl3_shader_program.hpp"
#include "opengl3_buffer.hpp"

namespace mhe {
namespace opengl {

class OpenGL3Driver : public DriverImpl
{
public:
private:
    bool init();
	void close() {}

	void set_ws(WindowSystem*) {}

	void save_view_matrix() {}
	void restore_view_matrix() {}
	void setup_view_matrix(const matrixf&) {}

	void load_projection_matrix(const matrixf&);
	void load_modelview_matrix(const matrixf&) {}

	void enable_lighting() {}
	void disable_lighting() {}
	void enable_blending();
	void disable_blending();
	void set_blend_func(BlendMode);

	void enable_depth() {}
	void disable_depth() {}
	void set_depth_func(DepthFunc) {}

	void clear_depth();
	void clear_color();

	void mask_zbuffer();
	void unmask_zbuffer();

	void set_clear_color(const colorf&);

	void save_current_color() {}
	void restore_color() {}
	void begin_draw(const float*, const float*, const float*, const float*,
						 cmn::uint) {}
	void begin_draw(boost::shared_ptr<Texture>,
						 const float*, const float*, const float*, const float*,
						 cmn::uint) {}
	void begin_draw(const RenderBuffer* buffer);
	void draw(const cmn::uint*, cmn::uint);
	void end_draw() {}
	void end_draw(boost::shared_ptr<Texture> /*texture*/) {}
	void end_draw(const RenderBuffer* buffer);

	void set_color(const colorf&) {}

	void get_display_data(std::vector<char>&) {}

	void set_viewport(int x, int y, int w, int h);

	void set_shader_program(const boost::shared_ptr<ShaderProgram>& program);
    
    void end_render();

	bool support_buffered_render() const
	{
		return true;
	}

	virtual RenderBuffer* create_render_buffer() const
	{
		return new OpenGLBuffer;
	}

	cmn::uint major_version_need() const
	{
		return 3;
	}

	cmn::uint minor_version_need() const
	{
		return 0;
	}
private:
	bool init_default_shader();

	boost::shared_ptr<OpenGL3ShaderProgram> active_shader_program_;
};

}}

#endif
