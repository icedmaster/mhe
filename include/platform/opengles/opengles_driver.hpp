#ifndef __OPENGLES_DRIVER_HPP__
#define __OPENGLES_DRIVER_HPP__

#include "video_driver.hpp"
#include "opengles_shader_program.hpp"

namespace mhe {
namespace opengl {

class OpenGLESDriver : public Driver
{
public:
private:
    bool init_impl();
	void close_impl() {}

	void set_ws(WindowSystem*) {}

	void save_view_matrix() {}
	void restore_view_matrix() {}
	void setup_view_matrix(const matrixf&) {}

	void load_projection_matrix(const matrixf&);
	void load_modelview_matrix(const matrixf&) {}

	void enable_lighting_impl() {}
	void disable_lighting_impl() {}
	void enable_blending_impl();
	void disable_blending_impl();
	void set_blend_func(BlendMode);

	void enable_depth_impl() {}
	void disable_depth_impl() {}
	void set_depth_func(DepthFunc) {}

	void clear_depth_impl();
	void clear_color_impl();

	void mask_zbuffer_impl();
	void unmask_zbuffer_impl();

	void set_clear_color_impl(const colorf&);

	void save_current_color() {}
	void restore_color() {}
	void begin_draw_impl(const float*, const float*, const float*, const float*,
						 cmn::uint);
	void begin_draw_impl(boost::shared_ptr<Texture>,
						 const float*, const float*, const float*, const float*,
						 cmn::uint) {}
	void draw_impl(const cmn::uint*, cmn::uint);
	void end_draw_impl();
	void end_draw_impl(boost::shared_ptr<Texture> /*texture*/) {}

	void set_color_impl(const colorf&) {}

	void get_display_data_impl(std::vector<char>&) {}

	void set_viewport_impl(int x, int y, int w, int h);

	void set_shader_program_impl(const boost::shared_ptr<ShaderProgram>& program);
    
    void end_render_impl();
private:
	bool init_default_shader();

	boost::shared_ptr<OpenGLESShaderProgram> active_shader_program_;
};

}}

#endif
