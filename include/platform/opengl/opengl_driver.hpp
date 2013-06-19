#ifndef _OPENGL_DRIVER_HPP_
#define _OPENGL_DRIVER_HPP_

#include "video_driver.hpp"
#include "opengl_state_attr.hpp"
#include "mhe_gl.hpp"
#include "opengl_extension.hpp"
#include "texture.hpp"

namespace mhe {
namespace opengl {

class OpenGLDriver : public DriverImpl
{
private:
	float cur_color[4];
	bool using_texture_;
	bool using_color_;
	WindowSystem* ws_;
	rect<int> vp;	// viewport

	// state
	bool zbuffer_masked_;
private:
	bool init();
	void close();

	void set_ws(WindowSystem* ws)
	{
		ws_ = ws;
	}

	void save_view_matrix();
	void restore_view_matrix();
	void setup_view_matrix(const matrixf& m);

	void load_projection_matrix(const matrixf& m);
	void load_modelview_matrix(const matrixf& m);

	void enable_lighting();
	void disable_lighting();
	void enable_blending();
	void disable_blending();
	void set_blend_func(BlendMode bf);

	void enable_depth();
	void disable_depth();
	void set_depth_func(DepthFunc);

	void clear_depth();
	void clear_color();

	void mask_zbuffer();
	void unmask_zbuffer();

	void set_clear_color(const colorf& color);

	void save_current_color();
	void restore_color();
	void begin_draw(const float*, const float*, const float*, const float*,
						 cmn::uint);
	void begin_draw(boost::shared_ptr<Texture>,
						 const float*, const float*, const float*, const float*,
						 cmn::uint);

	void draw(const cmn::uint*, cmn::uint);
	void end_draw();
	void end_draw(boost::shared_ptr<Texture> texture);

	void set_color(const colorf&);

	void get_display_data(std::vector<char>& data);

	void set_viewport(int x, int y, int w, int h);

	void set_shader_program(const boost::shared_ptr<ShaderProgram>&)
	{}

	cmn::uint major_version_need() const
	{
		return 1;
	}

	cmn::uint minor_version_need() const
	{
		return 0;
	}
public:
	OpenGLDriver();

};

}}

#endif
